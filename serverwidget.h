#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H
#include "server_h/tcpserver.h"
#include <QWidget>

namespace Ui {
class Widget;
}

class serverWidget : public QWidget
{
    Q_OBJECT

public:
    explicit serverWidget(QWidget *parent = 0);
    ~serverWidget();

private:
    Ui::Widget *ui;
    MyServer *server;
};

#endif // SERVERWIDGET_H
