//===========================================
//  wallpaper maker source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#include "ImageViewer.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QFileInfo>
#include <QScrollBar>
#include <QApplication>
#include <QDebug>
#include <cmath>

ImageViewer::ImageViewer(QWidget *parent)
    : QGraphicsView(parent)
    , m_scene(nullptr)
    , m_pixmapItem(nullptr)
    , m_panning(false)
    , m_scaleFactor(1.0)
    , m_minScaleFactor(0.1)
    , m_maxScaleFactor(10.0)
    , m_showCropOverlay(false)
{
    setupScene();
    setAcceptDrops(true);
    setDragMode(QGraphicsView::NoDrag);
    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::SmoothPixmapTransform);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setFrameStyle(QFrame::NoFrame);
}

void ImageViewer::setupScene()
{
    m_scene = new QGraphicsScene(this);
    m_scene->setBackgroundBrush(QBrush(Qt::gray));
    setScene(m_scene);
    
    connect(m_scene, &QGraphicsScene::changed, this, &ImageViewer::onSceneChanged);
}

void ImageViewer::setImage(const QPixmap &pixmap)
{
    clearImage();
    
    if (pixmap.isNull()) {
        return;
    }
    
    m_pixmapItem = m_scene->addPixmap(pixmap);
    m_pixmapItem->setTransformationMode(Qt::SmoothTransformation);
    
    // Set a larger scene rect to allow full panning
    QRect imageRect = pixmap.rect();
    int margin = qMax(imageRect.width(), imageRect.height());
    QRect expandedRect = imageRect.adjusted(-margin, -margin, margin, margin);
    m_scene->setSceneRect(expandedRect);
    
    centerImage();
    
    emit viewChanged();
}

void ImageViewer::clearImage()
{
    if (m_pixmapItem) {
        m_scene->removeItem(m_pixmapItem);
        delete m_pixmapItem;
        m_pixmapItem = nullptr;
    }
    
    m_scene->clear();
    m_scaleFactor = 1.0;
    m_showCropOverlay = false;
    
    emit viewChanged();
}

void ImageViewer::setCropOverlay(const QSize &targetSize)
{
    m_cropSize = targetSize;
    updateCropOverlay();
    viewport()->update();
}

void ImageViewer::showCropOverlay(bool show)
{
    m_showCropOverlay = show;
    viewport()->update();
}

QRect ImageViewer::getCropRect() const
{
    if (!hasImage() || !m_showCropOverlay || m_cropRect.isEmpty()) {
        return QRect();
    }
    
    // Convert viewport crop rect to scene coordinates
    QPointF topLeft = QGraphicsView::mapToScene(m_cropRect.topLeft());
    QPointF bottomRight = QGraphicsView::mapToScene(m_cropRect.bottomRight());
    
    return QRect(topLeft.toPoint(), bottomRight.toPoint());
}

void ImageViewer::updateCropOverlay()
{
    if (!hasImage() || m_cropSize.isEmpty()) {
        return;
    }
    
    // Calculate crop rectangle in viewport coordinates (static position)
    QRect viewportRect = viewport()->rect();
    double targetAspect = static_cast<double>(m_cropSize.width()) / m_cropSize.height();
    
    // Calculate maximum crop size that fits in viewport
    int maxWidth = viewportRect.width() - 40; // Leave some margin
    int maxHeight = viewportRect.height() - 40;
    
    int cropWidth, cropHeight;
    if (targetAspect > 1.0) {
        // Landscape orientation
        cropWidth = qMin(maxWidth, static_cast<int>(maxHeight * targetAspect));
        cropHeight = static_cast<int>(cropWidth / targetAspect);
    } else {
        // Portrait orientation
        cropHeight = qMin(maxHeight, static_cast<int>(maxWidth / targetAspect));
        cropWidth = static_cast<int>(cropHeight * targetAspect);
    }
    
    // Center the crop rectangle in viewport
    int x = (viewportRect.width() - cropWidth) / 2;
    int y = (viewportRect.height() - cropHeight) / 2;
    
    // Store crop rect in viewport coordinates
    m_cropRect = QRect(x, y, cropWidth, cropHeight);
}

void ImageViewer::fitToWindow()
{
    if (!hasImage()) {
        return;
    }
    
    fitInView(m_pixmapItem, Qt::KeepAspectRatio);
    m_scaleFactor = transform().m11();
    emit viewChanged();
}

void ImageViewer::actualSize()
{
    if (!hasImage()) {
        return;
    }
    
    resetTransform();
    m_scaleFactor = 1.0;
    emit viewChanged();
}

void ImageViewer::resetView()
{
    if (!hasImage()) {
        return;
    }
    
    resetTransform();
    m_scaleFactor = 1.0;
    centerImage();
    emit viewChanged();
}

void ImageViewer::zoomIn()
{
    scaleView(1.25);
}

void ImageViewer::zoomOut()
{
    scaleView(0.8);
}

