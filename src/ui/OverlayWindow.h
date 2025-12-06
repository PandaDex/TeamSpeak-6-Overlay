#ifndef OVERLAYWINDOW_H
#define OVERLAYWINDOW_H

#include <QWidget>
#include <qboxlayout.h>

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
    QWidget* getLayoutContainer();

protected:
    void showEvent(QShowEvent *event) override;

private:
    Ui::OverlayWindow *ui;
};
#endif // OVERLAYWINDOW_H
