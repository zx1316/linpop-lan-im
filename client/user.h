#ifndef USER_H
#define USER_H
#include <QString>

class User
{
public:
    User(QString, bool);
    QString username;
    bool type;
};

#endif // USER_H
