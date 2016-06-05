#ifndef LOGINOBJ
#define LOGINOBJ

#include <QObject>
class LoginObj:public QObject
{
    Q_OBJECT
public:
    explicit LoginObj(QObject *parent = 0);

};

#endif // LOGINOBJ

