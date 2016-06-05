#include "serverwidget.h"
#include "ui_widget.h"

serverWidget::serverWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    server = new MyServer(this);
    server->listen(QHostAddress::LocalHost,8000);
}

serverWidget::~serverWidget()
{
    delete ui;
    delete server;
}

