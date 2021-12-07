#ifndef DIALNEWDISCU_H
#define DIALNEWDISCU_H

#include <QDialog>

namespace Ui {
class DialNewDiscu;
}

class DialNewDiscu : public QDialog
{
    Q_OBJECT

public:
    explicit DialNewDiscu(QWidget *parent = nullptr);
    ~DialNewDiscu();

private slots:
    void on_pb_newConver_clicked();

signals:
    void newDiscu(QString);

private:
    Ui::DialNewDiscu *ui;
};

#endif // DIALNEWDISCU_H
