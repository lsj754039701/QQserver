#include "server_h/tcpserver.h"
#include "server_h/dataformat.h"
#include "model_h/datatype.h"
#include <QThread>
#include <QDataStream>
#include <QImage>
#include <QBuffer>
#include <QLabel>
#include <QImageReader>
#include <QDateTime>

//  Tool fucntions
QByteArray getHeadImage(QString name)
{
    QImage img(name);
    QBuffer buffer;
    img.save(&buffer,"png");
    return buffer.data();
}

MyServer::MyServer(QObject *parent):QTcpServer(parent)
{

}

// data类型  : 文字 ， 图片 ，文件头 ， 文件内容
/*
 * 结构 :  qint8(四种类型) + 数据
*/
void MyServer::slotReadData(QByteArray data)
{
    QDataStream out(&data,QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_0);
    qint8 type;
    out>>type;
    data.remove(0,sizeof(qint8));                qDebug()<<"slotReadData = "<<type;
    if (type == queryType::login) {
        userLogin(data);
    }
    else if (type == queryType::registe) {
        userRegister(data);
    }
    else if (type == queryType::recommend) {
        userRecommend(data);
    }
    else if (type == queryType::groupsize) {
        DataFormat_login info = DataFormat::getGroupSize(data);
        QVector<groupInfo> groups = model.friendGroup(info.account);
        sendGroupSize(groups, clients[ hx[info.account] ]);
    }
    else if (type == queryType::friendGroup) {
        DataFormat_login info = DataFormat::getGroupSize(data);
        QVector<groupInfo> groups = model.friendGroup(info.account);
        sendFriendGroup(groups,clients[ hx[info.account] ]);
    }
    else if (type == queryType::addFriend) {
        userAddFriend(data);
    }
    else if (type == queryType::findPerson) {
        userFindPerson(data);
    }
    else if (type == queryType::chat) {
        userChat(data);
    }
    else if (type == queryType::offlineMes) {
        sendOfflineMes(data);
    }
    else if (type == queryType::history) {
        userHistoryChat(data);
    }
}

void MyServer::slotDisconnected(int descrip)
{
     for (int i=0;i<clients.count();i++)  {
         if (clients.at(i)->getDescription() == descrip){
             hx.remove( clients.at(i)->getAccount());
             delete clients.at(i);
             clients.removeAt(i);
             break;
        }
     }
}

void MyServer::incomingConnection(qintptr descriptor)
{
    socketClients *client = new socketClients(descriptor);                      // qDebug()<<"descriptor= "<<descriptor;
    connect(client,SIGNAL(disconnected(int)),this,SLOT(slotDisconnected(int)));
    connect(client,SIGNAL(readyRead(QByteArray)),this,SLOT(slotReadData(QByteArray)));
    clients.append(client);
    // 回送服务器端socketDescriptor值
    QByteArray *data = model.sendDescriptor(descriptor);
    client->getSocket()->write(*data);
    delete data;
}

// data :descrip +  fromer + account + pw
void MyServer::userLogin(QByteArray& data)
{                                                                             // qDebug()<<"mainThread id:"<< QThread::currentThreadId();
    DataFormat_login loginInfo = DataFormat::getFrom(data);
    userInfo *info = model.checkUser(loginInfo.account,loginInfo.pw) ;
    for (int i=0;i<clients.count();i++) {
        if (clients.at(i)->getDescription() == loginInfo.descrip)
            {
                if (info==NULL) {                     qDebug()<<loginInfo.account<<" :登陆失败";
                    QByteArray *errorData = sql::getErrorData("账号或密码不正确");
                    clients[i]->getSocket()->write(*errorData);
                    delete errorData;
                    //clients.removeAt(i);
                }
                else{                                        qDebug()<<loginInfo.account<<":登陆成功";
                    // 将QQ号写入clients[i]中,以作以后转发聊天信息标识
                    makeUserInfo(info,userStateType::online);
                    clients[i]->setAccount(loginInfo.account);
                    clients[i]->getSocket()->write(bufBlock);
                    bufBlock.resize(0);
                    clients[i]->setUserState(userStateType::online);
                    hx[ loginInfo.account ] = i;  // 建立QQ号与clients链表节点之间的哈希映射

                    //QVector<groupInfo> groups = model.friendGroup(loginInfo.account);
                    //sendGroupSize(groups,clients[i]);
                    //sendFriendGroup(groups,clients[i]);
                }
                break;
            }
    }
    delete info;
}

