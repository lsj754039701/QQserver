#include "server_h/socketthread.h"
#include <QDebug>
#include <QDataStream>
#include <QMetaType>

socketClients::socketClients(int handle,QObject *parent):QObject(parent)
{
    id=handle;
    blocksize=0;
    socket = NULL;
    qRegisterMetaType<QByteArray>("QByteArray");
    qRegisterMetaType<QAbstractSocket::SocketError>("SocketError");
    init();
}

socketClients::~socketClients()
{
    delete socket;
    thread->quit();
    thread->wait();
    delete thread;
}

void socketClients::init()
{
    socket = new QTcpSocket;
    if (!socket->setSocketDescriptor(id)) {
          // emit error(socket->error());
        qDebug()<<"thread: setSocketDescriptor error";
        return ;
     }
    connect(socket,&QTcpSocket::readyRead,this,&socketClients::slotReadData);
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(displayError(QAbstractSocket::SocketError)));
    connect(socket,&QTcpSocket::disconnected,this,&socketClients::slotDisconnected);
    thread = new QThread();
    moveToThread(thread);
    thread->start();
}

int socketClients::getDescription()
{
    return id;
}

void socketClients::setUserState(int state)
{
    userState = state;
}

qint8 socketClients::getUserState()
{
    return userState;
}

void socketClients::setAccount(QString _account)
{
    account = _account;
}

QString socketClients::getAccount()
{
    return account;
}

QTcpSocket *socketClients::getSocket()
{
    return socket;
}

void socketClients::slotReadData()
{
    QDataStream in(socket);                                                         qDebug()<<"sonThread id:"<< QThread::currentThreadId();
    in.setVersion(QDataStream::Qt_5_0);
    if (blocksize == 0) {
        if (socket->bytesAvailable() < sizeof(qint32)) return ;
        in>>blocksize;
    }
    if (socket->bytesAvailable() < blocksize) return ;
    blocksize = 0;
    QByteArray data;
    data.append(socket->readAll());

    emit readyRead(data);
}

void socketClients::displayError(QAbstractSocket::SocketError)
{
    qDebug()<<socket->errorString();
}

void socketClients::slotDisconnected()
{
    qDebug()<<id<<": Disconnected";
    emit disconnected( id );
    //socket->deleteLater();
    //thread->deleteLater();
}

