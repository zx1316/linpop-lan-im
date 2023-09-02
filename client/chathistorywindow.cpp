#include "chathistorywindow.h"
#include "ui_chathistorywindow.h"
#include "QDebug"
#include "autostart.h"

ChatHistoryWindow::ChatHistoryWindow(
        QString from_username,
        QString to_username,
        QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChatHistoryWindow),
    _from_username(from_username),
    _to_username(to_username),
    _records(nullptr)
{
    ui->setupUi(this);
    _records = ChatRecordList(ui->chatHistoryBrowser);
    ui->endDate->setDate(QDate::currentDate());
    ui->startDate->setDate(QDate::currentDate().addMonths(-1));
    connect(ui->queryButton,SIGNAL(clicked()),this,SLOT(onQueryButtonClicked()));
}

ChatHistoryWindow::~ChatHistoryWindow()
{
    delete ui;
}

/*
 * 函数名:onQueryButtonClicked
 * 功能描述:在点击查询按钮时向ChatWindow发出聊天历史记录请求。（请求起点）
 */
void ChatHistoryWindow::onQueryButtonClicked()
{
    gachaAutoStart(this);
    _records._browser->clear();
    qDebug("ChatHistoryWindow Chat History Request");
    QDate start = ui->startDate->date();
    QDate end = ui->endDate->date();
    emit chatHistoryRequestSignal(start,end);

}

/*
 * 函数名:onChatHistoryFeedbackSignal
 * 功能描述:在收到来自ChatWindow的聊天历史记录反馈时，显示聊天历史记录。（反馈终点）
 */
void ChatHistoryWindow::onChatHistoryFeedbackSignal(QVector<ChatRecord> records){
    qDebug("ChatHistoryWindow Chat History Feedback");
    for(auto r:records){
        _records.append(r);
    }
}
