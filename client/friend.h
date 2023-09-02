/*
 * 文件名称：friend.h
 * 类名称：Friend（好友类）
 * 描述：用于存储好友的昵称、IP信息和在线状态，重载了其<运算符，使其能用于排序
 * 成员变量：
 * 1.   _state:bool             保存好友在线状态
 * 2.   _name:QString           保存好友昵称
 * 3.   _ip:QString             保存好友IP地址
 * 初版完成时间：2023.08.23
 * 做成时间：----。--。--
 * 作者：林方裕
 */
#ifndef FRIEND_H
#define FRIEND_H
#include "QString"

class Friend
{
public:
    Friend(bool,QString,QString,bool=true);
    bool operator<(const Friend& f);
    bool operator==(const Friend& f);
    bool _state;
    bool _real;
    QString _name;
    QString _ip;
    friend class FriendInformation;
};

#endif // FRIEND_H
