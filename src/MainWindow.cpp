//===========================================
//  wallpaper maker source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#include "MainWindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QProgressBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QAction>
#include <QActionGroup>
#include <QCloseEvent>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_mainLayout(nullptr)
    , m_controlsLayout(nullptr)
    , m_splitter(nullptr)
    , m_imageViewer(nullptr)
    , m_imageProcessor(nullptr)
    , m_resolutionGroup(nullptr)
    , m_formatGroup(nullptr)
    , m_adjustmentsGroup(nullptr)
    , m_resolutionComboBox(nullptr)
    , m_resolutionLabel(nullptr)
    , m_formatComboBox(nullptr)
    , m_qualitySlider(nullptr)
    , m_qualityLabel(nullptr)
    , m_brightnessSlider(nullptr)
    , m_contrastSlider(nullptr)
    , m_saturationSlider(nullptr)
    , m_brightnessLabel(nullptr)
    , m_contrastLabel(nullptr)
    , m_saturationLabel(nullptr)
    , m_openButton(nullptr)
    , m_saveButton(nullptr)
    , m_menuBar(nullptr)
    , m_toolBar(nullptr)
    , m_statusBar(nullptr)
    , m_progressBar(nullptr)
    , m_settings(nullptr)
    , m_settingsDialog(nullptr)
    , m_imageLoaded(false)
{
    setWindowTitle("WallpaperMaker");
    setMinimumSize(800, 600);
    resize(1200, 800);
    
    // Initialize components
    m_imageProcessor = new ImageProcessor(this);
    m_settings = new QSettings(this);
    
    setupUI();
    setupMenuBar();
    setupToolBar();
    setupStatusBar();
    setupConnections();
    
    loadSettings();
    updateUI();
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(6, 6, 6, 6);
    m_mainLayout->setSpacing(6);
    
    // Create splitter for main content
    m_splitter = new QSplitter(Qt::Horizontal, this);
    m_mainLayout->addWidget(m_splitter);
    
    // Create image viewer
    m_imageViewer = new ImageViewer(this);
    m_imageViewer->setMinimumSize(400, 300);
    m_splitter->addWidget(m_imageViewer);
    
    // Create controls panel
    QWidget *controlsWidget = new QWidget(this);
    controlsWidget->setMaximumWidth(300);
    controlsWidget->setMinimumWidth(250);
    
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(6, 6, 6, 6);
    controlsLayout->setSpacing(12);
    
    // Action buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_openButton = new QPushButton("Open Image", this);
    m_saveButton = new QPushButton("Save Wallpaper", this);
    m_saveButton->setEnabled(false);
    
    buttonLayout->addWidget(m_openButton);
    buttonLayout->addWidget(m_saveButton);
    controlsLayout->addLayout(buttonLayout);
    
    // Resolution group
    setupResolutionComboBox();
    controlsLayout->addWidget(m_resolutionGroup);
    
    // Format group
    setupFormatComboBox();
    controlsLayout->addWidget(m_formatGroup);
    
    // Adjustments group
    setupAdjustmentSliders();
    controlsLayout->addWidget(m_adjustmentsGroup);
    
    controlsLayout->addStretch();
    
    m_splitter->addWidget(controlsWidget);
    
    // Set splitter proportions
    m_splitter->setSizes({800, 300});
    m_splitter->setStretchFactor(0, 1);
    m_splitter->setStretchFactor(1, 0);
}

void MainWindow::setupResolutionComboBox()
{
    m_resolutionGroup = new QGroupBox("Resolution", this);
    QVBoxLayout *layout = new QVBoxLayout(m_resolutionGroup);
    
    // Add vertical orientation checkbox
    m_verticalCheckBox = new QCheckBox("Vertical (Portrait)", this);
    m_verticalCheckBox->setToolTip("Toggle between horizontal (landscape) and vertical (portrait) orientations");
    layout->addWidget(m_verticalCheckBox);
    
    m_resolutionComboBox = new QComboBox(this);
    
    // Populate with supported resolutions (initially horizontal only)
    populateResolutionComboBox(false);
    
    // Set default to Full HD
    int fullHdIndex = m_resolutionComboBox->findText("Full HD*", Qt::MatchStartsWith);
    if (fullHdIndex >= 0) {
        m_resolutionComboBox->setCurrentIndex(fullHdIndex);
    }
    
    layout->addWidget(m_resolutionComboBox);
    
    m_resolutionLabel = new QLabel("Select target resolution", this);
    m_resolutionLabel->setWordWrap(true);
    m_resolutionLabel->setStyleSheet("color: gray; font-size: 11px;");
    layout->addWidget(m_resolutionLabel);
}

