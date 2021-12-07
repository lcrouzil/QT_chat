#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(QWidget *parent = nullptr);
    ~LogDialog();

private:
    Ui::LogDialog *ui;


private slots:
    void bpConnexion();

signals:
    void tryLogin(QString, QString);

};

#endif // LOGDIALOG_H
