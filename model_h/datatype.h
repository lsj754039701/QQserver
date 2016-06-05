#ifndef DATATYPE
#define DATATYPE
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>

namespace queryType {
    enum ClientType {
        text,
        image,
        fileHead,
        fileBody,
        login,
        registe,
        recommend,
        addFriend,
        groupsize,
        friendGroup,
        findPerson,
        chat,
            textChat,
            imgChat,
            fileChat,
         offlineMes,
         history
    };

}

namespace respondType {
    enum ServerType {
        error,
        descriptor,
        loginOk,  // 登陆成功
        registeOk, // 注册成功
        recommendOk,
        friendGroupSize,
        friendGroup,
        addFriend,
        addFriendError,
        accountFind,
        nickFind,
        findPersonError,
        chat,
            textChat,
            imgChat,
            fileChat,
        offlineMes,
        history
    };
}

namespace userStateType {
    enum userState {
        offline,
        connected,
        online,
        login,
        registe
    };
}

namespace mesType{
    enum message{
        chat,
        addFriend
    };
}

struct userInfo {
    QString nick;
    QString signatrue;
    QString headImage;
    QString account;
    int level,id;
    userInfo(){  account=""; }
    userInfo(QSqlQuery& query)
    {
        account="";
        id = query.value(0).toInt();
        nick = query.value(1).toString();
        level = query.value(2).toInt();
        signatrue = query.value(3).toString();
        headImage = query.value(4).toString() ;
    }
};

struct groupInfo {
    QString name;
    qint32 id,total;
    groupInfo() {}
    groupInfo(QSqlQuery& query){
        id = query.value(0).toInt();
        name = query.value(1).toString();
        total = query.value(2).toInt();
    }
};

struct chatMes
{
    QString  fromAccount,content,toAccount;
    QDateTime time;
    qint8 type;
};

#endif // DATATYPE

