//===========================================
//  wallpaper maker source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#include "ImageProcessor.h"
#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QDir>
#include <QDebug>
#include <QColorSpace>
#include <cmath>

// Static member initialization
QList<ImageProcessor::Resolution> ImageProcessor::s_resolutions;
QMap<ImageProcessor::OutputFormat, ImageProcessor::FormatInfo> ImageProcessor::s_outputFormats;
QMap<QString, ImageProcessor::OutputFormat> ImageProcessor::s_defaultMapping;

ImageProcessor::ImageProcessor(QObject *parent)
    : QObject(parent)
    , m_brightness(0)
    , m_contrast(0)
    , m_saturation(0)
    , m_outputFormat(OutputFormat::PNG)
    , m_jpegQuality(85)
{
    initializeStaticData();
}

void ImageProcessor::initializeStaticData()
{
    if (!s_resolutions.isEmpty()) {
        return; // Already initialized
    }
    
    // Initialize supported resolutions
    s_resolutions = {
        {1280, 720, "HD", "1280 × 720 (HD)"},
        {1366, 768, "WXGA", "1366 × 768 (WXGA)"},
        {1920, 1080, "Full HD", "1920 × 1080 (Full HD)"},
        {2560, 1440, "QHD", "2560 × 1440 (QHD)"},
        {3840, 2160, "4K UHD", "3840 × 2160 (4K UHD)"},
        {4096, 2160, "Cinema 4K", "4096 × 2160 (Cinema 4K)"},
        {7680, 4320, "8K UHD", "7680 × 4320 (8K UHD)"},
        {1920, 1200, "WUXGA", "1920 × 1200 (WUXGA)"},
        {2560, 1600, "WQXGA", "2560 × 1600 (WQXGA)"},
        {3840, 2400, "WQUXGA", "3840 × 2400 (WQUXGA)"},
        {2256, 1504, "3:2 Tablet", "2256 × 1504 (3:2 Tablet)"},
        {2880, 1920, "3:2 High-res", "2880 × 1920 (3:2 High-res)"},
        {3000, 2000, "3:2 Ultra", "3000 × 2000 (3:2 Ultra)"},
        {2560, 1080, "Ultrawide", "2560 × 1080 (Ultrawide)"},
        {3440, 1440, "Ultrawide QHD", "3440 × 1440 (Ultrawide QHD)"},
        {5120, 2160, "5K Ultrawide", "5120 × 2160 (5K Ultrawide)"},
        {5120, 1440, "Super Ultrawide", "5120 × 1440 (Super Ultrawide)"}
    };
    
    // Initialize output format info
    s_outputFormats = {
        {OutputFormat::PNG, {"png", "PNG (Portable Network Graphics)", false, true}},
        {OutputFormat::JPEG, {"jpg", "JPEG (Joint Photographic Experts Group)", true, false}},
        {OutputFormat::BMP, {"bmp", "BMP (Windows Bitmap)", false, false}},
        {OutputFormat::TIFF, {"tiff", "TIFF (Tagged Image File Format)", false, true}}
    };
    
    // Initialize default format mapping
    s_defaultMapping = {
        {"jpg", OutputFormat::JPEG},
        {"jpeg", OutputFormat::JPEG},
        {"jfif", OutputFormat::JPEG},
        {"png", OutputFormat::PNG},
        {"bmp", OutputFormat::BMP},
        {"tiff", OutputFormat::TIFF},
        {"tif", OutputFormat::TIFF},
        {"heif", OutputFormat::JPEG},  // Convert to JPEG for compatibility
        {"heic", OutputFormat::JPEG},  // Convert to JPEG for compatibility
        {"gif", OutputFormat::PNG}     // Convert to PNG to preserve quality
    };
}

bool ImageProcessor::loadImage(const QString &filePath)
{
    QImageReader reader(filePath);
    if (!reader.canRead()) {
        emit errorOccurred(QString("Cannot read image file: %1").arg(filePath));
        return false;
    }
    
    QImage image = reader.read();
    if (image.isNull()) {
        emit errorOccurred(QString("Failed to load image: %1").arg(reader.errorString()));
        return false;
    }
    
    m_originalImage = image;
    m_currentImage = image;
    m_currentFilePath = filePath;
    
    // Set default output format based on input format
    QString extension = QFileInfo(filePath).suffix().toLower();
    if (s_defaultMapping.contains(extension)) {
        m_outputFormat = s_defaultMapping[extension];
    }
    
    emit imageLoaded(filePath);
    return true;
}

QString ImageProcessor::detectInputFormat(const QString &filePath)
{
    return QFileInfo(filePath).suffix().toLower();
}

void ImageProcessor::setBrightness(int brightness)
{
    m_brightness = qBound(-100, brightness, 100);
    m_currentImage = applyAdjustments(m_originalImage);
    emit imageProcessed();
}

void ImageProcessor::setContrast(int contrast)
{
    m_contrast = qBound(-100, contrast, 100);
    m_currentImage = applyAdjustments(m_originalImage);
    emit imageProcessed();
}

void ImageProcessor::setSaturation(int saturation)
{
    m_saturation = qBound(-100, saturation, 100);
    m_currentImage = applyAdjustments(m_originalImage);
    emit imageProcessed();
}