void MainWindow::setupFormatComboBox()
{
    m_formatGroup = new QGroupBox("Output Format", this);
    QVBoxLayout *layout = new QVBoxLayout(m_formatGroup);
    
    m_formatComboBox = new QComboBox(this);
    m_formatComboBox->addItem("PNG (Lossless)", static_cast<int>(ImageProcessor::OutputFormat::PNG));
    m_formatComboBox->addItem("JPEG (Compressed)", static_cast<int>(ImageProcessor::OutputFormat::JPEG));
    m_formatComboBox->addItem("BMP (Uncompressed)", static_cast<int>(ImageProcessor::OutputFormat::BMP));
    m_formatComboBox->addItem("TIFF (Professional)", static_cast<int>(ImageProcessor::OutputFormat::TIFF));
    
    layout->addWidget(m_formatComboBox);
    
    // Quality slider (for JPEG)
    QHBoxLayout *qualityLayout = new QHBoxLayout();
    QLabel *qualityTextLabel = new QLabel("Quality:", this);
    m_qualitySlider = new QSlider(Qt::Horizontal, this);
    m_qualitySlider->setRange(1, 100);
    m_qualitySlider->setValue(85);
    m_qualityLabel = new QLabel("85%", this);
    m_qualityLabel->setMinimumWidth(35);
    
    qualityLayout->addWidget(qualityTextLabel);
    qualityLayout->addWidget(m_qualitySlider);
    qualityLayout->addWidget(m_qualityLabel);
    
    layout->addLayout(qualityLayout);
    
    // Initially hide quality controls
    qualityTextLabel->setVisible(false);
    m_qualitySlider->setVisible(false);
    m_qualityLabel->setVisible(false);
}

void MainWindow::setupAdjustmentSliders()
{
    m_adjustmentsGroup = new QGroupBox("Image Adjustments (Coming Soon)", this);
    QGridLayout *layout = new QGridLayout(m_adjustmentsGroup);
    
    // Brightness
    layout->addWidget(new QLabel("Brightness:", this), 0, 0);
    m_brightnessSlider = new QSlider(Qt::Horizontal, this);
    m_brightnessSlider->setRange(-100, 100);
    m_brightnessSlider->setValue(0);
    m_brightnessSlider->setEnabled(false); // Disable for now
    layout->addWidget(m_brightnessSlider, 0, 1);
    m_brightnessLabel = new QLabel("0", this);
    m_brightnessLabel->setMinimumWidth(30);
    layout->addWidget(m_brightnessLabel, 0, 2);
    
    // Contrast
    layout->addWidget(new QLabel("Contrast:", this), 1, 0);
    m_contrastSlider = new QSlider(Qt::Horizontal, this);
    m_contrastSlider->setRange(-100, 100);
    m_contrastSlider->setValue(0);
    m_contrastSlider->setEnabled(false); // Disable for now
    layout->addWidget(m_contrastSlider, 1, 1);
    m_contrastLabel = new QLabel("0", this);
    m_contrastLabel->setMinimumWidth(30);
    layout->addWidget(m_contrastLabel, 1, 2);
    
    // Saturation
    layout->addWidget(new QLabel("Saturation:", this), 2, 0);
    m_saturationSlider = new QSlider(Qt::Horizontal, this);
    m_saturationSlider->setRange(-100, 100);
    m_saturationSlider->setValue(0);
    m_saturationSlider->setEnabled(false); // Disable for now
    layout->addWidget(m_saturationSlider, 2, 1);
    m_saturationLabel = new QLabel("0", this);
    m_saturationLabel->setMinimumWidth(30);
    layout->addWidget(m_saturationLabel, 2, 2);
    
    // Reset button
    QPushButton *resetButton = new QPushButton("Reset", this);
    resetButton->setEnabled(false); // Disable for now
    layout->addWidget(resetButton, 3, 0, 1, 3);
    
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetAdjustments);
}

