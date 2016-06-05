#include "model_h/SQL.h"
#include "model_h/sql_statement.h"
#include "model_h/datatype.h"
#include <QSqlError>
#include <QDebug>
#include <QDataStream>

sql::sql():N(1000000)
{
    db = QSqlDatabase::addDatabase("QMYSQL","user");
    db.setDatabaseName("storm");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("123");
    if (!db.open()) {
        qDebug()<<"database open fail";
    }
}

QByteArray *sql::getErrorData(QString str)
{
    QByteArray *data = new QByteArray;
    QDataStream in(data,QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);
    in<<qint32(0)<<qint8(respondType::error)<<str;
    in.device()->seek(0);
    in<<qint32(data->size() - sizeof(qint32));
    return data;
}

QByteArray *sql::getErrorData(QString str, int type)
{
    QByteArray *data = new QByteArray;
    QDataStream in(data,QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);
    in<<qint32(0)<<qint8(type)<<str;
    in.device()->seek(0);
    in<<qint32(data->size() - sizeof(qint32));
    return data;
}

userInfo* sql::checkUser(QString account,QString pw)
{
    QSqlQuery query(db);
    qint64 act = account.toLongLong();
    if ( !query.exec(sql_statement::checkUser_str(act,pw)) ) {
        qDebug()<<query.lastError().text();
    }
    userInfo *info=NULL;
    if (query.size()==1) {
        query.next();
        info =new  userInfo(query);
    }
    return info;
}

QByteArray *sql::sendDescriptor(int descrip)
{
    QByteArray *data=new QByteArray;
    QDataStream in(data,QIODevice::WriteOnly);
    in.setVersion(QDataStream::Qt_5_0);
    in<<qint32(0)<<qint8(respondType::descriptor)<<qint32(descrip);
    in.device()->seek(0);
    in<<qint32(data->size() - sizeof(qint32));
    return data;
}

bool sql::isRegistered(QString account)
{
    QSqlQuery query(db);
    qint64 act = account.toLongLong();
    if ( !query.exec(sql_statement::isRegistered_str(act) ) ) {
        qDebug()<<query.lastError().text();
    }
    return query.size()==1;
}

qint64 sql::addUser(QString nick, QString pw)
{
    QSqlQuery query(db);
    if ( !query.exec(sql_statement::addUser_str(nick,pw) ) ) {
        qDebug()<<"addUser fail: "<<query.lastError().text();
        return -1;
    }
    query.exec(sql_statement::lastUser_str());
    query.next();
    qint64 id = query.value(0).toLongLong();
    if (!query.exec(sql_statement::updateAccount(id))) {
        qDebug()<<"insertAccount fail: "<<query.lastError().text();
        return -1;
    }

    if (!query.exec(sql_statement::addfriendGruop_str(id,"我的好友"))) {
        qDebug()<<"addfriendGruop fail: "<<query.lastError().text();
        return -1;
    }
    query.exec(sql_statement::addfriendGruop_str(id,"陌生人"));
    query.exec(sql_statement::addfriendGruop_str(id,"黑名单"));
    return id;
}

userInfo *sql::userRecommend(QString account, int limit)
{
    QSqlQuery query(db);
    qint64 act = account.toLongLong();
    if ( !query.exec(sql_statement::userRecommend_str(act,limit) ) ) {
        qDebug()<<"userRecommend fail: "<<query.lastError().text();
        return NULL;
    }
    userInfo *info = new userInfo[limit];
    int i=0;
    while(query.next()) {
        info[i].id = query.value(0).toInt();
        info[i].nick = query.value(1).toString();
        info[i].headImage =  query.value(2).toString();
              qDebug()<<"Model: "<<info[i].id<<" "<<info[i].nick<<" "<<info[i].headImage;
        i++;
    }
    return info;
}

void sql::getFriendsID(QVector<int> &friendsID, int groupID)
{
    friendsID.clear();
    QSqlQuery query(db);
    if ( !query.exec(sql_statement::friendsID_str(groupID) ) ) {
        qDebug()<<"userRecommend fail: "<<query.lastError().text();
        return ;
    }
    while(query.next()) {
        friendsID.push_back(query.value(0).toInt());
    }
}

userInfo *sql::getFriendsInfo(int friendID)
{
    QSqlQuery query(db);
    if ( !query.exec(sql_statement::friendInfo_str(friendID) ) ) {
        qDebug()<<"userRecommend fail: "<<query.lastError().text();
        return NULL;
    }
    query.next();
    userInfo *info = new userInfo;
    info->account = QString::number( query.value(0).toInt() );
    info->nick = query.value(1).toString();
    info->level = query.value(2).toInt();
    info->signatrue = query.value(3).toString();
    info->headImage = query.value(4).toString();
    return info;
}

bool sql::addFriend(int userID, QString groupName, qint32 friendID)
{
    QSqlQuery query(db);
    query.exec(sql_statement::groupID_str(userID,groupName));
    query.next();
    int groupID = query.value(0).toInt();

    if (!query.exec(sql_statement::addFriend_str(friendID,groupID))) {
        qDebug()<<"addFriend fail: "<<query.lastError().text();
        return false;
    }
    if (!query.exec(sql_statement::updateGroup_str(groupID) )) {
        qDebug()<<"addFriend fail: "<<query.lastError().text();
        return false;
    }
    return true;
}

