//===========================================
//  wallpaper maker source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSettings>
#include "ImageProcessor.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    // Settings access
    ImageProcessor::OutputFormat getDefaultExportFormat() const;
    int getDefaultJpegQuality() const;
    bool getRememberAdjustments() const;
    bool getAutoFitOnLoad() const;
    
    // Settings modification
    void setDefaultExportFormat(ImageProcessor::OutputFormat format);
    void setDefaultJpegQuality(int quality);
    void setRememberAdjustments(bool remember);
    void setAutoFitOnLoad(bool autoFit);

public slots:
    void loadSettings();
    void saveSettings();
    void resetToDefaults();

private slots:
    void onFormatChanged();
    void onQualityChanged();
    void onAccepted();
    void onRejected();

private:
    // UI components
    QComboBox *m_formatComboBox;
    QSlider *m_qualitySlider;
    QLabel *m_qualityLabel;
    QCheckBox *m_rememberAdjustmentsCheckBox;
    QCheckBox *m_autoFitCheckBox;
    QPushButton *m_resetButton;
    QPushButton *m_okButton;
    QPushButton *m_cancelButton;
    
    // Layout
    QVBoxLayout *m_mainLayout;
    QFormLayout *m_formLayout;
    QHBoxLayout *m_buttonLayout;
    QGroupBox *m_exportGroup;
    QGroupBox *m_behaviorGroup;
    
    void setupUI();
    void setupConnections();
    void updateQualityLabel();
    void populateFormatComboBox();
};

#endif // SETTINGSDIALOG_H