void MainWindow::setupMenuBar()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");
    
    m_openAction = new QAction("&Open Image...", this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip("Open an image file");
    fileMenu->addAction(m_openAction);
    
    m_saveAction = new QAction("&Save Wallpaper...", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip("Save the wallpaper");
    m_saveAction->setEnabled(false);
    fileMenu->addAction(m_saveAction);
    
    fileMenu->addSeparator();
    
    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("Exit the application");
    fileMenu->addAction(m_exitAction);
    
    // Edit menu
    QMenu *editMenu = menuBar()->addMenu("&Edit");
    
    m_resetAdjustmentsAction = new QAction("Reset &Adjustments", this);
    m_resetAdjustmentsAction->setStatusTip("Reset all image adjustments");
    editMenu->addAction(m_resetAdjustmentsAction);
    
    m_resetViewAction = new QAction("Reset &View", this);
    m_resetViewAction->setStatusTip("Reset zoom and position");
    editMenu->addAction(m_resetViewAction);
    
    // View menu
    QMenu *viewMenu = menuBar()->addMenu("&View");
    
    m_fitToWindowAction = new QAction("&Fit to Window", this);
    m_fitToWindowAction->setShortcut(Qt::CTRL | Qt::Key_0);
    m_fitToWindowAction->setStatusTip("Fit image to window");
    viewMenu->addAction(m_fitToWindowAction);
    
    m_actualSizeAction = new QAction("&Actual Size", this);
    m_actualSizeAction->setShortcut(Qt::CTRL | Qt::Key_1);
    m_actualSizeAction->setStatusTip("Show image at actual size");
    viewMenu->addAction(m_actualSizeAction);
    
    viewMenu->addSeparator();
    
    m_toggleCropOverlayAction = new QAction("Show &Crop Overlay", this);
    m_toggleCropOverlayAction->setCheckable(true);
    m_toggleCropOverlayAction->setChecked(true);
    m_toggleCropOverlayAction->setStatusTip("Toggle crop overlay visibility");
    viewMenu->addAction(m_toggleCropOverlayAction);
    
    // Settings menu
    QMenu *settingsMenu = menuBar()->addMenu("&Settings");
    
    m_settingsAction = new QAction("&Preferences...", this);
    m_settingsAction->setStatusTip("Open preferences dialog");
    settingsMenu->addAction(m_settingsAction);
    
    m_resetDefaultsAction = new QAction("&Reset to Defaults", this);
    m_resetDefaultsAction->setStatusTip("Reset all settings to defaults");
    settingsMenu->addAction(m_resetDefaultsAction);
    
    // Help menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");
    
    m_aboutAction = new QAction("&About WallpaperMaker", this);
    m_aboutAction->setStatusTip("About this application");
    helpMenu->addAction(m_aboutAction);
    
    m_aboutQtAction = new QAction("About &Qt", this);
    m_aboutQtAction->setStatusTip("About Qt");
    helpMenu->addAction(m_aboutQtAction);
}

void MainWindow::setupToolBar()
{
    m_toolBar = addToolBar("Main");
    m_toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    
    m_toolBar->addAction(m_openAction);
    m_toolBar->addAction(m_saveAction);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_fitToWindowAction);
    m_toolBar->addAction(m_actualSizeAction);
}

void MainWindow::setupStatusBar()
{
    m_statusBar = statusBar();
    
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    m_statusBar->addPermanentWidget(m_progressBar);
    
    m_statusBar->showMessage("Ready");
}