void MyServer::userRegister(QByteArray &data)
{
    DataFormat_register registerInfo = DataFormat::getRegisterInfo(data);
    // 	返回新注册用户在QQUser表中的id，加上N后就是QQ号
    qint64 id = model.addUser(registerInfo.nick,registerInfo.pw);
    for (int i=0;i<clients.count();i++) {
        if (clients.at(i)->getDescription() == registerInfo.descrip)
            {
                if (id==-1) {                     qDebug()<<id<<":  注册失败 ";
                    QByteArray *errorData = sql::getErrorData("注册失败");
                    clients[i]->getSocket()->write(*errorData);
                    delete errorData;
                    //clients.removeAt(i);
                }
                else{                                        qDebug()<<id<<":  注册成功";
                    // 将QQ号写入clients[i]中,以作以后转发聊天信息标识
                    makeUserInfo(id);
                    clients[i]->setAccount(QString::number(id+N));
                    clients[i]->getSocket()->write(bufBlock);
                    bufBlock.resize(0);

                    clients[i]->setUserState(userStateType::online);
                    hx[ clients[i]->getAccount() ] = i;  // 建立QQ号与clients链表节点之间的哈希映射
                }
                break;
            }
    }
}

void MyServer::userRecommend(QByteArray &data)
{
    int limit = 3;
    DataFormat_login loginInfo = DataFormat::getRecommendInfo(data);
    userInfo *info = model.userRecommend(loginInfo.account,limit);

    for (int i=0;i<clients.count();i++) {
        if (clients.at(i)->getDescription() == loginInfo.descrip)
            {
                if (info==NULL) {                     qDebug()<<loginInfo.account<<" :推荐失败";
                    QByteArray *errorData = sql::getErrorData("没有可推荐好友");
                    clients[i]->getSocket()->write(*errorData);
                    delete errorData;
                    //clients.removeAt(i);
                }
                else{                                        qDebug()<<loginInfo.account<<":推荐成功";
                    // 将推荐好友发给客户
                    makeUserRecommd(info,limit,respondType::recommendOk);
                    clients[i]->getSocket()->write(bufBlock);
                    bufBlock.resize(0);
                }
                break;
            }
    }
    delete[] info;
}

void MyServer::userAddFriend(QByteArray &data)
{
    DataFormat_login queryInfo = DataFormat::getAddFriend(data);
    qint32 friendID = queryInfo.descrip;
    QString groupName = queryInfo.pw;
    QTcpSocket *socket = clients.at( hx[ queryInfo.account ] )->getSocket();

    int userID = model.getFriendID(queryInfo.account.toLongLong());
    // 是否已经是好友
    if ( model.isFriend(userID,friendID) ) {
        QByteArray *errorData = model.getErrorData("你们已经是好友",respondType::addFriendError);
        socket->write(*errorData);
        delete errorData;
        return ;
    }
    // 加为好友，并修改数据库
    model.addFriend(userID,groupName,friendID);
    // 将好友信息发给用户
    userInfo *info = model.getFriendsInfo(friendID);
    makeUserInfo(info,getUserState(info->account), respondType::addFriend);
    socket->write(bufBlock);
    delete info;
}

void MyServer::userFindPerson(QByteArray &data)
{
    DataFormat_login queryInfo = DataFormat::getFindPerson(data);
    QTcpSocket *socket = clients.at( hx[ queryInfo.account ] )->getSocket();
    QString mes = queryInfo.pw;  // mes is nick or QQaccount
    bool ok,isExsit=true;      qDebug()<<"userFindPerson:  "<<queryInfo.account<<" "<<mes;
    qint64 account = mes.toInt(&ok);
    if (ok) {
        qint32 friendID = model.getFriendID(account);
        if (friendID<0) {
            isExsit = false;
        }
        else {
            //userInfo *info = model.getFriendsInfo(friendID);
            userInfo *infos=new userInfo[1];
            infos[0]=*(model.getFriendsInfo(friendID));
            infos[0].id  = friendID;
            makeUserRecommd(infos,1,respondType::accountFind);
        }
    }
    else {
        int infosize=0;                       qDebug()<<" mes:=  "<<mes;
        userInfo* infos = model.getNickFind(mes,infosize);
        if (infos==NULL) {
            isExsit = false;
        }
        else{
            makeUserRecommd(infos,infosize,respondType::nickFind);
        }
    }
    if (isExsit)
        socket->write(bufBlock);
    else {
        QByteArray *errorData = model.getErrorData("没有用户符合搜索条件",respondType::findPersonError);
        socket->write(*errorData);
        delete errorData;
    }
}

