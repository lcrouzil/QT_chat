#ifndef NEWDISCUSSION_H
#define NEWDISCUSSION_H

#include <QMainWindow>

namespace Ui {
class NewDiscussion;
}

class NewDiscussion : public QMainWindow
{
    Q_OBJECT

public:
    explicit NewDiscussion(QWidget *parent = nullptr);
    ~NewDiscussion();

private:
    Ui::NewDiscussion *ui;
};

#endif // NEWDISCUSSION_H
