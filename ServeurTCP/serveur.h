#ifndef SERVEUR_H
#define SERVEUR_H

#include <QMainWindow>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include<QDebug>
#include<QFile>
#include<QJsonObject>
#include <QJsonArray>
#include<QJsonDocument>
#include <QDebug>
#include<QDataStream>
#include<QByteArray>
#include<QMap>
#include<QDateTime>
#include <QCryptographicHash>

QT_BEGIN_NAMESPACE
namespace Ui { class Serveur; }
QT_END_NAMESPACE

class Serveur : public QMainWindow
{
    Q_OBJECT

public:
    Serveur(QWidget *parent = nullptr);
    ~Serveur();




    void authentification(QJsonObject&);
    void authentificationReponse();
    void receptionJson();
    void emissionJson(QTcpSocket *, QJsonObject &);
    void envoieMessage(QJsonObject&);



private:
    Ui::Serveur *ui;
    QTcpServer* mServeur;
    QList<QTcpSocket*> mListeSocketClient;
    QMap<QString, QString> *inscrit; // liste d'inscrit

signals:

public slots:

private slots:
    void nouvelleConnexion();
    void deconnexionClient();
    void clientMeParle();


};
#endif // SERVEUR_H
