#include "chathistorywindow.h"
#include "ui_chathistorywindow.h"
#include <QDesktopServices>

ChatHistoryWindow::ChatHistoryWindow(MiHoYoLauncher *launcher, QWidget *parent) : QWidget(parent), ui(new Ui::ChatHistoryWindow), launcher(launcher) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    ui->endDate->setDate(QDate::currentDate());
    ui->startDate->setDate(QDate::currentDate().addMonths(-1));
    ui->chatHistoryBrowser->setOpenLinks(false);
    connect(ui->chatHistoryBrowser, &QTextBrowser::anchorClicked, this, &ChatHistoryWindow::onAnchorClicked);
    connect(ui->queryButton, SIGNAL(clicked()), this, SLOT(onQueryButtonClicked()));
}

ChatHistoryWindow::~ChatHistoryWindow() {
    emit windowClosed();
    delete ui;
}

/*
 * 函数名:onQueryButtonClicked
 * 功能描述:在点击查询按钮时向ChatWindow发出聊天历史记录请求。（请求起点）
 */
void ChatHistoryWindow::onQueryButtonClicked() {
    launcher->gachaLaunch();
    QDate start = ui->startDate->date();
    QDate end = ui->endDate->date();
    ui->queryButton->setDisabled(true);
    emit chatHistoryRequestSignal(start, end);
}

void ChatHistoryWindow::refreshRecords(const QString& html) {
    ui->chatHistoryBrowser->setHtml(html);
    ui->chatHistoryBrowser->moveCursor(QTextCursor::End);
    ui->queryButton->setDisabled(false);
}

void ChatHistoryWindow::onAnchorClicked(const QUrl& url) {
    QDesktopServices::openUrl(url);
}

