/**
 * 文件名称：chatrecordlist.h
 * 类名称：ChatRecordList（聊天记录列表）
 * 描述：用于保存并显示复数的聊天记录
 * 成员变量：
 * 1.   _records:vector<ChatRecord> 保存聊天记录
 * 2.   _browser:QTextBrowser*      保存将要显示聊天信息的区域的指针
 * 初版完成时间：2023.08.24
 * 做成时间：----。--。--
 * 作者：刘文景
 */
#ifndef CHATRECORDLIST_H
#define CHATRECORDLIST_H

#include "chatrecord.h"
#include "QVector"
#include "QTextBrowser"

class ChatRecordList
{
public:
    ChatRecordList(QTextBrowser* browser);
    void append(ChatRecord record);
    QTextBrowser* _browser;
};

#endif // CHATRECORDLIST_H