void MainWindow::setupConnections()
{
    // File actions
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openImage);
    connect(m_openButton, &QPushButton::clicked, this, &MainWindow::openImage);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::saveWallpaper);
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::saveWallpaper);
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::exit);
    
    // Edit actions
    connect(m_resetAdjustmentsAction, &QAction::triggered, this, &MainWindow::resetAdjustments);
    connect(m_resetViewAction, &QAction::triggered, this, &MainWindow::resetView);
    
    // View actions
    connect(m_fitToWindowAction, &QAction::triggered, this, &MainWindow::fitToWindow);
    connect(m_actualSizeAction, &QAction::triggered, this, &MainWindow::actualSize);
    connect(m_toggleCropOverlayAction, &QAction::triggered, this, &MainWindow::toggleCropOverlay);
    
    // Settings actions
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::showSettings);
    connect(m_resetDefaultsAction, &QAction::triggered, this, &MainWindow::resetToDefaults);
    
    // Help actions
    connect(m_aboutAction, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "About WallpaperMaker",
            "WallpaperMaker v1.0.0\n\n"
            "A simple tool for creating custom wallpapers\n"
            "by cropping and scaling images to specific resolutions.\n\n"
            "Built with Qt6 and C++");
    });
    connect(m_aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    
    // Control connections
    connect(m_resolutionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onResolutionChanged);
    connect(m_verticalCheckBox, &QCheckBox::toggled, this, &MainWindow::onVerticalOrientationChanged);
    connect(m_formatComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onOutputFormatChanged);
    connect(m_qualitySlider, &QSlider::valueChanged, this, &MainWindow::onQualityChanged);
    
    // Adjustment sliders
    connect(m_brightnessSlider, &QSlider::valueChanged, this, &MainWindow::onBrightnessChanged);
    connect(m_contrastSlider, &QSlider::valueChanged, this, &MainWindow::onContrastChanged);
    connect(m_saturationSlider, &QSlider::valueChanged, this, &MainWindow::onSaturationChanged);
    
    // Image viewer connections
    connect(m_imageViewer, &ImageViewer::imageDropped, this, &MainWindow::onImageDropped);
    connect(m_imageViewer, &ImageViewer::viewChanged, this, &MainWindow::updateStatusBar);
    
    // Image processor connections
    connect(m_imageProcessor, &ImageProcessor::imageLoaded, this, &MainWindow::onImageLoaded);
    connect(m_imageProcessor, &ImageProcessor::imageProcessed, this, &MainWindow::onImageProcessed);
    connect(m_imageProcessor, &ImageProcessor::errorOccurred, this, &MainWindow::onProcessingError);
}

void MainWindow::loadSettings()
{
    // Window geometry
    restoreGeometry(m_settings->value("geometry").toByteArray());
    restoreState(m_settings->value("windowState").toByteArray());
    
    // Splitter state
    m_splitter->restoreState(m_settings->value("splitterState").toByteArray());
    
    // Vertical orientation preference
    bool isVertical = m_settings->value("VerticalOrientation", false).toBool();
    m_verticalCheckBox->setChecked(isVertical);
    
    // Repopulate resolution combo box with correct orientation
    populateResolutionComboBox(isVertical);
    
    // Last resolution
    int lastResolution = m_settings->value("LastResolution", 2).toInt(); // Default to Full HD
    if (lastResolution >= 0 && lastResolution < m_resolutionComboBox->count()) {
        m_resolutionComboBox->setCurrentIndex(lastResolution);
    }
    
    // Default format from settings dialog
    int defaultFormat = m_settings->value("DefaultExportFormat", 
                                         static_cast<int>(ImageProcessor::OutputFormat::PNG)).toInt();
    int formatIndex = m_formatComboBox->findData(defaultFormat);
    if (formatIndex >= 0) {
        m_formatComboBox->setCurrentIndex(formatIndex);
    }
    
    // JPEG quality
    int quality = m_settings->value("JpegQuality", 85).toInt();
    m_qualitySlider->setValue(quality);
    
    // Remember adjustments if enabled
    bool rememberAdjustments = m_settings->value("RememberAdjustments", true).toBool();
    if (rememberAdjustments) {
        m_brightnessSlider->setValue(m_settings->value("ImageAdjustments/Brightness", 0).toInt());
        m_contrastSlider->setValue(m_settings->value("ImageAdjustments/Contrast", 0).toInt());
        m_saturationSlider->setValue(m_settings->value("ImageAdjustments/Saturation", 0).toInt());
    }
    
    // Crop overlay visibility
    bool showCropOverlay = m_settings->value("ShowCropOverlay", true).toBool();
    m_toggleCropOverlayAction->setChecked(showCropOverlay);
}

