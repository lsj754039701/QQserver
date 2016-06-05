#include "server_h/dataformat.h"
#include <QDataStream>
#include <QDebug>

DataFormat_login DataFormat::getFrom(QByteArray& data)
{
     DataFormat_login info;
     QDataStream in(&data,QIODevice::ReadOnly);
     in.setVersion(QDataStream::Qt_5_0);
     in>>info.descrip>>info.account>>info.pw;
     return info;
}

DataFormat_register DataFormat::getRegisterInfo(QByteArray &data)
{
    DataFormat_register info;
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_0);
    in>>info.descrip>>info.nick>>info.pw;
    return info;
}

DataFormat_login DataFormat::getRecommendInfo(QByteArray &data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_0);

    DataFormat_login info;
    in>>info.descrip>>info.account;

    return info;
}

DataFormat_login DataFormat::getGroupSize(QByteArray &data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_0);
    DataFormat_login info;
    in>>info.account;
    return info;
}

DataFormat_login DataFormat::getAddFriend(QByteArray &data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_0);
    DataFormat_login info;
    in>>info.account>>info.descrip>>info.pw;  // account  friendID  groupName
    return info;
}

DataFormat_login DataFormat::getFindPerson(QByteArray &data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_0);
    DataFormat_login info;
    in>>info.account>>info.pw;  // account  str
    return info;
}

QString DataFormat::offlineMes(QByteArray &data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_0);
    QString account;
    in>>account;

    return account;
}

QMap<QString, QVariant> DataFormat::histroyChat(QByteArray &data)
{
    QDataStream in(&data,QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_0);

    QString friendQQ,  meQQ;
    qint8 offset;
    in>>meQQ>>friendQQ>>offset;
    QMap<QString, QVariant> map;
    map[ "friendQQ" ] = friendQQ;
    map[ "meQQ" ] = meQQ;
    map[ "offset" ] = offset;
    return map;
}

  //qDebug()<<"loginInfo: "<<info.descrip<<" "<<info.account<<" "<<info.pw;
