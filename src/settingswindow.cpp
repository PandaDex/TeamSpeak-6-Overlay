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

    connect(ui->BTN_Save, &QPushButton::clicked, this, &SettingsWindow::OnSaveButtonClicked);
}

void SettingsWindow::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    loadSettings();
}

void SettingsWindow::loadSettings()
{
    QString opacity = DatabaseManager::get("overlayOpacity");
    ui->INPUT_Opacity->setValue(opacity.isEmpty() ? 100 : opacity.toInt());

    QString padding = DatabaseManager::get("overlayPadding");
    ui->INPUT_Padding->setValue(padding.isEmpty() ? 10 : padding.toInt());

    QString position = DatabaseManager::get("overlayPosition");
    ui->SELECT_Position->setCurrentIndex(position.isEmpty() ? 0 : position.toInt());
}

void SettingsWindow::OnSaveButtonClicked()
{
    int padding = ui->INPUT_Padding->value();
    int opacity = ui->INPUT_Opacity->value();
    int position = ui->SELECT_Position->currentIndex();

    DatabaseManager::set("overlayPosition", QString::number(position));
    DatabaseManager::set("overlayOpacity", QString::number(opacity));
    DatabaseManager::set("overlayPadding", QString::number(padding));

    this->close();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}
