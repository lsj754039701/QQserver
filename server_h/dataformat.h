#ifndef DATAFORMAT
#define DATAFORMAT
#include <QString>
#include <QVariant>

struct DataFormat_login
{
     QString account,pw;
     qint32 descrip;
};

struct DataFormat_register
{
    QString nick,pw;
    qint32 descrip;
};

class DataFormat
{
public:
    static DataFormat_login getFrom(QByteArray& data);
    static DataFormat_register getRegisterInfo(QByteArray& data);
    static DataFormat_login getRecommendInfo(QByteArray& data);
    static DataFormat_login getGroupSize(QByteArray& data);
    static DataFormat_login getAddFriend(QByteArray& data);
    static DataFormat_login getFindPerson(QByteArray& data);
    static QString offlineMes(QByteArray& data);
    static QMap<QString,QVariant> histroyChat(QByteArray& data);
};


#endif // DATAFORMAT