void MyServer::userChat(QByteArray& data)
{
    QDataStream out(&data,QIODevice::ReadOnly);
    out.setVersion(QDataStream::Qt_5_0);

    qint8 type;
    out>>type;
    switch (type) {
    case queryType::textChat:
        userTextChat(out);
        break;
    }
}

void MyServer::userTextChat(QDataStream& out)
{
    QString toAccount,fromAccount,content;
    QDateTime time;
    out>>fromAccount>>toAccount>>content;
    out>>time;
    QMap<QString,QVariant> map;
    map["fromAccount"]=fromAccount;
    map["toAccount"]=toAccount;
    map["content"]=content;
    map["time"]=time;
    map["type"] = queryType::textChat;

    if ( hx.contains(toAccount) ) {
        QTcpSocket *socket = clients[ hx[toAccount] ]->getSocket();
        makeTextChat(map);
        socket->write(bufBlock);
        model.saveChatMes(map);
    }
    else {
        model.saveSysMes(map, queryType::textChat);
    }
}

void MyServer::userHistoryChat(QByteArray &data)
{
    QMap<QString, QVariant> map=DataFormat::histroyChat(data);
    QVector<chatMes> mesVec = model.getHistoryChat(map);
    makeSomeChats(mesVec,respondType::history,respondType::chat);

    QTcpSocket *socket = NULL;          qDebug()<<" userHistoryChat "<<mesVec.count();
    if (hx.contains(map["meQQ"].toString() )) {
        socket = clients.at( hx[ map["meQQ"].toString() ] )->getSocket();
        socket->write(bufBlock);
    }
}

void MyServer::makeUserInfo(userInfo *info,qint8 state)
{
    bufBlock.resize(0);
    QDataStream in(&bufBlock,QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);
    in<<qint32(0)<<qint8(respondType::loginOk);
    in<<info->nick<<qint8(info->level) <<info->signatrue<<qint8(state);
    QByteArray data = getHeadImage( QString(":/image/") +  info->headImage);
    in<<qint32(data.size());
    bufBlock.append( data );

    in.device()->seek(0);
    in<<qint32(bufBlock.size() - sizeof(qint32));
}

void MyServer::makeUserInfo(qint64 id)
{
    QDataStream in(&bufBlock,QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);
    in<<qint32(0)<<qint8(respondType::registeOk);
    in<<qint64(id+N)<<qint8(1)<<QString("编辑个性签名")<<QString("head1.png");
    in.device()->seek(0);
    in<<qint32(bufBlock.size() - sizeof(qint32));
}

// 用于返回好友列表    封装用户基本信息>>bufBlock
void MyServer::makeUserInfo(userInfo *info, qint8 state, qint8 type)
{
    bufBlock.resize(0);
    QDataStream in(&bufBlock,QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);
    in<<qint32(0)<<type;
    in<<info->account<<info->nick<<qint8(info->level) <<info->signatrue<<qint8(state);
    QPixmap pix(QString(":/image/") +  info->headImage);
    in<<pix;
    in.device()->seek(0);
    in<<qint32(bufBlock.size() - sizeof(qint32));
}

void MyServer::makeUserRecommd(userInfo *info,int limit, int type)
{
    bufBlock.resize(0);
    QDataStream in(&bufBlock,QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);
    in<<qint32(0)<<qint8(type) <<qint8(limit) ;
    for (int i=0;i<limit;i++ ) {
        in<<qint32(info[i].id)<<info[i].nick;   qDebug()<<" for "<<info[i].id<<" "<<info[i].nick<<" "<<info[i].headImage;
        QPixmap pix(QString(":/image/") +  info[i].headImage);
        in<<pix;
    }
    in.device()->seek(0);
    in<<qint32(bufBlock.size() - sizeof(qint32));
}

