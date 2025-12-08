#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "../core/ConfigManager.h"
#include "../core/Constants.h"

SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    connect(ui->BTN_Save, &QPushButton::clicked, this, &SettingsWindow::OnSaveButtonClicked);
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

    QString position = ConfigManager::get("overlayPosition");
    ui->SELECT_Position->setCurrentIndex(position.isEmpty() ? 0 : position.toInt());
}

void SettingsWindow::OnSaveButtonClicked()
{
    int padding = ui->INPUT_Padding->value();
    int opacity = ui->INPUT_Opacity->value();
    int position = ui->SELECT_Position->currentIndex();

    ConfigManager::set("overlayPosition", QString::number(position));
    ConfigManager::set("overlayOpacity", QString::number(opacity));
    ConfigManager::set("overlayPadding", QString::number(padding));

    this->close();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
