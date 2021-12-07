#include "client.h"

#include "ui_client.h"
#include<QFile>
#include<QJsonObject>
#include <QJsonArray>
#include<QJsonDocument>
#include <QDebug>
#include<QDataStream>
#include<QByteArray>
#include<QMap>


Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Client)
{
    logDialog = new LogDialog(this);
    DiscuDial = new DialNewDiscu(this);

    logDialog->setModal(true); //fenetre de log en modal
    connect(logDialog, SIGNAL(tryLogin(QString, QString)), this, SLOT(storeLog(QString, QString)));


    ui->setupUi(this);
    this->sock = new QTcpSocket(this);


    //partie connexion
    connect(this->sock, SIGNAL(connected()), this, SLOT(connexion()));
    connect(this->sock, SIGNAL(readyRead()), this, SLOT(serveurMeParle()));
    connect(this->sock, SIGNAL(disconnected()), this->sock, SLOT(deleteLater()));

    //partie messagerie
        //fenetre chat
    connect(this->ui->le_message, SIGNAL(returnPressed()), this, SLOT(on_le_message_returnPressed()));
    connect(this->ui->pb_envoyer, SIGNAL(clicked()), this, SLOT(on_le_message_returnPressed()));
    connect(this->ui->pb_newConver, SIGNAL(clicked()), this, SLOT(newConv())); //afficher le pop up new conversation
    connect(this->ui->pb_pdf, SIGNAL(clicked()), this, SLOT(on_pb_pdf_clicked())); //lance la conversion pdf

    //Pop up nouvelle conv
    connect(DiscuDial, SIGNAL(newDiscu(QString)), this, SLOT(newTabDiscu(QString)));



    logDialog->show();

    this->sock->connectToHost("localhost",8585);
}

Client::~Client()
{
    delete ui;
}

void Client::connexion()
{
    qDebug() << "mdp recu" << monPassword;


    qDebug() << "methode connexion";

}

/**
 *verification de doublon de tab et ajout en cas de besoin
 *créer un tab en cas de reception sans tab existante
 *
 * @brief Client::newTabDiscu
 * @param destinataire
 */
void Client:: newTabDiscu(QString destinataire)
{

    QTextEdit *monTexte = new QTextEdit();

    monTexte->setProperty("destinataire",destinataire);


    bool doublon = false;
     for (int i=0;i<ui->tabWidget->count() ;i++ )
     {
         if (ui->tabWidget->widget(i)->property("destinataire") == destinataire)
         {
             doublon = true;
         }
     }
     if (!doublon)
     {
         ui->tabWidget->insertTab(1, monTexte, destinataire);
     }
     DiscuDial->close();
}

/**
 * Ajouter la tab text propre a chaque conversation
 *
 * @brief Client::ajoutTexte
 * @param destinataire
 * @param message
 */
void Client::ajoutTexte(QString destinataire, QString message)
{
    newTabDiscu(destinataire);

    for (int i=0;i<ui->tabWidget->count() ;i++ )
    {
        if (ui->tabWidget->widget(i)->property("destinataire") == destinataire)
        {
             QTextEdit *tabText = qobject_cast<QTextEdit *>(ui->tabWidget->widget(i));


             tabText->append(message);
        }

    }
}

/**
 * fenetre de nouvelle conversation
 *
 * @brief Client::newConv
 */
void Client::newConv()
{
    DiscuDial->show();
    DiscuDial->setModal(true);

}

/**
 * enregistrement des log de la fenetre
 * de connexion avec appel de la connexion
 *
 * @brief Client::storeLog
 * @param log
 * @param password
 */
void Client::storeLog(QString log, QString password)
{
    this->monPassword = password;
    this->monPseudo = log;
    jsonConnexion();

}


/**
 * lecture de message venant du client
 *
 * @brief Client::serveurMeParle
 */
void Client::serveurMeParle()
{
    QString messageRecu = sock->readAll();

    qDebug().noquote() << "serveur me parle" << messageRecu;

    //transformation en jsondoc du Qbyte array (utf8)
    QJsonDocument messageJson = QJsonDocument::fromJson(messageRecu.toUtf8());
    //transformation en json object du jsondoc
    QJsonObject messageJsonObject = messageJson.object();
    jsonMessageReception(messageJsonObject);

}

//useless
void Client::envoieServeur()
{
    qDebug() << "serveur conected";

}


/**
 * json de connexion
 *
 * @brief Client::jsonConnexion
 */
void Client::jsonConnexion()
{

    QJsonObject connexion;
    connexion.insert("action","login");
    connexion.insert("login",monPseudo);
    QString monPasswordHashe = QString(QCryptographicHash::hash(monPassword.toUtf8(), QCryptographicHash::Sha3_512).toHex());
    connexion.insert("password",monPasswordHashe);
    this->emssionJson(connexion);
    qDebug()<< "emiisionJson" << connexion;

}


