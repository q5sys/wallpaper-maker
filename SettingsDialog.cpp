//===========================================
//  wallpaper maker source code
//  Copyright (c) 2025, jt(q5sys)
//  Available under the MIT license
//  See the LICENSE file for full details
//===========================================
#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSettings>
#include <QApplication>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_formatComboBox(nullptr)
    , m_qualitySlider(nullptr)
    , m_qualityLabel(nullptr)
    , m_rememberAdjustmentsCheckBox(nullptr)
    , m_autoFitCheckBox(nullptr)
    , m_resetButton(nullptr)
    , m_okButton(nullptr)
    , m_cancelButton(nullptr)
    , m_mainLayout(nullptr)
    , m_formLayout(nullptr)
    , m_buttonLayout(nullptr)
    , m_exportGroup(nullptr)
    , m_behaviorGroup(nullptr)
{
    setWindowTitle("Preferences");
    setModal(true);
    resize(400, 300);
    
    setupUI();
    setupConnections();
    loadSettings();
}

void SettingsDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // Export settings group
    m_exportGroup = new QGroupBox("Export Settings", this);
    m_formLayout = new QFormLayout(m_exportGroup);
    
    // Default format combo box
    m_formatComboBox = new QComboBox(this);
    populateFormatComboBox();
    m_formLayout->addRow("Default Export Format:", m_formatComboBox);
    
    // JPEG quality slider
    m_qualitySlider = new QSlider(Qt::Horizontal, this);
    m_qualitySlider->setRange(1, 100);
    m_qualitySlider->setValue(85);
    m_qualitySlider->setTickPosition(QSlider::TicksBelow);
    m_qualitySlider->setTickInterval(10);
    
    m_qualityLabel = new QLabel("85%", this);
    m_qualityLabel->setMinimumWidth(40);
    
    QHBoxLayout *qualityLayout = new QHBoxLayout();
    qualityLayout->addWidget(m_qualitySlider);
    qualityLayout->addWidget(m_qualityLabel);
    
    m_formLayout->addRow("JPEG Quality:", qualityLayout);
    
    m_mainLayout->addWidget(m_exportGroup);
    
    // Behavior settings group
    m_behaviorGroup = new QGroupBox("Behavior Settings", this);
    QVBoxLayout *behaviorLayout = new QVBoxLayout(m_behaviorGroup);
    
    m_rememberAdjustmentsCheckBox = new QCheckBox("Remember last used adjustments", this);
    m_autoFitCheckBox = new QCheckBox("Auto-fit image when loaded", this);
    
    behaviorLayout->addWidget(m_rememberAdjustmentsCheckBox);
    behaviorLayout->addWidget(m_autoFitCheckBox);
    
    m_mainLayout->addWidget(m_behaviorGroup);
    
    // Button layout
    m_buttonLayout = new QHBoxLayout();
    
    m_resetButton = new QPushButton("Reset to Defaults", this);
    m_buttonLayout->addWidget(m_resetButton);
    
    m_buttonLayout->addStretch();
    
    m_okButton = new QPushButton("OK", this);
    m_cancelButton = new QPushButton("Cancel", this);
    
    m_okButton->setDefault(true);
    
    m_buttonLayout->addWidget(m_okButton);
    m_buttonLayout->addWidget(m_cancelButton);
    
    m_mainLayout->addLayout(m_buttonLayout);
    
    // Initially hide quality slider for non-JPEG formats
    onFormatChanged();
}

void SettingsDialog::setupConnections()
{
    connect(m_formatComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::onFormatChanged);
    
    connect(m_qualitySlider, &QSlider::valueChanged,
            this, &SettingsDialog::onQualityChanged);
    
    connect(m_resetButton, &QPushButton::clicked,
            this, &SettingsDialog::resetToDefaults);
    
    connect(m_okButton, &QPushButton::clicked,
            this, &SettingsDialog::onAccepted);
    
    connect(m_cancelButton, &QPushButton::clicked,
            this, &SettingsDialog::onRejected);
}

