#ifndef TCPSERVER
#define TCPSERVER
#include "server_h/socketthread.h"
#include "model_h/SQL.h"
#include <QTcpServer>
#include <QDebug>
const int N=1000000;

class MyServer:public QTcpServer
{
    Q_OBJECT
public:
    explicit MyServer(QObject *parent=0);

protected slots:
    void slotReadData(QByteArray data);
    void slotDisconnected(int descrip);
//    void displayError(QAbstractSocket::SocketError);
    //void slotNewConnection();

protected:
    void incomingConnection(qintptr descriptor);

    void userLogin(QByteArray& data);
    void userRegister(QByteArray& data);
    void userRecommend(QByteArray& data);
    void userAddFriend(QByteArray& data);
    void userFindPerson(QByteArray& data);
    void userChat(QByteArray& data);
    void userTextChat(QDataStream& out);

    void userHistoryChat(QByteArray& data);

    void makeUserInfo(userInfo *info,qint8 state);  // 封装用户基本信息>>bufBlock, send login
    void makeUserInfo(qint64 id); // 封装用户基本信息, send register
    void makeUserInfo(userInfo *info, qint8 state,qint8 type);
    void makeUserRecommd(userInfo *info, int limit, int type);  // 封装好友推荐信息
    void makeFriendInfo(QDataStream& in, userInfo *info, qint8 state);
    void makeTextChat(QMap<QString,QVariant> map);
    // onetype : offline mes or history mes , type: chat or appendFriend
    void makeSomeChats(QVector<chatMes> chatVec,qint8 onetype,qint8 type);

    void sendOfflineMes(QByteArray& data);
    void sendFriendGroup(const QVector<groupInfo>& groups,socketClients* client);
    void sendGroupSize(const QVector<groupInfo>& groups,socketClients* client);
    void bufferHead(QDataStream &stream);
    qint8 getUserState(QString account);

private:
    QList<socketClients*> clients;
    sql model;
    QMap<QString,int> hx;  // account  -> clients.at(x)
    QByteArray bufBlock;
};

#endif // TCPSERVER