void MainWindow::saveSettings()
{
    // Window geometry
    m_settings->setValue("geometry", saveGeometry());
    m_settings->setValue("windowState", saveState());
    
    // Splitter state
    m_settings->setValue("splitterState", m_splitter->saveState());
    
    // Current selections
    m_settings->setValue("LastResolution", m_resolutionComboBox->currentIndex());
    
    // Remember adjustments if enabled
    bool rememberAdjustments = m_settings->value("RememberAdjustments", true).toBool();
    if (rememberAdjustments) {
        m_settings->setValue("ImageAdjustments/Brightness", m_brightnessSlider->value());
        m_settings->setValue("ImageAdjustments/Contrast", m_contrastSlider->value());
        m_settings->setValue("ImageAdjustments/Saturation", m_saturationSlider->value());
    }
    
    // Crop overlay visibility
    m_settings->setValue("ShowCropOverlay", m_toggleCropOverlayAction->isChecked());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

// File operations
void MainWindow::openImage()
{
    QString lastDir = getLastDirectory();
    QString filter = "Image Files (*.png *.jpg *.jpeg *.jfif *.heif *.heic *.bmp *.tiff *.tif *.gif);;";
    filter += "JPEG Files (*.jpg *.jpeg *.jfif);;";
    filter += "PNG Files (*.png);;";
    filter += "HEIF Files (*.heif *.heic);;";
    filter += "TIFF Files (*.tiff *.tif);;";
    filter += "BMP Files (*.bmp);;";
    filter += "GIF Files (*.gif);;";
    filter += "All Files (*.*)";
    
    QString filePath = QFileDialog::getOpenFileName(this, "Open Image", lastDir, filter);
    if (!filePath.isEmpty()) {
        setLastDirectory(QFileInfo(filePath).absolutePath());
        if (m_imageProcessor->loadImage(filePath)) {
            m_currentImagePath = filePath;
        }
    }
}

void MainWindow::saveWallpaper()
{
    if (!m_imageLoaded) {
        return;
    }
    
    QSize targetSize = m_resolutionComboBox->currentData().toSize();
    QRect cropRect = m_imageViewer->getCropRect();
    
    if (cropRect.isEmpty()) {
        showError("No crop area defined. Please select a resolution first.");
        return;
    }
    
    // Apply adjustments and crop/scale
    QImage processedImage = m_imageProcessor->applyAdjustments(m_imageProcessor->getCurrentImage());
    QImage finalImage = m_imageProcessor->cropAndScale(processedImage, targetSize, cropRect);
    
    // Generate suffix
    QString suffix = QString("_%1x%2").arg(targetSize.width()).arg(targetSize.height());
    
    // Save the image
    if (m_imageProcessor->saveImage(m_currentImagePath, suffix, finalImage)) {
        showInfo("Wallpaper saved successfully!");
    }
}

void MainWindow::exit()
{
    close();
}

// Edit operations
void MainWindow::resetAdjustments()
{
    m_brightnessSlider->setValue(0);
    m_contrastSlider->setValue(0);
    m_saturationSlider->setValue(0);
}

void MainWindow::resetView()
{
    m_imageViewer->resetView();
}

// View operations
void MainWindow::fitToWindow()
{
    m_imageViewer->fitToWindow();
}

void MainWindow::actualSize()
{
    m_imageViewer->actualSize();
}

void MainWindow::toggleCropOverlay()
{
    bool show = m_toggleCropOverlayAction->isChecked();
    m_imageViewer->showCropOverlay(show);
}

// Settings
void MainWindow::showSettings()
{
    if (!m_settingsDialog) {
        m_settingsDialog = new SettingsDialog(this);
    }
    
    if (m_settingsDialog->exec() == QDialog::Accepted) {
        updateControlsFromSettings();
    }
}

void MainWindow::resetToDefaults()
{
    int ret = QMessageBox::question(this, "Reset to Defaults",
                                   "This will reset all settings to their default values. Continue?",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        m_settings->clear();
        resetAdjustments();
        m_resolutionComboBox->setCurrentIndex(2); // Full HD
        m_formatComboBox->setCurrentIndex(0); // PNG
        m_qualitySlider->setValue(85);
        m_toggleCropOverlayAction->setChecked(true);
        updateUI();
    }
}

// Image processing slots
void MainWindow::onImageDropped(const QString &filePath)
{
    setLastDirectory(QFileInfo(filePath).absolutePath());
    if (m_imageProcessor->loadImage(filePath)) {
        m_currentImagePath = filePath;
    }
}

void MainWindow::onResolutionChanged()
{
    if (m_imageLoaded) {
        QSize targetSize = m_resolutionComboBox->currentData().toSize();
        m_imageViewer->setCropOverlay(targetSize);
        m_imageViewer->showCropOverlay(m_toggleCropOverlayAction->isChecked());
    }
    updateStatusBar();
}

void MainWindow::onOutputFormatChanged()
{
    int formatValue = m_formatComboBox->currentData().toInt();
    ImageProcessor::OutputFormat format = static_cast<ImageProcessor::OutputFormat>(formatValue);
    
    m_imageProcessor->setOutputFormat(format);
    
    // Show/hide quality controls for JPEG
    bool isJpeg = (format == ImageProcessor::OutputFormat::JPEG);
    
    // Find quality controls in the format group
    QList<QWidget*> widgets = m_formatGroup->findChildren<QWidget*>();
    for (QWidget *widget : widgets) {
        if (widget->objectName().contains("quality") || 
            qobject_cast<QSlider*>(widget) == m_qualitySlider ||
            qobject_cast<QLabel*>(widget) == m_qualityLabel) {
            widget->setVisible(isJpeg);
        }
    }
    
    // Also check the parent layout
    QWidget *parentWidget = qobject_cast<QWidget*>(m_qualitySlider->parent());
    if (parentWidget) {
        QHBoxLayout *qualityLayout = qobject_cast<QHBoxLayout*>(parentWidget->layout());
        if (qualityLayout) {
            for (int i = 0; i < qualityLayout->count(); ++i) {
                QWidget *widget = qualityLayout->itemAt(i)->widget();
                if (widget) {
                    widget->setVisible(isJpeg);
                }
            }
        }
    }
}

void MainWindow::onBrightnessChanged()
{
    int value = m_brightnessSlider->value();
    m_brightnessLabel->setText(QString::number(value));
    m_imageProcessor->setBrightness(value);
}

void MainWindow::onContrastChanged()
{
    int value = m_contrastSlider->value();
    m_contrastLabel->setText(QString::number(value));
    m_imageProcessor->setContrast(value);
}

void MainWindow::onSaturationChanged()
{
    int value = m_saturationSlider->value();
    m_saturationLabel->setText(QString::number(value));
    m_imageProcessor->setSaturation(value);
}

void MainWindow::onQualityChanged()
{
    int value = m_qualitySlider->value();
    m_qualityLabel->setText(QString("%1%").arg(value));
    m_imageProcessor->setJpegQuality(value);
}

// Image processor signal handlers
void MainWindow::onImageLoaded(const QString &filePath)
{
    m_imageLoaded = true;
    m_imageViewer->setImage(m_imageProcessor->getCurrentPixmap());
    
    // Auto-fit if enabled
    bool autoFit = m_settings->value("AutoFitOnLoad", true).toBool();
    if (autoFit) {
        m_imageViewer->fitToWindow();
    }
    
    // Update crop overlay
    onResolutionChanged();
    
    updateUI();
    m_statusBar->showMessage(QString("Loaded: %1").arg(QFileInfo(filePath).fileName()));
}

void MainWindow::onImageProcessed()
{
    if (m_imageLoaded) {
        m_imageViewer->setImage(m_imageProcessor->getCurrentPixmap());
        // Restore crop overlay after image update
        QSize targetSize = m_resolutionComboBox->currentData().toSize();
        m_imageViewer->setCropOverlay(targetSize);
        m_imageViewer->showCropOverlay(m_toggleCropOverlayAction->isChecked());
    }
}

void MainWindow::onProcessingError(const QString &error)
{
    showError(error);
}

// UI updates
void MainWindow::updateUI()
{
    bool hasImage = m_imageLoaded;
    
    // Enable/disable controls
    enableControls(hasImage);
    
    // Update format controls visibility
    onOutputFormatChanged();
    
    // Update status bar
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    if (!m_imageLoaded) {
        m_statusBar->showMessage("Ready - Drag an image here or use File > Open");
        return;
    }
    
    QSize imageSize = m_imageProcessor->getCurrentImage().size();
    QSize targetSize = m_resolutionComboBox->currentData().toSize();
    qreal zoomFactor = m_imageViewer->getZoomFactor();
    
    QString message = QString("Image: %1×%2 | Target: %3×%4 | Zoom: %5%")
                     .arg(imageSize.width())
                     .arg(imageSize.height())
                     .arg(targetSize.width())
                     .arg(targetSize.height())
                     .arg(qRound(zoomFactor * 100));
    
    m_statusBar->showMessage(message);
}

void MainWindow::updateControlsFromSettings()
{
    // Update format combo box from settings
    int defaultFormat = m_settings->value("DefaultExportFormat", 
                                         static_cast<int>(ImageProcessor::OutputFormat::PNG)).toInt();
    int formatIndex = m_formatComboBox->findData(defaultFormat);
    if (formatIndex >= 0) {
        m_formatComboBox->setCurrentIndex(formatIndex);
    }
    
    // Update quality slider
    int quality = m_settings->value("JpegQuality", 85).toInt();
    m_qualitySlider->setValue(quality);
}

void MainWindow::updateImageProcessor()
{
    // Update processor with current settings
    int formatValue = m_formatComboBox->currentData().toInt();
    m_imageProcessor->setOutputFormat(static_cast<ImageProcessor::OutputFormat>(formatValue));
    m_imageProcessor->setJpegQuality(m_qualitySlider->value());
    m_imageProcessor->setBrightness(m_brightnessSlider->value());
    m_imageProcessor->setContrast(m_contrastSlider->value());
    m_imageProcessor->setSaturation(m_saturationSlider->value());
}

// Helper methods
void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, "Error", message);
}