void ImageViewer::setZoomFactor(qreal factor)
{
    if (!hasImage()) {
        return;
    }
    
    factor = qBound(m_minScaleFactor, factor, m_maxScaleFactor);
    qreal scaleFactor = factor / m_scaleFactor;
    scaleView(scaleFactor);
}

void ImageViewer::scaleView(qreal scaleFactor)
{
    if (!hasImage()) {
        return;
    }
    
    qreal newScale = m_scaleFactor * scaleFactor;
    newScale = qBound(m_minScaleFactor, newScale, m_maxScaleFactor);
    
    if (qAbs(newScale - m_scaleFactor) < 0.001) {
        return; // No significant change
    }
    
    scaleFactor = newScale / m_scaleFactor;
    scale(scaleFactor, scaleFactor);
    m_scaleFactor = newScale;
    
    updateCropOverlay();
    emit viewChanged();
}

void ImageViewer::centerImage()
{
    if (!hasImage()) {
        return;
    }
    
    centerOn(m_pixmapItem);
}

QRect ImageViewer::getImageBounds() const
{
    if (!hasImage()) {
        return QRect();
    }
    
    return m_pixmapItem->boundingRect().toRect();
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
    if (!hasImage()) {
        QGraphicsView::wheelEvent(event);
        return;
    }
    
    const int numDegrees = event->angleDelta().y() / 8;
    const int numSteps = numDegrees / 15;
    
    if (numSteps > 0) {
        scaleView(std::pow(1.125, numSteps));
    } else if (numSteps < 0) {
        scaleView(std::pow(0.875, -numSteps));
    }
    
    event->accept();
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && hasImage()) {
        m_panning = true;
        m_lastPanPoint = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning && (event->buttons() & Qt::LeftButton)) {
        QPoint delta = event->pos() - m_lastPanPoint;
        m_lastPanPoint = event->pos();
        
        // Pan the view
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_panning) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void ImageViewer::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString filePath = urls.first().toLocalFile();
            if (isImageFile(filePath)) {
                event->acceptProposedAction();
                return;
            }
        }
    }
    
    event->ignore();
}

void ImageViewer::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}

void ImageViewer::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        QList<QUrl> urls = event->mimeData()->urls();
        if (!urls.isEmpty()) {
            QString filePath = urls.first().toLocalFile();
            if (isImageFile(filePath)) {
                emit imageDropped(filePath);
                event->acceptProposedAction();
                return;
            }
        }
    }
    
    event->ignore();
}

void ImageViewer::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);
    
    if (m_showCropOverlay && hasImage() && !m_cropRect.isEmpty()) {
        QPainter painter(viewport());
        drawCropOverlay(&painter);
    }
}

void ImageViewer::drawCropOverlay(QPainter *painter)
{
    if (m_cropRect.isEmpty()) {
        return;
    }
    
    // m_cropRect is already in viewport coordinates
    QRect viewportRect = m_cropRect;
    
    // Draw semi-transparent overlay outside crop area
    QRegion outsideRegion(viewport()->rect());
    outsideRegion = outsideRegion.subtracted(QRegion(viewportRect));
    
    painter->save();
    painter->setClipRegion(outsideRegion);
    painter->fillRect(viewport()->rect(), QColor(0, 0, 0, 128));
    painter->restore();
    
    // Draw crop rectangle border
    painter->save();
    QPen pen(Qt::white, 2, Qt::SolidLine);
    painter->setPen(pen);
    painter->drawRect(viewportRect);
    
    // Draw corner handles
    int handleSize = 8;
    QBrush handleBrush(Qt::white);
    painter->setBrush(handleBrush);
    
    // Top-left
    painter->drawRect(viewportRect.topLeft().x() - handleSize/2, 
                     viewportRect.topLeft().y() - handleSize/2, 
                     handleSize, handleSize);
    
    // Top-right
    painter->drawRect(viewportRect.topRight().x() - handleSize/2, 
                     viewportRect.topRight().y() - handleSize/2, 
                     handleSize, handleSize);
    
    // Bottom-left
    painter->drawRect(viewportRect.bottomLeft().x() - handleSize/2, 
                     viewportRect.bottomLeft().y() - handleSize/2, 
                     handleSize, handleSize);
    
    // Bottom-right
    painter->drawRect(viewportRect.bottomRight().x() - handleSize/2, 
                     viewportRect.bottomRight().y() - handleSize/2, 
                     handleSize, handleSize);
    
    painter->restore();
}

void ImageViewer::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    updateCropOverlay();
}

bool ImageViewer::isImageFile(const QString &filePath) const
{
    QStringList supportedExtensions = {
        "png", "jpg", "jpeg", "jfif", "heif", "heic", 
        "bmp", "tiff", "tif", "gif"
    };
    
    QString extension = QFileInfo(filePath).suffix().toLower();
    return supportedExtensions.contains(extension);
}

void ImageViewer::onSceneChanged()
{
    updateCropOverlay();
}
