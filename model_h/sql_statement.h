#ifndef SQL_STATEMENT
#define SQL_STATEMENT
#include <QString>

namespace sql_statement {
    const int N=1000000;  //账号的初始值

    // 查询用户基本信息
    QString checkUser_str(qint64 account,QString pw)
    {
        return QString("select userID,nick,level,signature,headImage "
                                  "from QQuser "
                                  "where account = %1 and password = %2").arg(account).arg(pw);
    }

    // no useful
    QString isRegistered_str(qint64 account)
    {
        return QString("select count(*)"
                                  "from QQuser"
                                  "where account = %1").arg(account);
    }

    // 添加用户
    QString addUser_str(QString nick,QString pw)
    {
        //return QString("insert into QQuser(nick,password) values(%1,%2)").arg(nick).arg(pw);
        return QString("insert into QQuser(nick,password) values('%1',%2)").arg(nick).arg(pw);
    }

    // 查询最后一个用户（即刚刚添加的那个用户）的id
    QString lastUser_str()
    {
        return QString("select userID from QQuser order by userID desc limit 1");
    }

    // 为刚才添加的用户设置QQ号
    QString updateAccount(qint64 id)
    {
        return QString("update QQuser set account = %1 "
                       " where userID=%2").arg(id+N).arg(id);
    }

    // 添加好友分组
    QString addfriendGruop_str(qint64 owerID,QString name)
    {
        return QString("insert into friendGroup(owerID,groupName) "
                                  "values(%1,'%2')").arg(owerID).arg(name);
    }

    // 获取推荐好友
    QString userRecommend_str(qint64 id,int num)
    {
        return QString("select userID,nick,headImage from QQuser "
                                  "order by userID limit %1").arg(num);
    }

    //   获得所有好友分组信息
    QString friendGroup_str(qint64 account)
    {
        return QString("select id,groupName,total"
                                  " from QQuser,friendGroup "
                                  " where account=%1 and owerID=userID").arg(account);
    }

    // 获取某一个分组下所有好友的ID(数据库表的id)
    QString friendsID_str(int groupID)
    {
        return QString("select friendID"
                                  " from friend "
                                  " where groupID=%1").arg(groupID);
    }

    // 根据用户ID获取用户信息
    QString friendInfo_str(int friendID)
    {
        return QString("select account,nick,level,signature,headImage"
                                  " from QQuser "
                                  " where userID=%1").arg(friendID);
    }

    // 	根据QQ号获取用户的ID
    QString userID_str(qint64 account)
    {
        return QString("select userID"
                                  " from QQuser "
                                  " where account=%1").arg(account);
    }

    // 根据用户ID和分组名获取分组ID
    QString groupID_str(int friendID, QString groupName)
    {
        return QString("select id"
                                  " from friendGroup "
                                  " where owerID=%1 and  groupName='%2'").arg(friendID).arg(groupName);
    }

    // 在friend表中添加一个好友记录
    QString addFriend_str(int friendId, int groupID)
    {
        return QString("insert into friend(friendID,groupID) values(%1,%2)").arg(friendId).arg(groupID);
    }

    //
    QString updateGroup_str(int groupID)
    {
        return QString("update friendGroup"
                                  " set total=total+1 "
                                  " where id=%1").arg(groupID);
    }

    // 该用户是否已经是好友
    QString isAddedFriend_str(int userID,int friendID)
    {
        return QString("select count(*)"
                                  " from friend,friendGroup"
                                  " where owerID = %1 and"
                                  " friendGroup.id = friend.groupID and"
                                  " friend.friendID = %2").arg(userID).arg(friendID);
    }

    QString nickFind_str(QString nick)
    {
        return QString("select userID,nick,headImage"
                                  " from QQuser"
                                  " where nick ='%1' ").arg(nick);
    }

    QString saveSysMes_str()
    {
        return QString("insert into systemMes(type, fromID, toID, content, create_time) "
                                  " values(%1, %2, %3, '%4', '%5')");
    }

    QString saveChatMes_str()
    {
        return QString("insert into chatMes(type,fromID, toID, content, create_time) "
                                  " values(%1, %2, %3, '%4', '%5')");
    }

    QString offlineMes_str(int userID)
    {
        return QString("select fromID,content,create_time,type"
                                  " from systemMes"
                                  " where toID =%1 ").arg(userID);
    }

    QString delOfflineMes_str(int userID)
    {
        return QString("delete"
                                  " from systemMes"
                                  " where toID =%1 ").arg(userID);
    }

    QString historyChat_str(int userID,int friendID,int offset)
    {
        int limit = 20;
        return QString("select fromID,content,create_time,type"
                                  " from chatMes"
                                  " where (fromID=%1 and toID=%2) or "
                                  " (toID = %3 and fromID = %4)"
                                  " limit %5,%6").arg(userID).arg(friendID).arg(userID).arg(friendID).arg(offset).arg(limit);
    }
}



#endif // SQL_STATEMENT

