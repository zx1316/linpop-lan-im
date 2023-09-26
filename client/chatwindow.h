/**
 * 文件名称：chatwindow.h
 * 类名称：ChatWindow（聊天窗口）
 * 描述：用于与其他用户进行即时通讯的窗口
 * 初版完成时间：2023.08.24
 * 做成时间：----。--。--
 * 成员变量：
 * 1.   ui:ChatHistoryWindow*   窗口界面
 * 2.   _from_username:QString  保存聊天信息发送者用户名
 * 3.   _to_username:QString    保存聊天信息接收者用户名
 * 4.   _records:ChatRecordList 保存并显示聊天记录
 * 5.   _fontsize:int           保存字号
 * 6.   _color:QColor           保存字体颜色
 * 7.   _board:bool             保存字体加粗信息
 * 8.   _italics:bool           保存字体斜体信息
 * 9.   _underline:bool         保存字体下划线信息
 * 10.  _colormap:ColorMap*     保存颜色与相应文字的映射(static)
 * 作者：刘文景
 */
#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QMainWindow>
#include "transferfilewindow.h"
#include "chathistorywindow.h"
#include "fontselectorwindow.h"
#include "groupfilewindow.h"
#include "groupmemberwindow.h"

namespace Ui {
class ChatWindow;
}

class ChatWindow : public QWidget {
    Q_OBJECT

public:
    explicit ChatWindow(const QString &fromName, const QString &toName, const QString &serverIp, MiHoYoLauncher *, QWidget *parent = nullptr);
    ~ChatWindow();
    // 主窗口发来的传送文件反馈
    void onAcceptTransferFileSignal(const QString &ip, int port);
    void onRejectTransferFileSignal();
    // 主窗口发来的历史记录
    void onReceiveHistorySignal(const QList<ChatRecord> &);
    // 主窗口发来的新消息
    void onNewMessageSignal(const QString &sender, const QString &msg, const QString &type);
    // 主窗口发来的成员列表
    void onGroupMemberSignal(const QList<QString> &);
    // 主窗口发来的群文件列表
    void onGroupFileSignal(const QList<GroupFile> &);
    void onSendMessageSuccessSignal(const QString &msg, const QString &type);

private:
    Ui::ChatWindow *ui;
    QString selfName;
    QString receiver;
    QString serverIp;
    QString htmlToShow;
    TransferFileWindow *transferFileWindow = nullptr;
    GroupFileWindow *groupFileWindow = nullptr;
    ChatHistoryWindow *chatHistoryWindow = nullptr;
    FontSelectorWindow *fontWindow = nullptr;
    GroupMemberWindow *memberWindow = nullptr;
    MiHoYoLauncher *launcher;
    QColor inputColor;
    bool isRecordOk = false;

private slots:
    // 字体窗口更新槽函数
    void onUpdateFont(QFont font, QColor color);
    // 文件传输窗口请求发送文件槽函数，即刻转发
    void onTranferFileRequestSignal(QString, qint64);
    // 历史记录窗口请求查询槽函数，即刻转发
    void onChatHistoryRequestSignal(QDate, QDate);
    // 群成员窗口请求查询槽函数，即刻转发
    void onGroupMemberRequestSignal();
    // 群文件窗口请求查询槽函数，即刻转发
    void onGroupFileQuerySignal();
    // 群文件窗口请求删除槽函数，即刻转发
    void onGroupFileDeleteSignal(QString fileName);
    // 群文件窗口请求下载槽函数，即刻转发
    void onGroupFileDownloadSignal(QString fileName, quint16 port);
    // 群文件窗口请求上传槽函数，即刻转发
    void onGroupFileUploadSignal(QString fileName, qint64 size, quint16 port);

    // 本窗口按钮点击的槽函数
    void onFontButtonClicked();
    void onPictureButtonClicked();
    void onFileButtonClicked();
    void onMemberButtonClicked();
    void onHistoryButtonClicked();
    void onSendButtonClicked();
    void onAnchorClicked(QUrl url);

    // 本窗口的子窗口关闭后的槽函数
    void onTransferFileWindowClosed();
    void onGroupFileWindowClosed();
    void onMemberWindowClosed();
    void onFontWindowClosed();
    void onHistoryWindowClosed();

signals:
    // 本窗口被关闭信号
    void windowClosed(QString name);
    void sendMessageRequestSignal(QString name, QString msg, QString type);
    void transferFileRequestSignal(QString receiver, QString fileName, qint64 size);
    void chatHistoryRequestSignal(QString name, QDate start, QDate end);
    void groupMemberRequestSignal(QString groupName);
    void groupFileQuerySignal(QString groupName);
    // 请求删除群文件信号
    void groupFileDeleteSignal(QString groupName, QString fileName);
    // 请求下载群文件信号
    void groupFileDownloadSignal(QString groupName, QString fileName, quint16 port);
    // 请求上传群文件信号
    void groupFileUploadSignal(QString groupName, QString fileName, qint64 size, quint16 port);
};

#endif // CHATWINDOW_H
