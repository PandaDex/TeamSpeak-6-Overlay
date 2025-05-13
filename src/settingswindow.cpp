#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "databasemanager.h"
#include <QDebug>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
    ui->INPUT_Opacity->setValue(DatabaseManager::get("overlayOpacity").toInt());
    if(DatabaseManager::get("overlayOpacity").isEmpty()) {
        ui->INPUT_Opacity->setValue(100);
    }
    ui->INPUT_Padding->setValue(DatabaseManager::get("overlayPadding").toInt());
    if(DatabaseManager::get("overlayPadding").isEmpty()) {
        ui->INPUT_Padding->setValue(10);
    }
    ui->SELECT_Position->setCurrentIndex(DatabaseManager::get("overlayPosition").toInt());
    if(DatabaseManager::get("overlayPosition").isEmpty()) {
        ui->SELECT_Position->setCurrentIndex(0);
    }
    connect(ui->BTN_Save, &QPushButton::clicked, this, &SettingsWindow::OnSaveButtonClicked);
}

void SettingsWindow::OnSaveButtonClicked()
{
    int padding = ui->INPUT_Padding->value();
    int opacity = ui->INPUT_Opacity->value();
    int position = ui->SELECT_Position->currentIndex();
    DatabaseManager::set("overlayPosition", QString::number(position));
    DatabaseManager::set("overlayOpacity", QString::number(opacity));
    DatabaseManager::set("overlayPadding ", QString::number(padding));
    this->close();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
