#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QShowEvent>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void OnSaveButtonClicked();
    void loadSettings();
    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
