#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <client.h>
namespace Ui {
class loginWindow;
}

class loginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit loginWindow(QWidget *parent = nullptr);
    ~loginWindow();

private:
    Ui::loginWindow *ui;
};

#endif // LOGINWINDOW_H
