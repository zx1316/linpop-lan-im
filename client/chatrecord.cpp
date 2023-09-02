#include "chatrecord.h"

ChatRecord::ChatRecord(QString from_username, QString to_username,
                       QDateTime datetime, QString message,
                       int fontsize, QColor color, bool board,
                       bool italics, bool underline):
    _from_username(from_username), _to_username(to_username),
    _datetime(datetime), _message(message), _fontsize(fontsize),
    _color(color), _board(board), _italics(italics), _underline(underline)
{
}

/**
 * 以下为简单的Getter函数,无需注释
 */
QString ChatRecord::fromUsername(){
    return _from_username;
}
QDateTime ChatRecord::datetime(){
    return _datetime;
}
QString ChatRecord::message(){
    return _message;
}
bool ChatRecord::board(){
    return _board;
}
bool ChatRecord::italics(){
    return _italics;
}
bool ChatRecord::underline(){
    return _underline;
}
int ChatRecord::fontsize(){
    return _fontsize;
}
QColor ChatRecord::color(){
    return _color;
}