qint32 sql::getFriendID(qint64 account)
{
    QSqlQuery query(db);
    if (!query.exec(sql_statement::userID_str(account))) {
        qDebug()<<"getFriendID fail: "<<query.lastError().text();
        return -1;
    }
    if (query.size()<=0) return -1;
    query.next();
    return query.value(0).toInt();
}

userInfo *sql::getNickFind(QString nick, int &size)
{
    QSqlQuery query(db);
    if (!query.exec(sql_statement::nickFind_str(nick))) {
        qDebug()<<"getNickFind fail: "<<query.lastError().text();
        return NULL;
    }
    size = query.size();
    if (query.size()==0) {
        return NULL;
    }
    userInfo *infos = new userInfo[query.size() ];
    int i=0;
    while(query.next()) {
        infos[i].id = query.value(0).toInt();
        infos[i].nick = query.value(1).toString();
        infos[i].headImage = query.value(2).toString();
        i++;
    }
    return infos;
}

bool sql::isFriend(int userID, qint32 friendID)
{
    QSqlQuery query(db);
    query.exec(sql_statement::isAddedFriend_str(userID,friendID));
    query.next();
    int cnt = query.value(0).toInt();
    if (cnt==0) return false;
    return true;
}

bool sql::saveChatMes(QMap<QString, QVariant> map)
{
    QString sql_str = sql_statement::saveChatMes_str();
    QString str = map["time"].toDateTime().toString("yyyy-MM-dd hh:mm");
    int fromID = getFriendID(map["fromAccount"].toInt()) ;
    int toID = getFriendID(map["toAccount"].toInt());
    sql_str = sql_str.arg(map["type"].toInt()).arg(fromID).arg(toID).arg(map["content"].toString()).arg(str);

    QSqlQuery query(db);
    if ( !query.exec(sql_str ) ) {
        qDebug()<<"saveChatMes fail: "<<query.lastError().text();
        return false;
    }
    return true;
}

bool sql::saveSysMes(QMap<QString, QVariant> map, int type)
{
    QString sql_str = sql_statement::saveSysMes_str();
    QString str = map["time"].toDateTime().toString("yyyy-MM-dd hh:mm");
    int fromID = getFriendID(map["fromAccount"].toInt()) ;
    int toID = getFriendID(map["toAccount"].toInt());
    sql_str = sql_str.arg(type).arg(fromID).arg(toID).arg(map["content"].toString()).arg(str);

    QSqlQuery query(db);
    if ( !query.exec(sql_str ) ) {
        qDebug()<<"saveSysMes fail: "<<query.lastError().text();
        return false;
    }
    return true;
}

bool sql::delOfflineMes(int userID)
{
    QSqlQuery query(db);
    if ( !query.exec(sql_statement::delOfflineMes_str(userID) ) ) {
        qDebug()<<"saveSysMes fail: "<<query.lastError().text();
        return false;
    }
    return true;
}

QVector<chatMes> sql::getOfflineMes(int userID)
{
    QVector<chatMes> chatVec;
    QSqlQuery query(db);
    if (!query.exec(sql_statement::offlineMes_str(userID))) {
        qDebug()<<"getOfflineMes fail: "<<query.lastError().text();
        return chatVec;
    }
    int size = query.size();
    if (query.size()==0) {
        return chatVec;
    }
    while(query.next()) {
        int fromID = query.value(0).toInt();
        QString content = query.value(1).toString();
        QString time = query.value(2).toString();
        qint8 type = query.value(3).toInt();
        chatMes mes;
        mes.fromAccount = QString::number(fromID+N);
        mes.content = content;
        mes.time=QDateTime::fromString(time,"yyyy-MM-dd hh:mm");
        mes.type = type;
        chatVec.push_back(mes);
    }
    return chatVec;
}

QVector<chatMes> sql::getHistoryChat(QMap<QString, QVariant> map)
{
    QSqlQuery query(db);
    QVector<chatMes> mesVec;

    int userID = getFriendID(map["friendQQ"].toLongLong()) ;
    int friendID = getFriendID(map["meQQ"].toLongLong());
    int offset =  map[ "offset" ].toInt();
    if ( !query.exec(sql_statement::historyChat_str(userID,friendID,offset) ) ) {
        qDebug()<<"getHistoryChat fail: "<<query.lastError().text();
        return mesVec;
    }           qDebug()<<sql_statement::historyChat_str(userID,friendID,offset);
    int size = query.size();
    if (size==0) {
        return mesVec;
    }
    int i=0;
    while(query.next()) {
        chatMes mes;
        int fromID = query.value(0).toInt();
        mes.fromAccount = QString::number(fromID+N);
        mes.content = query.value(1).toString();
        mes.time=QDateTime::fromString(query.value(2).toString(),"yyyy-MM-dd hh:mm");
        mes.type = query.value(3).toInt();
        mesVec.push_back(mes);
                    qDebug()<<" getHistoryChat"<<mesVec.count() <<mesVec.back().fromAccount<< " "<<mesVec.back().content;
    }
    return mesVec;
}

QVector<groupInfo> sql::friendGroup(const QString& account)
{
    QSqlQuery query(db);
    QVector<groupInfo> groups;
    qint64 act = account.toLongLong();
    if ( !query.exec(sql_statement::friendGroup_str(act) ) ) {
        qDebug()<<"userRecommend fail: "<<query.lastError().text();
        return groups;
    }
    while(query.next()) {
        groups.push_back( groupInfo(query) );
    }
    return groups;
}


