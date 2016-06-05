#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H
//#include <QThread>
#include <QTcpSocket>
#include <QThread>

class socketClients : public QObject
{
    Q_OBJECT
public:
    socketClients(int handle,QObject *parent = 0);
    ~socketClients();
    int getDescription();
    void setUserState(int state);
    qint8 getUserState();
    QString getAccount();
    QTcpSocket* getSocket();
    void setAccount(QString _account);


signals:
    void disconnected(int descrip);
    void readyRead(QByteArray data);

private slots:
    void slotReadData();
    void displayError(QAbstractSocket::SocketError);
    void slotDisconnected();

private:
    void init();

    QTcpSocket *socket;
    int id;
    qint32 blocksize;
    QString account;
    QThread *thread;
    int userState;
};

#endif // SOCKETTHREAD_H
