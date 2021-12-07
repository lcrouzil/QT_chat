#include "serveur.h"
#include "ui_serveur.h"

Serveur::Serveur(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Serveur)
{
    ui->setupUi(this);
    mServeur = new QTcpServer(this);
    //QList<QTcpSocket*> mListeSocketClient;
    this->inscrit = new QMap<QString,QString> ;
    this->inscrit->insert("larenzu","48c8947f69c054a5caa934674ce8881d02bb18fb59d5a63eeaddff735b0e9801e87294783281ae49fc8287a0fd86779b27d7972d3e84f0fa0d826d7cb67dfefc");//123
    this->inscrit->insert("toto","b2b54dae057c26e515a6fec9d6f863d538f0a81c0e96c834c153ab63b98b07f8666963728a844edef66d94149868438146d7d09fe9595d37b196dd39e8a6ef16");//mdp123

    if(!mServeur->listen(QHostAddress::Any,8585))
    {
        qDebug() <<"le serveur ne démarre pas";
        this->ui->lb_info->setText("le serveur ne démarre pas");
    }
    else
    {
        connect(mServeur,SIGNAL(newConnection()),this,SLOT(nouvelleConnexion()));
        qDebug() <<"ca marche !!!";
        this->ui->lb_info->setText("le serveur a bien démarré sur le port 8585");
    }
}

Serveur::~Serveur()
{
    delete ui;
}

/**
 * nouvelle connexion de client
 *
 * @brief Serveur::nouvelleConnexion
 */
void Serveur::nouvelleConnexion()
{
    QTcpSocket *nouveauClient = mServeur->nextPendingConnection();
    mListeSocketClient << nouveauClient;
    qDebug() <<"nouvelle co";
    qDebug() <<mListeSocketClient;
    qDebug() <<"liste des inscrit"<<*inscrit;

    connect(nouveauClient, SIGNAL(readyRead()), this, SLOT(clientMeParle()));
    connect(nouveauClient, SIGNAL(disconnected()), this, SLOT(deconnexionClient()));


}

/**
 * deconnexion du client
 *
 * @brief Serveur::deconnexionClient
 */
void Serveur::deconnexionClient()
{
    QTcpSocket * sockClient = (QTcpSocket*)sender();
    mListeSocketClient.removeOne(sockClient);
    qDebug()<<"déconexion client";
    qDebug() <<mListeSocketClient;
    //delete sockClient;

}

/**
 * authentification de l'appelant
 *
 * @brief Serveur::authentification
 * @param messageJsonObject
 */
void Serveur::authentification(QJsonObject &messageJsonObject)
{
    QString messageLogin = messageJsonObject.value("login").toString();
    QString messagePassword = messageJsonObject.value("password").toString();

    QJsonObject reponseConnexion;
    reponseConnexion.insert("action","answerlogin");

    //identifiant de l'appelant
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());


    //verification concordance login mdp
    QString motDePasseEnregistre = this->inscrit->value(messageLogin);
    qDebug() << motDePasseEnregistre;
    qDebug() << messagePassword ;

    if (motDePasseEnregistre == messagePassword) //verification ok
    {


        //récupération du login pour connaitre le client
        socket->setProperty("login",messageLogin);

        reponseConnexion.insert("state","ok");
        qDebug()<<"vérification ok";

    }

    else //verification echoué
    {
        reponseConnexion.insert("state","errorID");
        qDebug()<<"erreur identification";

    }
    this->emissionJson(socket,reponseConnexion);
}

/**
 * emission json
 *
 * @brief Serveur::emissionJson
 * @param s
 * @param message
 */
void Serveur::emissionJson(QTcpSocket *s,QJsonObject &message)
{


    //transformation de reponseConnexion en docJSON
    QJsonDocument docReponseConnexion(message);

    //transformation de reponseConnexionen Qstring
    QString strReponseConnexion(docReponseConnexion.toJson(QJsonDocument::Compact));

    //transformation de reponseConnexion en QByteArray
    QByteArray qbaReponseConnexion=strReponseConnexion.toUtf8();

    //envoie du message par la méthode write()
    s->write(qbaReponseConnexion);

}

/**
 * envoie du message
 *
 * @brief Serveur::envoieMessage
 * @param messageAEnvoyer
 */
void Serveur::envoieMessage(QJsonObject &messageAEnvoyer)
{
    //determine quel client parle
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());

    //verification properties
    if (socket->property("login").toString().isEmpty())
    {
        return;
    }


    //transformation en qstring des trames to et content

    QVariantList messageTo = messageAEnvoyer.value("to").toArray().toVariantList();

    QString messageContent = messageAEnvoyer.value("content").toString();


    QJsonObject envoieServeur;
    envoieServeur.insert("action","receive");

    envoieServeur.insert("from",socket->property("login").toString());
    envoieServeur.insert("content",messageContent);

    QDateTime dateTime = QDateTime::currentDateTime();
    envoieServeur.insert("datetime",dateTime.toString("dd/MM/yyyy HH:mm :<- "));

    foreach (QVariant To, messageTo)
    {
        foreach (QTcpSocket *s, this->mListeSocketClient)
        {
            if ( s->property("login").toString() == To.toString()) // socket trouvé
            {
                qDebug() << "client trouvé";

                this->emissionJson(s,envoieServeur); //envoie sur le socket
            }
        }
    }
}


/**
 * client parle au serveur
 * verification si login ou send
 *
 * @brief Serveur::clientMeParle
 */
void Serveur::clientMeParle()
{
    //determine quel client parle
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (socket == 0) // si signal client introuvable
    {
        return; // arrêt
    }

    // on recupere le contenant
    QByteArray messageRecu = socket->readAll();
    //transformation en jsondoc du Qbyte array (utf8)
    QJsonDocument messageJson = QJsonDocument::fromJson(messageRecu);
    //transformation en json object du jsondoc
    QJsonObject messageJsonObject = messageJson.object();

    //transformation en qstring de la trame action
    QString messageAction = messageJsonObject.value("action").toString();

    // verification de l'action
    // action de connexion
    if (messageAction == "login")
    {
        authentification(messageJsonObject);
    }


    //message envoie   
    else if (messageAction == "send")
    {
        this->envoieMessage(messageJsonObject);
        qDebug() << messageJsonObject;

    }

}