/**
 * json d'envoie de message
 *
 * @brief Client::jsonMessageEnvoie
 * @param message
 * @param personne
 */
void Client::jsonMessageEnvoie(QString &message, QString &personnes)
{
    //QTcpSocket * sock;
    QJsonObject envoie;
    envoie.insert("action","send");


    QJsonArray To;
    //foreach split
    foreach(QString personne, personnes.remove(' ').split(','))
    {
        To.append(personne);
    }

    envoie.insert("to",To);
    envoie.insert("content",message);
    this->emssionJson(envoie);

    qDebug() << "qjsonObject de jsonmessageEnvoie" <<  envoie;

}

/**
 * emissionJson -> serveur
 *
 * @brief Client::emssionJson
 * @param envoie
 */
void Client::emssionJson(QJsonObject &envoie)
{
    //transformation de envoie en docJSON
    QJsonDocument docEnvoie(envoie);

    //transformation de envoie en Qstring
    QString strEnvoie(docEnvoie.toJson(QJsonDocument::Compact));

    //transformation de envoie en QByteArray
    QByteArray qbaEnvoie=strEnvoie.toUtf8();

    //envoie du message par la méthode write()
    sock->write(qbaEnvoie);
    qDebug() << "qbytearray de emissionJson" <<  qbaEnvoie;
}

/**
 * récupere les trame pour verifier si
 * 1- connexion
 * 2-reception de message
 *
 *
 * @brief Client::jsonMessageReception
 * @param messageJsonObject
 */
void Client::jsonMessageReception(QJsonObject messageJsonObject)
{


    //transformation en qstring
    //trame action
    QString messageAction = messageJsonObject.value("action").toString();

    qDebug() << "Action => " << messageAction;
    //connexion
    if (messageAction == "answerlogin")
    {
        //trame state
        QString messageState = messageJsonObject.value("state").toString();
        if (messageState == "ok")
        {
            logDialog->close();
        }

    }

    //reecption message

    else if (messageAction == "receive")
    {
        //trame From
        QString messageFrom = messageJsonObject.value("from").toString();


        //trame content
        QString messageContent = messageJsonObject.value("content").toString();

        //trame  datetime
        QString messageDateTime = messageJsonObject.value("datetime").toString();

        qDebug() << "from  => " << messageFrom;
        qDebug() << "message => " << messageContent;
        qDebug() << "Date et heure => " << messageDateTime;

        QString message = QString("%1 %2").arg(messageDateTime,messageContent);

        ajoutTexte(messageFrom, message);

    }

}


/**
 * fonction qui recupere le message dans le
 * line edit pour le rajouter a la zone de texte
 *
 *
 * @brief Client::on_le_message_returnPressed
 */
void Client::on_le_message_returnPressed()
{

    QString message = ui->le_message->text();
    if (message.isEmpty())
        return;
    else
    {

        QString destinataire = ui->tabWidget->currentWidget()->property("destinataire").toString();

        QTextEdit *tabText = qobject_cast<QTextEdit *>(ui->tabWidget->currentWidget());


        QDateTime dataTime = QDateTime::currentDateTime();
        QString messageEmis = QString("%1 %2").arg(dataTime.toString("dd/MM/yyyy HH:mm : ->"),message);

        tabText->append(messageEmis);

        jsonMessageEnvoie(message,destinataire);

        qDebug() <<"methode on le message return " << message;
        ui->le_message->clear();

    }

}


/**
 * bouton pdf clicke
 * recupere le textEdit de la conv ainsi que le destinataire
 * insere conv et detinataire dans un QText doc
 * crée puis enregistre un pdf
 *
 * @brief Client::on_pb_pdf_clicked
 */
void Client::on_pb_pdf_clicked()
{
    QTextEdit *tabText = qobject_cast<QTextEdit *>(ui->tabWidget->currentWidget());
    QString destinataire = ui->tabWidget->currentWidget()->property("destinataire").toString();




     // text de la conversation convertie en html puis stocké en qstring
     QString tabTextString = tabText->toHtml();

     //Créer le fichier pdf
     QTextDocument docPDF ;
     QPrinter docPrinter ;

     //creation du doc
     docPDF.setHtml("<h1> chat enregistré avec " + destinataire + " </h1>"+tabTextString);

     //L'enregistrer
     docPrinter.setOutputFormat(QPrinter::PdfFormat);
     docPrinter.setPageSize(QPrinter::A4);

     //Zone d'enregistrement du document
     QString chemin = QFileDialog::getSaveFileName(this);
     docPrinter.setOutputFileName(chemin);


     //création des marges
     docPrinter.setPageMargins(QMarginsF(15, 30, 15, 30));

     //impression du doc en pdf
     docPDF.print(&docPrinter);


}

