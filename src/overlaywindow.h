#ifndef OVERLAYWINDOW_H
#define OVERLAYWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class OverlayWindow;
}
QT_END_NAMESPACE

class OverlayWindow : public QWidget
{
    Q_OBJECT

public:
    OverlayWindow(QWidget *parent = nullptr);
    ~OverlayWindow();

private:
    Ui::OverlayWindow *ui;
};
#endif // OVERLAYWINDOW_H