void MainWindow::showInfo(const QString &message)
{
    QMessageBox::information(this, "Information", message);
}

bool MainWindow::confirmUnsavedChanges()
{
    // For now, we don't track unsaved changes
    return true;
}

QString MainWindow::getLastDirectory() const
{
    return m_settings->value("LastDirectory", 
                            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
}

void MainWindow::setLastDirectory(const QString &directory)
{
    m_settings->setValue("LastDirectory", directory);
}

void MainWindow::enableControls(bool enabled)
{
    m_saveButton->setEnabled(enabled);
    m_saveAction->setEnabled(enabled);
    m_resolutionGroup->setEnabled(enabled);
    m_formatGroup->setEnabled(enabled);
    m_adjustmentsGroup->setEnabled(enabled);
    m_resetAdjustmentsAction->setEnabled(enabled);
    m_resetViewAction->setEnabled(enabled);
    m_fitToWindowAction->setEnabled(enabled);
    m_actualSizeAction->setEnabled(enabled);
    m_toggleCropOverlayAction->setEnabled(enabled);
}

void MainWindow::populateResolutionComboBox(bool vertical)
{
    m_resolutionComboBox->clear();
    
    QList<ImageProcessor::Resolution> resolutions = ImageProcessor::getSupportedResolutions();
    for (const auto &resolution : resolutions) {
        // Filter resolutions based on orientation
        if (resolution.isVertical == vertical) {
            QString text = QString("%1 (%2×%3)")
                          .arg(resolution.name)
                          .arg(resolution.width)
                          .arg(resolution.height);
            m_resolutionComboBox->addItem(text, QSize(resolution.width, resolution.height));
        }
    }
    
    // Set default to Full HD (or Full HD Portrait for vertical)
    QString searchText = vertical ? "Full HD Portrait*" : "Full HD*";
    int defaultIndex = m_resolutionComboBox->findText(searchText, Qt::MatchStartsWith);
    if (defaultIndex >= 0) {
        m_resolutionComboBox->setCurrentIndex(defaultIndex);
    } else if (m_resolutionComboBox->count() > 0) {
        // Fallback to first item if Full HD not found
        m_resolutionComboBox->setCurrentIndex(0);
    }
}

void MainWindow::onVerticalOrientationChanged()
{
    bool isVertical = m_verticalCheckBox->isChecked();
    
    // Remember current selection name to try to match it
    QString currentName;
    if (m_resolutionComboBox->currentIndex() >= 0) {
        QString currentText = m_resolutionComboBox->currentText();
        // Extract the name part (before the parentheses)
        int parenIndex = currentText.indexOf('(');
        if (parenIndex > 0) {
            currentName = currentText.left(parenIndex).trimmed();
        }
    }
    
    // Repopulate combo box with appropriate orientation
    populateResolutionComboBox(isVertical);
    
    // Try to find a matching resolution in the new orientation
    if (!currentName.isEmpty()) {
        QString targetName = currentName;
        if (isVertical && !currentName.contains("Portrait")) {
            targetName += " Portrait";
        } else if (!isVertical && currentName.contains("Portrait")) {
            targetName = currentName.replace(" Portrait", "");
        }
        
        int matchIndex = m_resolutionComboBox->findText(targetName + "*", Qt::MatchStartsWith);
        if (matchIndex >= 0) {
            m_resolutionComboBox->setCurrentIndex(matchIndex);
        }
    }
    
    // Update crop overlay if image is loaded
    if (m_imageLoaded) {
        onResolutionChanged();
    }
    
    // Save the vertical preference
    m_settings->setValue("VerticalOrientation", isVertical);
}