void MyServer::makeFriendInfo(QDataStream& in, userInfo *info, qint8 state)
{
    bufBlock.resize(0);
    in<<info->account<<info->nick<<qint8(info->level)<<info->signatrue<<state;
    //QByteArray data = getHeadImage( QString(":/image/") +  info->headImage);
    //in<<qint32(data.size());
    //bufBlock.append( data );
    QPixmap pix(QString(":/image/") +  info->headImage);
    in<<pix;
}

void MyServer::makeTextChat(QMap<QString, QVariant> map)
{
    bufBlock.resize(0);
    QDataStream in(&bufBlock,QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);

    in<<qint32(0)<<qint8(respondType::chat)<<qint8(respondType::textChat);
    in<<map["fromAccount"].toString()<<map["content"].toString();
    in<<map["time"].toDateTime();

    in.device()->seek(0);
    in<<qint32(bufBlock.size() - sizeof(qint32));
}

void MyServer::makeSomeChats(QVector<chatMes> chatVec,qint8 onetype, qint8 type)
{
    bufBlock.resize(0);
    QDataStream in(&bufBlock,QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);

    in<<qint32(0)<<onetype<<type;
    if (type  == respondType::chat) {
        in<<qint8(chatVec.count());
        for (int i=0;i<chatVec.count();i++) {
            in<<chatVec[i].fromAccount <<chatVec[i].type;
            if (chatVec[i].type == queryType::textChat ) {
                in<<chatVec[i].content;
            }
            in<<chatVec[i].time;
        }
    }

    in.device()->seek(0);
    in<<qint32(bufBlock.size() - sizeof(qint32));
}

void MyServer::sendOfflineMes(QByteArray &data)
{
    QString account = DataFormat::offlineMes(data);
    QTcpSocket *socket = NULL;
    if (hx.contains(account)) {
        socket = clients.at( hx[account] )->getSocket();
    }

    int userID = model.getFriendID(account.toLongLong());
    QVector<chatMes> chatVec =model.getOfflineMes(userID);
    if (chatVec.count()>0) {
        makeSomeChats(chatVec, respondType::offlineMes,respondType::chat);
        if (socket) socket->write(bufBlock);
        model.delOfflineMes(userID);
    }
}

void MyServer::sendFriendGroup(const QVector<groupInfo>& groups,socketClients* client)
{
    int size = groups.count();
   QTcpSocket* socket = client->getSocket();

   QByteArray data;
   QDataStream in(&data,QIODevice::WriteOnly);
   in.setVersion(QDataStream::Qt_5_0);

   in<<qint32(0)<<qint8(respondType::friendGroup);
    QVector<int> friendsID;
    for (int i=0;i<size;i++) {
        in<<groups[i].name;
        // 将每一个分组下的所有好友信息封装在一起，再发送出去
        if (groups[i].total<=0) continue;
        model.getFriendsID(friendsID, groups[i].id);
        for (int j=0;j<friendsID.count();j++) {
            userInfo* info = model.getFriendsInfo(friendsID[j]);
            if ( hx.contains(info->account) ) {
                makeFriendInfo(in,info,clients.at( hx[ info->account ] )->getUserState() );
            }
            else {
                makeFriendInfo(in,info,userStateType::offline );
            }
            delete info;
        }
    }
    in.device()->seek(0);
    in<<qint32(data.size()-sizeof(qint32));
    socket->write(data);
}

void MyServer::sendGroupSize(const QVector<groupInfo> &groups, socketClients *client)
{
    int size = groups.count();
    QDataStream in(&bufBlock,QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);

    // 	好友分组个数上限 : qint8
    in<<qint32(0)<<qint8(respondType::friendGroupSize)<<qint8(size);
    for (int i=0;i<size;i++) {     //  qDebug()<<size<<" ,  "<<groups[i].name<<" , "<<groups[i].total;
        in<<qint32(groups[i].total)<<groups[i].name;
    }
    bufferHead(in);
    client->getSocket()->write(bufBlock);
    bufBlock.resize(0);
}

void MyServer::bufferHead(QDataStream &stream)
{
    stream.device()->seek(0);
    stream<<qint32(bufBlock.size() - sizeof(qint32));
}

qint8 MyServer::getUserState(QString account)
{
    if ( hx.contains(account) ) {
        return clients.at( hx[ account ] )->getUserState();
    }
    else {
        return  userStateType::offline;
    }
}







