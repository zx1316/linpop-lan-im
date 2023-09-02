/**
 * 文件名称：chatrecord.h
 * 类名称：ChatRecord（聊天记录）
 * 描述：用于保存聊天记录的各种信息
 * 成员变量：
 * 1.   _from_username:QString  保存聊天信息发送者用户名
 * 2.   _to_username:QString    保存聊天信息接收者用户名
 * 3.   _from_nickname:QString  保存聊天信息发送者昵称
 * 4.   _datetime:QDateTime     保存聊天信息发送时间
 * 5.   _message:QString        保存聊天信息内容
 * 6.   _fontsize:int           保存字号
 * 7.   _color:QColor           保存字体颜色
 * 8.   _board:bool             保存字体加粗信息
 * 9.   _italics:bool           保存字体斜体信息
 * 10.  _underline:bool         保存字体下划线信息
 * 初版完成时间：2023.08.24
 * 做成时间：----。--。--
 * 作者：刘文景
 */
#ifndef CHATRECORD_H
#define CHATRECORD_H

#include <QDateTime>
#include <QColor>

class ChatRecord
{
public:
    ChatRecord(QString from_username, QString to_username,
               QDateTime datetime, QString message,
               int fontsize, QColor color, bool board,
               bool italics, bool underline);
    QColor color();
    bool board();
    bool italics();
    bool underline();
    int fontsize();
    QString fromUsername();
    QDateTime datetime();
    QString message();

private:
    QString _from_username, _to_username;

    QDateTime _datetime;

    QString _message;

    int _fontsize;

    QColor _color;

    bool _board, _italics, _underline;
};

#endif // CHATRECORD_H
