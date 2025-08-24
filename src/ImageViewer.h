#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
//===========================================
//  wallpaper maker source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QRect>
#include <QSize>

class ImageViewer : public QGraphicsView
{
    Q_OBJECT

public:
    explicit ImageViewer(QWidget *parent = nullptr);

    // Image management
    void setImage(const QPixmap &pixmap);
    void clearImage();
    bool hasImage() const { return m_pixmapItem != nullptr; }
    
    // Crop overlay
    void setCropOverlay(const QSize &targetSize);
    void showCropOverlay(bool show);
    QRect getCropRect() const;
    
    // View controls
    void fitToWindow();
    void actualSize();
    void resetView();
    
    // Zoom controls
    void zoomIn();
    void zoomOut();
    void setZoomFactor(qreal factor);
    qreal getZoomFactor() const { return m_scaleFactor; }

signals:
    void imageDropped(const QString &filePath);
    void viewChanged();

protected:
    // Event handlers
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onSceneChanged();

private:
    QGraphicsScene *m_scene;
    QGraphicsPixmapItem *m_pixmapItem;
    
    // Pan functionality
    bool m_panning;
    QPoint m_lastPanPoint;
    
    // Zoom functionality
    qreal m_scaleFactor;
    qreal m_minScaleFactor;
    qreal m_maxScaleFactor;
    
    // Crop overlay
    bool m_showCropOverlay;
    QSize m_cropSize;
    QRect m_cropRect;
    
    // Helper methods
    void setupScene();
    void updateCropOverlay();
    void drawCropOverlay(QPainter *painter);
    bool isImageFile(const QString &filePath) const;
    void scaleView(qreal scaleFactor);
    void centerImage();
    QPoint mapToScene(const QPoint &point) const;
    QRect getImageBounds() const;
};

#endif // IMAGEVIEWER_H
