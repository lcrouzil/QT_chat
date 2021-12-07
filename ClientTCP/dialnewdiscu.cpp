#include "dialnewdiscu.h"
#include "ui_dialnewdiscu.h"

DialNewDiscu::DialNewDiscu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialNewDiscu)
{
    ui->setupUi(this);
    connect(this->ui->pb_newConver, SIGNAL(clicked()), this, SLOT(on_pb_newConver_clicked()));
}

DialNewDiscu::~DialNewDiscu()
{
    delete ui;
}

/**
 * envoie du destinataire par le line edit de
 * la fenetre discussion
 *
 * @brief DialNewDiscu::on_pb_newConver_clicked
 */
void DialNewDiscu::on_pb_newConver_clicked()
{
    if (ui->le_Destinataire->text().isEmpty())
    {
        return;
    }
    else
    {
        emit newDiscu(ui->le_Destinataire->text());
    }
}

