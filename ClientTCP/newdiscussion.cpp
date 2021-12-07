#include "newdiscussion.h"
#include "ui_newdiscussion.h"

NewDiscussion::NewDiscussion(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NewDiscussion)
{
    ui->setupUi(this);
}

NewDiscussion::~NewDiscussion()
{
    delete ui;
}
