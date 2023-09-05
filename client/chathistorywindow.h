/*
 * 文件名称：chathistorywindow.h
 * 类名称：ChatHistoryWindow（聊天记录窗口）
 * 描述：用于显示以往聊天记录，并根据日期查询聊天记录
 * 成员变量：
 * 1.   ui:ChatHistoryWindow*   窗口界面
 * 2.   _from_username:QString  保存聊天信息发送者用户名
 * 3.   _to_username:QString    保存聊天信息接收者用户名
 * 4.   _records:ChatRecordList 保存并显示聊天记录
 * 初版完成时间：2023.08.24
 * 做成时间：----。--。--
 * 作者：刘文景
 */
#ifndef CHATHISTORYWINDOW_H
#define CHATHISTORYWINDOW_H

#include <QWidget>
#include "chatrecordlist.h"
#include "mihoyolauncher.h"
namespace Ui {
class ChatHistoryWindow;
}

class ChatHistoryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatHistoryWindow(
            QString from_username,
            QString to_username,
            MiHoYoLauncher *launcher,
            QWidget *parent = nullptr);
    ~ChatHistoryWindow();
    void getRecordsByServer(QDate start, QDate end);

private slots:
    void onQueryButtonClicked();
public slots:
    void onChatHistoryFeedbackSignal(QVector<ChatRecord> records);
private:
    Ui::ChatHistoryWindow *ui;
    MiHoYoLauncher *launcher;
    QString _from_username, _to_username;

    ChatRecordList _records;
signals:
    void chatHistoryRequestSignal(QDate,QDate);

};

#endif // CHATHISTORYWINDOW_H
