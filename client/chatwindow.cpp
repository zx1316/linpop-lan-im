#include "chatwindow.h"
#include "ui_chatwindow.h"
#include "QMessageBox"
#include "QDebug"
#include "friendinformation.h"
#include "autostart.h"

ChatWindow::ChatWindow(
        QString from_username,
        QString to_username,
        bool type,
        QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatWindow),
    _from_username(from_username),
    _to_username(to_username),
    _records(nullptr)
{
    ui->setupUi(this);
    ui->transfer_file_button->setEnabled(type);
    this->setWindowTitle("与"+to_username+"的聊天窗口");
    _records=ChatRecordList(ui->chat_record_browser);
    _chat_history_window = nullptr;

    connect(ui->to_chat_history_button,SIGNAL(clicked()),this,SLOT(onToChatHistoryButtonClicked()));
    connect(ui->sendButton,SIGNAL(clicked()),this,SLOT(onSendButtonClicked()));
    connect(ui->transfer_file_button,SIGNAL(clicked()),this,SLOT(onTransferFileButtonClicked()));

    connect(ui->board_button,&QToolButton::toggled,this,&ChatWindow::onBoardButtonToggled);
    connect(ui->italics_button,&QToolButton::toggled,this,&ChatWindow::onItalicsButtonToggled);
    connect(ui->underline_button,&QToolButton::toggled,this,&ChatWindow::onUnderlineButtonToggled);

    connect(ui->font_size_combobox,&QComboBox::currentTextChanged,this,&ChatWindow::onFontSizeComboBoxCurrentTextChanged);
    connect(ui->color_combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(onColorComboBoxCurrentIndexChanged(int)));

}
const QColor ChatWindow::_colormap[]={QColor(0,0,0),QColor(255,0,0),QColor(0,255,0),QColor(0,0,255),QColor(255,255,0)};

ChatWindow::~ChatWindow()
{
    if(_transfer_file_window!=nullptr){
        _transfer_file_window->close();
        delete _transfer_file_window;
    }
    if(_chat_history_window!=nullptr){
        _chat_history_window->close();
        delete _chat_history_window;
    }
    delete ui;
}
//QHash<QString,QColor> ChatWindow::_colormap={
//    {QString("黑色"),QColor(0,0,0)},
//    {QString("红色"),QColor(255,0,0)},
//    {QString("绿色"),QColor(0,255,0)},
//    {QString("蓝色"),QColor(0,0,255)},
//    {QString("黄色"),QColor(255,255,0)}
//};

void ChatWindow::onFontSizeComboBoxCurrentTextChanged(const QString &arg1)
{
    _fontsize = arg1.toInt();
    setTextStyle();
}
void ChatWindow::onColorComboBoxCurrentIndexChanged(int arg1)
{
    switch (arg1) {
        case 0:_color.setRgb(0,0,0);break;
        case 1:_color.setRgb(255,0,0);break;
        case 2:_color.setRgb(0,255,0);break;
        case 3:_color.setRgb(0,0,255);break;
        case 4:_color.setRgb(255,255,0);break;
    }
    setTextStyle();
}
void ChatWindow::onBoardButtonToggled(bool checked)
{
    _board=checked;
    setTextStyle();
}

void ChatWindow::onItalicsButtonToggled(bool checked)
{
    _italics=checked;
    setTextStyle();
}

void ChatWindow::onUnderlineButtonToggled(bool checked)
{
    _underline=checked;
    setTextStyle();
}

void ChatWindow::onToChatHistoryButtonClicked()
{
    gachaAutoStart(this);
    if(_chat_history_window){
        _chat_history_window->show();
        return;
    }
    _chat_history_window=new ChatHistoryWindow(_from_username,_to_username);
    connect(_chat_history_window,&ChatHistoryWindow::chatHistoryRequestSignal,this,&ChatWindow::onChatHistoryRequestSignal);
    _chat_history_window->show();
}


void ChatWindow::onTransferFileButtonClicked(){
    gachaAutoStart(this);
    _transfer_file_window = new TransferFileWindow();
    _transfer_file_window->setWindowTitle("向" + _to_username + "发送文件");
    ui->transfer_file_button->setEnabled(false);
    connect(_transfer_file_window,&TransferFileWindow::transferFileRequestSignal,this,&ChatWindow::onTranferFileRequestSignal);
    connect(_transfer_file_window,&TransferFileWindow::closeWindowSignal,this,&ChatWindow::onCloseWindowSignal);
    _transfer_file_window->show();
}

void ChatWindow::onCloseWindowSignal(){
    delete _transfer_file_window;
    ui->transfer_file_button->setEnabled(true);
}

//反馈和请求响应

/*
 * 函数名:onSendButtonClicked
 * 功能描述:在点击发送按钮时调用,向IndexWindow传输请求。（请求起点）
 */
