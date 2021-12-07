#ifndef CLIENT_H
#define CLIENT_H


#include <QMainWindow>
#include <QObject>
#include <QTcpSocket>
#include <QArrayData>
#include<logdialog.h>
#include<dialnewdiscu.h>
#include <QCryptographicHash>
#include<QTextEdit>
#include<qdatetime.h>
#include <QPdfWriter>
#include <QPrinter>
#include <QFileDialog>



QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();


    void emssionJson(QJsonObject &);


private:
    Ui::Client *ui;
    QTcpSocket *sock;
    QString monPseudo;
    QString monPassword;

    LogDialog *logDialog;
    DialNewDiscu *DiscuDial;



    void jsonConnexion();
    void jsonMessageEnvoie(QString&, QString&);
    void jsonMessageReception(QJsonObject);
    void ajoutTexte(QString, QString);


signals:
    void returnPressed();
    void clicked();


public slots:
    void connexion();
    void serveurMeParle();
    void envoieServeur();
    void storeLog(QString, QString);
    void newConv();
    void newTabDiscu(QString);

private slots:
    void on_le_message_returnPressed();
    void on_pb_pdf_clicked();
};
#endif // CLIENT_H