void SettingsDialog::populateFormatComboBox()
{
    m_formatComboBox->clear();
    
    m_formatComboBox->addItem("PNG (Portable Network Graphics)", 
                             static_cast<int>(ImageProcessor::OutputFormat::PNG));
    m_formatComboBox->addItem("JPEG (Joint Photographic Experts Group)", 
                             static_cast<int>(ImageProcessor::OutputFormat::JPEG));
    m_formatComboBox->addItem("BMP (Windows Bitmap)", 
                             static_cast<int>(ImageProcessor::OutputFormat::BMP));
    m_formatComboBox->addItem("TIFF (Tagged Image File Format)", 
                             static_cast<int>(ImageProcessor::OutputFormat::TIFF));
}

ImageProcessor::OutputFormat SettingsDialog::getDefaultExportFormat() const
{
    int index = m_formatComboBox->currentData().toInt();
    return static_cast<ImageProcessor::OutputFormat>(index);
}

int SettingsDialog::getDefaultJpegQuality() const
{
    return m_qualitySlider->value();
}

bool SettingsDialog::getRememberAdjustments() const
{
    return m_rememberAdjustmentsCheckBox->isChecked();
}

bool SettingsDialog::getAutoFitOnLoad() const
{
    return m_autoFitCheckBox->isChecked();
}

void SettingsDialog::setDefaultExportFormat(ImageProcessor::OutputFormat format)
{
    int index = m_formatComboBox->findData(static_cast<int>(format));
    if (index >= 0) {
        m_formatComboBox->setCurrentIndex(index);
    }
}

void SettingsDialog::setDefaultJpegQuality(int quality)
{
    m_qualitySlider->setValue(qBound(1, quality, 100));
}

void SettingsDialog::setRememberAdjustments(bool remember)
{
    m_rememberAdjustmentsCheckBox->setChecked(remember);
}

void SettingsDialog::setAutoFitOnLoad(bool autoFit)
{
    m_autoFitCheckBox->setChecked(autoFit);
}

void SettingsDialog::loadSettings()
{
    QSettings settings;
    
    // Load default export format
    int formatValue = settings.value("DefaultExportFormat", 
                                   static_cast<int>(ImageProcessor::OutputFormat::PNG)).toInt();
    setDefaultExportFormat(static_cast<ImageProcessor::OutputFormat>(formatValue));
    
    // Load JPEG quality
    int quality = settings.value("JpegQuality", 85).toInt();
    setDefaultJpegQuality(quality);
    
    // Load behavior settings
    bool rememberAdjustments = settings.value("RememberAdjustments", true).toBool();
    setRememberAdjustments(rememberAdjustments);
    
    bool autoFit = settings.value("AutoFitOnLoad", true).toBool();
    setAutoFitOnLoad(autoFit);
}

void SettingsDialog::saveSettings()
{
    QSettings settings;
    
    // Save default export format
    settings.setValue("DefaultExportFormat", static_cast<int>(getDefaultExportFormat()));
    
    // Save JPEG quality
    settings.setValue("JpegQuality", getDefaultJpegQuality());
    
    // Save behavior settings
    settings.setValue("RememberAdjustments", getRememberAdjustments());
    settings.setValue("AutoFitOnLoad", getAutoFitOnLoad());
}

void SettingsDialog::resetToDefaults()
{
    setDefaultExportFormat(ImageProcessor::OutputFormat::PNG);
    setDefaultJpegQuality(85);
    setRememberAdjustments(true);
    setAutoFitOnLoad(true);
}

void SettingsDialog::onFormatChanged()
{
    ImageProcessor::OutputFormat format = getDefaultExportFormat();
    bool isJpeg = (format == ImageProcessor::OutputFormat::JPEG);
    
    // Show/hide quality controls based on format
    m_qualitySlider->setVisible(isJpeg);
    m_qualityLabel->setVisible(isJpeg);
    
    // Update the form layout
    QWidget *qualityWidget = m_formLayout->itemAt(1)->widget();
    if (qualityWidget) {
        qualityWidget->setVisible(isJpeg);
    }
    
    updateQualityLabel();
}

void SettingsDialog::onQualityChanged()
{
    updateQualityLabel();
}

void SettingsDialog::updateQualityLabel()
{
    m_qualityLabel->setText(QString("%1%").arg(m_qualitySlider->value()));
}

void SettingsDialog::onAccepted()
{
    saveSettings();
    accept();
}

void SettingsDialog::onRejected()
{
    loadSettings(); // Revert changes
    reject();
}