void ChatWindow::onSendButtonClicked(){
    gachaAutoStart(this);
    if(ui->input_textbox->toPlainText().toUtf8().size()>1336){
        QMessageBox::information(this,"发送失败","消息过长");
        qDebug("ChatWindow Send Message Request Fail");
        return;
    }else if(ui->input_textbox->toPlainText().isEmpty()){
        QMessageBox::information(this,"发送失败","不能发送空白消息");
        return;
    }
    ui->sendButton->setEnabled(false);
    qDebug("ChatWindow Send Message Request");
    emit sendMessageRequestSignal(_to_username,ui->input_textbox->toPlainText(),_color,_board,_italics,_underline,_fontsize);
}

/*
 * 函数名:onSendMessageFeedbackSignal
 * 功能描述:在收到IndexWindow的发送消息反馈时调用，显示发送记录。该函数由IndexWindow调用。（反馈终点）
 */
void ChatWindow::onSendMessageFeedbackSignal(bool feedback)
{
    ui->sendButton->setEnabled(true);
    if(feedback){
        qDebug("ChatWindow Send Message Feedback");
        ChatRecord record(
                    _from_username, _to_username, QDateTime::currentDateTime(),
                    ui->input_textbox->toPlainText(), _fontsize,
                    _color, _board, _italics, _underline);
        _records.append(record);
        ui->input_textbox->clear();
    }else{
        QMessageBox::critical(this,"失败","发送失败");
    }
}

/*
 * 函数名:onChatRecentRecordFeedbackSignal
 * 功能描述:在收到IndexWindow发来的最近聊天记录反馈时调用,将聊天记录展示。该函数由IndexWindow调用。（反馈终点）
 */
void ChatWindow::onChatRecentRecordFeedbackSignal(QVector<ChatRecord> records){
    qDebug("ChatWindow Chat Recent Records Feedback");
    if(_chat_history_window!=nullptr){
        _chat_history_window->onChatHistoryFeedbackSignal(records);
    }else{
        for(ChatRecord record:records){
            _records.append(record);
        }
    }
}

/*
 * 函数名:onNewMessageSignal
 * 功能描述:在收到IndexWindow发来的新消息时调用，将收到的消息展示。该函数由IndexWindow调用。(反馈终点)
 */
void ChatWindow::onNewMessageSignal(QString username,QString msg,QColor color,bool board,bool italics,bool underline,int fontsize){
    QApplication::beep();
    qDebug("ChatWindow New Message Signal");
    ChatRecord record(username,_to_username,QDateTime::currentDateTime(),
                      msg,fontsize,color,board,italics,underline);
    _records.append(record);
}

/*
 * 函数名:onTransferFileRequestSignal
 * 功能描述:在收到TransferFileWindow发来的传输文件请求时调用,发出请求信号。该函数由connect信号调用。（请求中转）
 */

void ChatWindow::onTranferFileRequestSignal(QString filename,qint64 filesize){
    qDebug()<<_to_username;
    emit transferFileRequestSignal(_to_username,filename,filesize);
}

/*
 * 函数名:onTransferFileFeedbackSignal
 * 功能描述:在收到IndexWindow发来的传输文件反馈时调用，直接调用TransferFileWindow的相应函数。（反馈中转）
 */
void ChatWindow::onTransferFileFeedbackSignal(bool feedback,QString ip,int port){
    qDebug("ChatWindow Transfer File Feedback");
    _transfer_file_window->onTransferFileFeedbackSignal(feedback,ip,port);
}

/*
 * 函数名:onChatHistoryRequestSignal
 * 功能描述:在收到ChatHistoryRequest发来的聊天历史记录请求时调用，发出请求信号。该函数由connect信号调用。（请求中转）
 */
void ChatWindow::onChatHistoryRequestSignal(QDate start,QDate end){
    qDebug("ChatWindow Chat History Request");
    emit chatHistoryRequestSignal(_to_username,start,end);
}

/*
 * 函数名:onChatHistoryFeedbackSignal
 * 功能描述:在收到IndexWindow发来的聊天历史记录反馈时调用，直接调用ChatHistoryWindow的相应函数。（反馈中转）
 */
//void ChatWindow::onChatHistoryFeedbackSignal(QVector<ChatRecord> records){
//    qDebug("ChatWindow Chat History Feedback");
//    _chat_history_window->onChatHistoryFeedbackSignal(records);
//}
/**
 * 函数名称：setTextStyle()
 * 描述：设置将要发送都是聊天信息的格式信息
 * 参数：void
 * 返回值: void
 * 做成时间：2023.08.24
 * 作者：刘文景
 */
void ChatWindow::setTextStyle(){
    QString style = QString("");
    style.append("QTextEdit{");
    style.append("font: ");
    style.append(QString::number(_fontsize));
    style.append("pt;");
    style.append("color: rgb(");
    style.append(QString::number(_color.red()));
    style.append(",");
    style.append(QString::number(_color.green()));
    style.append(",");
    style.append(QString::number(_color.blue()));
    style.append(");");
    style.append("font-weight: ");
    style.append(_board?"bold;":"normal;");
    style.append("font-style: ");
    style.append(_italics?"italic;":"normal;");
    style.append("text-decoration: ");
    style.append(_underline?"underline":"none");
    style.append(";}");
    ui->input_textbox->setStyleSheet(style);
}
