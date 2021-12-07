#include "logdialog.h"
#include "ui_logdialog.h"

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
    connect(this->ui->pb_Co, SIGNAL(clicked()), this, SLOT(bpConnexion()));
}

LogDialog::~LogDialog()
{
    delete ui;
}

/**
 * envoie des identifiants par e BP connexion
 * @brief LogDialog::bpConnexion
 */
void LogDialog::bpConnexion()
{
  emit tryLogin(ui->le_login->text(),ui->le_mdp->text());
}
