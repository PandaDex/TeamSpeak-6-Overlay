#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "../core/ConfigManager.h"
#include "../core/Constants.h"
#include "../core/OverlayPosition.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->BTN_Save, &QPushButton::clicked, this, &SettingsWindow::OnSaveButtonClicked);
    connect(ui->SELECT_Position, &QComboBox::currentIndexChanged, this, &SettingsWindow::updateCustomPositionInputs);

    ui->INPUT_CustomX->setToolTip("Overlay X position in pixels, relative to the top-left corner of the primary screen");
    ui->INPUT_CustomY->setToolTip("Overlay Y position in pixels, relative to the top-left corner of the primary screen");
    ui->SELECT_DisplayMode->setToolTip("Show only speaking clients, or everyone in your current channel with speakers highlighted");
}

void SettingsWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    loadSettings();
}

void SettingsWindow::loadSettings()
{
    QString opacity = ConfigManager::get("overlayOpacity");
    ui->INPUT_Opacity->setValue(opacity.isEmpty() ? Constants::DEFAULT_OPACITY : opacity.toInt());

    QString padding = ConfigManager::get("overlayPadding");
    ui->INPUT_Padding->setValue(padding.isEmpty() ? Constants::DEFAULT_PADDING : padding.toInt());

    ui->SELECT_Position->setCurrentIndex(OverlayPosition::fromString(ConfigManager::get("overlayPosition")));
    ui->INPUT_CustomX->setValue(ConfigManager::get("overlayCustomX").toInt());
    ui->INPUT_CustomY->setValue(ConfigManager::get("overlayCustomY").toInt());
    ui->SELECT_DisplayMode->setCurrentIndex(DisplayMode::fromString(ConfigManager::get("displayMode")));

    updateCustomPositionInputs();
}

void SettingsWindow::updateCustomPositionInputs()
{
    const bool custom = ui->SELECT_Position->currentIndex() == OverlayPosition::Custom;
    ui->INPUT_CustomX->setEnabled(custom);
    ui->INPUT_CustomY->setEnabled(custom);
    ui->LABEL_CustomX->setEnabled(custom);
    ui->LABEL_CustomY->setEnabled(custom);
}

void SettingsWindow::OnSaveButtonClicked()
{
    int padding = ui->INPUT_Padding->value();
    int opacity = ui->INPUT_Opacity->value();
    int position = ui->SELECT_Position->currentIndex();

    // Custom coordinates first, so the position change already uses them
    ConfigManager::set("overlayCustomX", QString::number(ui->INPUT_CustomX->value()));
    ConfigManager::set("overlayCustomY", QString::number(ui->INPUT_CustomY->value()));
    ConfigManager::set("displayMode", QString::number(ui->SELECT_DisplayMode->currentIndex()));
    ConfigManager::set("overlayPosition", QString::number(position));
    ConfigManager::set("overlayOpacity", QString::number(opacity));
    ConfigManager::set("overlayPadding", QString::number(padding));

    this->close();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
