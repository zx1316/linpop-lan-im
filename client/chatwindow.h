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
#include "chatrecordlist.h"
#include "transferfilewindow.h"
#include "chathistorywindow.h"


namespace Ui {
class ChatWindow;
}

struct ColorMap{
    QString name;
    QColor color;
};

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(
            QString from_username,
            QString to_username,
            bool,
            MiHoYoLauncher *,
            QWidget *parent = nullptr);

    ~ChatWindow();

    void onSendMessageFeedbackSignal(bool);

    void onChatRecentRecordFeedbackSignal(QVector<ChatRecord>);

    void onNewMessageSignal(QString,QString,QColor,bool,bool,bool,int);

private slots:
    void onFontSizeComboBoxCurrentTextChanged(const QString &arg1);

//    void onColorComboBoxCurrentTextChanged(const QString &arg1);

    void onBoardButtonToggled(bool checked);

    void onItalicsButtonToggled(bool checked);

    void onUnderlineButtonToggled(bool checked);

    void onSendButtonClicked();

    void onToChatHistoryButtonClicked();

    void onTransferFileButtonClicked();

    void onCloseWindowSignal();

    void onTranferFileRequestSignal(QString,qint64);

    void onChatHistoryRequestSignal(QDate,QDate);

    void onColorComboBoxCurrentIndexChanged(int arg1);

public slots:
    void onTransferFileFeedbackSignal(bool,QString,int);

//    void onChatHistoryFeedbackSignal(QVector<ChatRecord>);

private:
    Ui::ChatWindow *ui;

    static const QColor _colormap[];

    QString _from_username, _to_username;

    int _fontsize = 9;

    QColor _color = QColor(0,0,0);

    bool _board = false, _italics = false, _underline = false;

    ChatRecordList _records;

    void setTextStyle();

    TransferFileWindow* _transfer_file_window;

    ChatHistoryWindow* _chat_history_window;
    MiHoYoLauncher *launcher;
signals:
    void sendMessageRequestSignal(QString,QString,QColor,bool,bool,bool,int);

    void transferFileRequestSignal(QString,QString,qint64);

    void chatHistoryRequestSignal(QString,QDate,QDate);
};

#endif // CHATWINDOW_H
