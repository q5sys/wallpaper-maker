//===========================================
//  wallpaper maker source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QSettings>
#include <QAction>
#include <QActionGroup>
#include <QSplitter>
#include "ImageViewer.h"
#include "ImageProcessor.h"
#include "SettingsDialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    // File operations
    void openImage();
    void saveWallpaper();
    void exit();
    
    // Edit operations
    void resetAdjustments();
    void resetView();
    
    // View operations
    void fitToWindow();
    void actualSize();
    void toggleCropOverlay();
    
    // Settings
    void showSettings();
    void resetToDefaults();
    
    // Image processing
    void onImageDropped(const QString &filePath);
    void onResolutionChanged();
    void onOutputFormatChanged();
    void onBrightnessChanged();
    void onContrastChanged();
    void onSaturationChanged();
    void onQualityChanged();
    
    // Image processor signals
    void onImageLoaded(const QString &filePath);
    void onImageProcessed();
    void onProcessingError(const QString &error);
    
    // UI updates
    void updateUI();
    void updateStatusBar();

private:
    // Central widget and layout
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_controlsLayout;
    QSplitter *m_splitter;
    
    // Image viewer
    ImageViewer *m_imageViewer;
    
    // Image processor
    ImageProcessor *m_imageProcessor;
    
    // Control panels
    QGroupBox *m_resolutionGroup;
    QGroupBox *m_formatGroup;
    QGroupBox *m_adjustmentsGroup;
    
    // Resolution controls
    QComboBox *m_resolutionComboBox;
    QLabel *m_resolutionLabel;
    
    // Format controls
    QComboBox *m_formatComboBox;
    QSlider *m_qualitySlider;
    QLabel *m_qualityLabel;
    
    // Adjustment controls
    QSlider *m_brightnessSlider;
    QSlider *m_contrastSlider;
    QSlider *m_saturationSlider;
    QLabel *m_brightnessLabel;
    QLabel *m_contrastLabel;
    QLabel *m_saturationLabel;
    
    // Action buttons
    QPushButton *m_openButton;
    QPushButton *m_saveButton;
    
    // Menu and toolbar
    QMenuBar *m_menuBar;
    QToolBar *m_toolBar;
    QStatusBar *m_statusBar;
    QProgressBar *m_progressBar;
    
    // Actions
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_exitAction;
    QAction *m_resetAdjustmentsAction;
    QAction *m_resetViewAction;
    QAction *m_fitToWindowAction;
    QAction *m_actualSizeAction;
    QAction *m_toggleCropOverlayAction;
    QAction *m_settingsAction;
    QAction *m_resetDefaultsAction;
    QAction *m_aboutAction;
    QAction *m_aboutQtAction;
    
    // Settings
    QSettings *m_settings;
    SettingsDialog *m_settingsDialog;
    
    // State
    QString m_currentImagePath;
    bool m_imageLoaded;
    
    // Setup methods
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
    void setupConnections();
    void setupResolutionComboBox();
    void setupFormatComboBox();
    void setupAdjustmentSliders();
    
    // Helper methods
    void loadSettings();
    void saveSettings();
    void updateControlsFromSettings();
    void updateImageProcessor();
    void showError(const QString &message);
    void showInfo(const QString &message);
    bool confirmUnsavedChanges();
    QString getLastDirectory() const;
    void setLastDirectory(const QString &directory);
    void enableControls(bool enabled);
};

#endif // MAINWINDOW_H
