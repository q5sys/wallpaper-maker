//===========================================
//  wallpaper maker source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QSize>
#include <QMap>

class ImageProcessor : public QObject
{
    Q_OBJECT

public:
    enum class OutputFormat {
        PNG,
        JPEG,
        BMP,
        TIFF
    };

    struct FormatInfo {
        QString extension;
        QString description;
        bool supportsQuality;
        bool supportsTransparency;
    };

    struct Resolution {
        int width;
        int height;
        QString name;
        QString description;
        bool isVertical;
        
        Resolution(int w, int h, const QString &n, const QString &d, bool vertical = false)
            : width(w), height(h), name(n), description(d), isVertical(vertical) {}
    };

    explicit ImageProcessor(QObject *parent = nullptr);

    // Image loading and format detection
    bool loadImage(const QString &filePath);
    QString detectInputFormat(const QString &filePath);
    QImage getCurrentImage() const { return m_currentImage; }
    QPixmap getCurrentPixmap() const { return QPixmap::fromImage(m_currentImage); }
    
    // Image adjustments
    void setBrightness(int brightness); // -100 to 100
    void setContrast(int contrast);     // -100 to 100
    void setSaturation(int saturation); // -100 to 100
    QImage applyAdjustments(const QImage &image) const;
    
    // Output format handling
    void setOutputFormat(OutputFormat format);
    void setJpegQuality(int quality); // 1-100
    OutputFormat getOutputFormat() const { return m_outputFormat; }
    int getJpegQuality() const { return m_jpegQuality; }
    
    // Cropping and scaling
    QImage cropAndScale(const QImage &image, const QSize &targetSize, const QRect &cropRect) const;
    
    // File operations
    bool saveImage(const QString &basePath, const QString &suffix, const QImage &image);
    QString getOutputExtension(OutputFormat format) const;
    
    // Static utility methods
    static QList<Resolution> getSupportedResolutions();
    static QMap<QString, OutputFormat> getDefaultOutputMapping();
    static FormatInfo getFormatInfo(OutputFormat format);
    static bool isFormatSupported(const QString &extension);
    static QStringList getSupportedInputExtensions();
    static QStringList getSupportedOutputExtensions();

signals:
    void imageLoaded(const QString &filePath);
    void imageProcessed();
    void errorOccurred(const QString &error);

private:
    QImage m_originalImage;
    QImage m_currentImage;
    QString m_currentFilePath;
    
    // Image adjustments
    int m_brightness;
    int m_contrast;
    int m_saturation;
    
    // Output settings
    OutputFormat m_outputFormat;
    int m_jpegQuality;
    
    // Static data
    static QList<Resolution> s_resolutions;
    static QMap<OutputFormat, FormatInfo> s_outputFormats;
    static QMap<QString, OutputFormat> s_defaultMapping;
    
    void initializeStaticData();
    QImage adjustBrightness(const QImage &image, int brightness) const;
    QImage adjustContrast(const QImage &image, int contrast) const;
    QImage adjustSaturation(const QImage &image, int saturation) const;
};

#endif // IMAGEPROCESSOR_H