QImage ImageProcessor::applyAdjustments(const QImage &image) const
{
    QImage result = image;
    
    if (m_brightness != 0) {
        result = adjustBrightness(result, m_brightness);
    }
    
    if (m_contrast != 0) {
        result = adjustContrast(result, m_contrast);
    }
    
    if (m_saturation != 0) {
        result = adjustSaturation(result, m_saturation);
    }
    
    return result;
}

QImage ImageProcessor::adjustBrightness(const QImage &image, int brightness) const
{
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    int adjustment = brightness * 255 / 100;
    
    for (int y = 0; y < result.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            QRgb pixel = line[x];
            int r = qBound(0, qRed(pixel) + adjustment, 255);
            int g = qBound(0, qGreen(pixel) + adjustment, 255);
            int b = qBound(0, qBlue(pixel) + adjustment, 255);
            line[x] = qRgba(r, g, b, qAlpha(pixel));
        }
    }
    
    return result;
}

QImage ImageProcessor::adjustContrast(const QImage &image, int contrast) const
{
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    double factor = (259.0 * (contrast + 255.0)) / (255.0 * (259.0 - contrast));
    
    for (int y = 0; y < result.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            QRgb pixel = line[x];
            int r = qBound(0, static_cast<int>(factor * (qRed(pixel) - 128) + 128), 255);
            int g = qBound(0, static_cast<int>(factor * (qGreen(pixel) - 128) + 128), 255);
            int b = qBound(0, static_cast<int>(factor * (qBlue(pixel) - 128) + 128), 255);
            line[x] = qRgba(r, g, b, qAlpha(pixel));
        }
    }
    
    return result;
}

QImage ImageProcessor::adjustSaturation(const QImage &image, int saturation) const
{
    QImage result = image.convertToFormat(QImage::Format_ARGB32);
    double factor = (saturation + 100.0) / 100.0;
    
    for (int y = 0; y < result.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(result.scanLine(y));
        for (int x = 0; x < result.width(); ++x) {
            QRgb pixel = line[x];
            int r = qRed(pixel);
            int g = qGreen(pixel);
            int b = qBlue(pixel);
            
            // Convert to grayscale
            int gray = static_cast<int>(0.299 * r + 0.587 * g + 0.114 * b);
            
            // Apply saturation
            r = qBound(0, static_cast<int>(gray + factor * (r - gray)), 255);
            g = qBound(0, static_cast<int>(gray + factor * (g - gray)), 255);
            b = qBound(0, static_cast<int>(gray + factor * (b - gray)), 255);
            
            line[x] = qRgba(r, g, b, qAlpha(pixel));
        }
    }
    
    return result;
}

void ImageProcessor::setOutputFormat(OutputFormat format)
{
    m_outputFormat = format;
}

void ImageProcessor::setJpegQuality(int quality)
{
    m_jpegQuality = qBound(1, quality, 100);
}

QImage ImageProcessor::cropAndScale(const QImage &image, const QSize &targetSize, const QRect &cropRect) const
{
    // First crop the image
    QImage cropped = image.copy(cropRect);
    
    // Then scale to target size
    return cropped.scaled(targetSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

bool ImageProcessor::saveImage(const QString &basePath, const QString &suffix, const QImage &image)
{
    QFileInfo fileInfo(basePath);
    QString extension = getOutputExtension(m_outputFormat);
    QString outputPath = QString("%1/%2%3.%4")
                        .arg(fileInfo.absolutePath())
                        .arg(fileInfo.baseName())
                        .arg(suffix)
                        .arg(extension);
    
    QImageWriter writer(outputPath);
    
    if (m_outputFormat == OutputFormat::JPEG) {
        writer.setQuality(m_jpegQuality);
    }
    
    if (!writer.write(image)) {
        emit errorOccurred(QString("Failed to save image: %1").arg(writer.errorString()));
        return false;
    }
    
    return true;
}

QString ImageProcessor::getOutputExtension(OutputFormat format) const
{
    return s_outputFormats[format].extension;
}

// Static methods
QList<ImageProcessor::Resolution> ImageProcessor::getSupportedResolutions()
{
    ImageProcessor temp; // Ensure static data is initialized
    return s_resolutions;
}

QMap<QString, ImageProcessor::OutputFormat> ImageProcessor::getDefaultOutputMapping()
{
    ImageProcessor temp; // Ensure static data is initialized
    return s_defaultMapping;
}

ImageProcessor::FormatInfo ImageProcessor::getFormatInfo(OutputFormat format)
{
    ImageProcessor temp; // Ensure static data is initialized
    return s_outputFormats[format];
}

bool ImageProcessor::isFormatSupported(const QString &extension)
{
    ImageProcessor temp; // Ensure static data is initialized
    return s_defaultMapping.contains(extension.toLower());
}

QStringList ImageProcessor::getSupportedInputExtensions()
{
    ImageProcessor temp; // Ensure static data is initialized
    return s_defaultMapping.keys();
}

QStringList ImageProcessor::getSupportedOutputExtensions()
{
    ImageProcessor temp; // Ensure static data is initialized
    QStringList extensions;
    for (auto it = s_outputFormats.begin(); it != s_outputFormats.end(); ++it) {
        extensions << it.value().extension;
    }
    return extensions;
}
