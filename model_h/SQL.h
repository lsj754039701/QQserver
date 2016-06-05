#ifndef SQL
#define SQL

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
class userInfo;
class groupInfo;
class chatMes;

class sql
{
public:
    sql();
    static QByteArray* getErrorData(QString str);
    QByteArray* getErrorData(QString str,int type);
    userInfo* checkUser(QString account,QString pw);
    QByteArray* sendDescriptor(int descrip);
    bool isRegistered(QString account);
    qint64 addUser(QString nick,QString pw);
    userInfo* userRecommend(QString account,int limit);
    QVector<groupInfo> friendGroup(const QString& account);
    void getFriendsID(QVector<int>& friendsID,int groupID);

    userInfo* getFriendsInfo(int friendID);
    bool addFriend(int userID,QString groupName,qint32 friendID);
    qint32 getFriendID(qint64 account);
    userInfo* getNickFind(QString nick, int &size);
    bool isFriend(int userID,qint32 friendID);

    bool saveChatMes(QMap<QString,QVariant> map);
    bool saveSysMes(QMap<QString,QVariant> map,int type);
    bool delOfflineMes(int userID);

    QVector<chatMes> getOfflineMes(int userID);
    QVector<chatMes> getHistoryChat(QMap<QString,QVariant> map);

private:
    const int N;
    QSqlDatabase db;
};


#endif // CREATECONNECTION
