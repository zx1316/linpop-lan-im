#include "chatwindow.h"
#include "ui_chatwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QCryptographicHash>
#include <QDesktopServices>
#include <QWindowStateChangeEvent>

ChatWindow::ChatWindow(const QString &sender, const QString &receiver, const QString &serverIp, MiHoYoLauncher *launcher, QWidget *parent) : QWidget(parent), ui(new Ui::ChatWindow), selfName(sender), receiver(receiver), serverIp(serverIp), launcher(launcher) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    if (receiver[0] == '_') {
        this->setWindowTitle("聊天室：" + receiver.right(receiver.length() - 1));
    } else {
        this->setWindowTitle("与" + receiver + "的聊天窗口");
        ui->memberButton->setHidden(true);
    }
    ui->chatBrowser->setOpenLinks(false);
    inputColor.setRgb(0, 0, 0);
    ui->inputTextbox->setStyleSheet("color:rgb(0,0,0);");

    connect(ui->sendButton, &QPushButton::clicked, this, &ChatWindow::onSendButtonClicked);
    connect(ui->fileButton, &QPushButton::clicked, this, &ChatWindow::onFileButtonClicked);
    connect(ui->pictureButton, &QPushButton::clicked,this, &ChatWindow::onPictureButtonClicked);
    connect(ui->historyButton, &QPushButton::clicked, this, &ChatWindow::onHistoryButtonClicked);
    connect(ui->fontButton, &QPushButton::clicked, this, &ChatWindow::onFontButtonClicked);
    connect(ui->memberButton, &QPushButton::clicked, this, &ChatWindow::onMemberButtonClicked);
    connect(ui->closeButton, &QPushButton::clicked, this, &ChatWindow::close);
    connect(ui->chatBrowser, &QTextBrowser::anchorClicked, this, &ChatWindow::onAnchorClicked);
}

ChatWindow::~ChatWindow() {
    if (fontWindow != nullptr) {
        fontWindow->close();
    }
    if (transferFileWindow != nullptr) {
        transferFileWindow->close();
    }
    if (chatHistoryWindow != nullptr) {
        chatHistoryWindow->close();
    }
    if (groupFileWindow != nullptr) {
        groupFileWindow->close();
    }
    if (memberWindow != nullptr) {
        memberWindow->close();
    }
    emit windowClosed(receiver);
    delete ui;
}

void ChatWindow::onAnchorClicked(QUrl url) {
    launcher->gachaLaunch();
    QDesktopServices::openUrl(url);
}

void ChatWindow::onUpdateFont(QFont font, QColor color) {
    launcher->gachaLaunch();
    inputColor = color;
    ui->inputTextbox->setFont(font);
    QString colorStr = "color:rgb(%1,%2,%3);";
    ui->inputTextbox->setStyleSheet(colorStr.arg(QString::number(color.red()), QString::number(color.green()), QString::number(color.blue())));
}

void ChatWindow::onTranferFileRequestSignal(QString fileName, qint64 size) {
    emit transferFileRequestSignal(receiver, fileName, size);
}

void ChatWindow::onChatHistoryRequestSignal(QDate start, QDate end) {
    emit chatHistoryRequestSignal(receiver, start, end);
}

// 群成员窗口请求查询槽函数，即刻转发
void ChatWindow::onGroupMemberRequestSignal() {
    launcher->gachaLaunch();
    emit groupMemberRequestSignal(receiver);
}
// 群文件窗口请求查询槽函数，即刻转发
void ChatWindow::onGroupFileQuerySignal() {
    emit groupFileQuerySignal(receiver);
}
// 群文件窗口请求删除槽函数，即刻转发
void ChatWindow::onGroupFileDeleteSignal(QString fileName) {
    emit groupFileDeleteSignal(receiver, fileName);
}
// 群文件窗口请求下载槽函数，即刻转发
void ChatWindow::onGroupFileDownloadSignal(QString fileName, quint16 port) {
    emit groupFileDownloadSignal(receiver, fileName, port);
}
// 群文件窗口请求上传槽函数，即刻转发
void ChatWindow::onGroupFileUploadSignal(QString fileName, qint64 size, quint16 port) {
    emit groupFileUploadSignal(receiver, fileName, size, port);
}

void ChatWindow::onFontButtonClicked() {
    launcher->gachaLaunch();
    fontWindow = new FontSelectorWindow(ui->inputTextbox->font(), inputColor);
    connect(fontWindow, &FontSelectorWindow::updateFont, this, &ChatWindow::onUpdateFont);
    connect(fontWindow, &FontSelectorWindow::windowClosed, this, &ChatWindow::onFontWindowClosed);
    fontWindow->show();
    ui->fontButton->setDisabled(true);
}

void ChatWindow::onPictureButtonClicked() {
    launcher->gachaLaunch();
    auto fileName = QFileDialog::getOpenFileName(this, "选择一张图片", "../", "Images (*.png *.jpg *.gif *.bmp)");
    if (!fileName.isNull()) {
        QFileInfo info(fileName);
        QString suffix = info.suffix();
        if (info.size() > 1048576) {
            //太大了，返回
            QMessageBox::information(this, "发送失败", "图片大小超过1MB，请考虑发送文件");
            return;
        }
        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QByteArray array = file.readAll();
        file.close();
        QString md5Name = QCryptographicHash::hash(array, QCryptographicHash::Md5).toHex();
        QFile file1(QCoreApplication::applicationDirPath() + "/images/" + md5Name + "." + suffix);
        if (!file1.exists()) {
            file1.open(QIODevice::WriteOnly);
            file1.write(array);
            file1.close();
        }
        ui->sendButton->setDisabled(true);
        emit sendMessageRequestSignal(receiver, md5Name + "." + suffix, "img");
    }
}

void ChatWindow::onFileButtonClicked() {
    launcher->gachaLaunch();
    if (receiver[0] == '_') {
        groupFileWindow = new GroupFileWindow(selfName, serverIp, launcher);
        groupFileWindow->setWindowTitle(receiver.right(receiver.length() - 1) + "的群文件");
        connect(groupFileWindow, &GroupFileWindow::groupFileDeleteSignal, this, &ChatWindow::onGroupFileDeleteSignal);
        connect(groupFileWindow, &GroupFileWindow::groupFileDownloadSignal, this, &ChatWindow::onGroupFileDownloadSignal);
        connect(groupFileWindow, &GroupFileWindow::groupFileUploadSignal, this, &ChatWindow::onGroupFileUploadSignal);
        connect(groupFileWindow, &GroupFileWindow::groupFileQuerySignal, this, &ChatWindow::onGroupFileQuerySignal);
        connect(groupFileWindow, &GroupFileWindow::windowClosed, this, &ChatWindow::onGroupFileWindowClosed);
        groupFileWindow->show();
        onGroupFileQuerySignal();
    } else {
        transferFileWindow = new TransferFileWindow(launcher);
        transferFileWindow->setWindowTitle("向" + receiver + "发送文件");
        connect(transferFileWindow, &TransferFileWindow::transferFileRequestSignal, this, &ChatWindow::onTranferFileRequestSignal);
        connect(transferFileWindow, &TransferFileWindow::windowClosed, this, &ChatWindow::onTransferFileWindowClosed);
        transferFileWindow->show();
    }
    ui->fileButton->setDisabled(true);
}

void ChatWindow::onMemberButtonClicked() {
    launcher->gachaLaunch();
    memberWindow = new GroupMemberWindow;
    memberWindow->setWindowTitle(receiver.right(receiver.length() - 1) + "的成员列表");
    connect(memberWindow, &GroupMemberWindow::GroupMemberRequestSignal, this, &ChatWindow::onGroupMemberRequestSignal);
    connect(memberWindow, &GroupMemberWindow::windowClosed, this, &ChatWindow::onMemberWindowClosed);
    memberWindow->show();
    onGroupMemberRequestSignal();
    ui->memberButton->setDisabled(true);
}

void ChatWindow::onHistoryButtonClicked() {
    launcher->gachaLaunch();
    chatHistoryWindow = new ChatHistoryWindow(launcher);
    if (receiver[0] == '_') {
        chatHistoryWindow->setWindowTitle(receiver.right(receiver.length() - 1) + "的消息记录");
    } else {
        chatHistoryWindow->setWindowTitle(receiver + "的消息记录");
    }
    connect(chatHistoryWindow, &ChatHistoryWindow::chatHistoryRequestSignal, this, &ChatWindow::onChatHistoryRequestSignal);
    connect(chatHistoryWindow, &ChatHistoryWindow::windowClosed, this, &ChatWindow::onHistoryWindowClosed);
    chatHistoryWindow->show();
    ui->historyButton->setDisabled(true);
}

void ChatWindow::onSendButtonClicked() {
    launcher->gachaLaunch();
    QString content = ui->inputTextbox->toPlainText();
    if (content.length() == 0) {
        QMessageBox::critical(this, "无法发送", "发送内容不能为空");
        return;
    }
    if (content.toUtf8().size() > 3072) {
        QMessageBox::critical(this, "无法发送", "发送内容过长");
        return;
    }
    content.replace('&', "&amp;");
    content.replace(' ', "&ensp;");
    content.replace('<', "&lt;");
    content.replace('>', "&gt;");
    content.replace('"', "&quot;");
    content.replace('\n', "<br>");
    QString span = "<span style=\"font-family:%1;font-size:%2pt;color:rgb(%3,%4,%5);\">%6%7%8%9%10%11%12</span>";
    QFont font = ui->inputTextbox->font();
    QString completed = span.arg(font.family(),
                                 QString::number(font.pointSize()),
                                 QString::number(inputColor.red()),
                                 QString::number(inputColor.green()),
                                 QString::number(inputColor.blue()),
                                 font.bold() ? "<b>" : "",
                                 font.italic() ? "<i>" : "",
                                 font.underline() ? "<u>" : "",
                                 content,
                                 font.bold() ? "</b>" : "",
                                 font.italic() ? "</i>" : "",
                                 font.underline() ? "</u>" : "");
    ui->inputTextbox->clear();
    ui->sendButton->setDisabled(true);
    emit sendMessageRequestSignal(receiver, completed, "text");
}

void ChatWindow::onTransferFileWindowClosed() {
    transferFileWindow = nullptr;
    ui->fileButton->setDisabled(false);
}

void ChatWindow::onGroupFileWindowClosed() {
    groupFileWindow = nullptr;
    ui->fileButton->setDisabled(false);
}

void ChatWindow::onMemberWindowClosed() {
    memberWindow = nullptr;
    ui->memberButton->setDisabled(false);
}

void ChatWindow::onFontWindowClosed() {
    fontWindow = nullptr;
    ui->fontButton->setDisabled(false);
}

void ChatWindow::onHistoryWindowClosed() {
    chatHistoryWindow = nullptr;
    ui->historyButton->setDisabled(false);
}

// 主窗口发来的传送文件反馈槽函数
void ChatWindow::onAcceptTransferFileSignal(const QString &ip, int port) {
    if (transferFileWindow != nullptr) {
        transferFileWindow->onAcceptTransferFileSignal(ip, port);
    }
}

void ChatWindow::onRejectTransferFileSignal() {
    if (transferFileWindow != nullptr) {
        transferFileWindow->onRejectTransferFileSignal();
    }
}

// 主窗口发来的收到历史记录槽函数
void ChatWindow::onReceiveHistorySignal(const QList<ChatRecord> &list) {
    QString html1;
    QString head1 = "<span style=\"font-size:9pt;color:#008040\">%1&emsp;%2</span><br>";
    QString head2 = "<span style=\"font-size:9pt;color:#0000FF\">%1&emsp;%2</span><br>";
    for (auto item : list) {
        if (item.sender == selfName) {
            html1 += head1.arg(item.sender, QDateTime::fromMSecsSinceEpoch(item.timestamp).toString("yyyy/MM/dd hh:mm:ss"));
        } else {
            html1 += head2.arg(item.sender, QDateTime::fromMSecsSinceEpoch(item.timestamp).toString("yyyy/MM/dd hh:mm:ss"));
        }
        if (item.type == "text") {
            html1 += item.msg;
        } else if (item.type == "img") {
            QString img = "<a href=\"%1\"><img src=\"%1\"></a>";
            html1 += img.arg("file:///" + QCoreApplication::applicationDirPath() + "/images/" + item.msg);
        }
        html1 += "<br>";
    }
    if (isRecordOk && chatHistoryWindow != nullptr) {
        // 导向聊天记录窗口
        chatHistoryWindow->refreshRecords(html1);
    } else {
        // 导向本窗口
        htmlToShow = html1;
        ui->chatBrowser->setHtml(htmlToShow);
        ui->chatBrowser->moveCursor(QTextCursor::End);
        isRecordOk = true;
    }
}
// 主窗口发来的新消息槽函数
void ChatWindow::onNewMessageSignal(const QString &sender, const QString &msg, const QString &type) {
    QString head;
    if (sender == selfName) {
        head = "<span style=\"font-size:9;color:#008040\">%1&emsp;%2</span><br>";
    } else {
        head = "<span style=\"font-size:9;color:#0000FF\">%1&emsp;%2</span><br>";
    }
    htmlToShow += head.arg(sender, QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"));
    if (type == "text") {
        htmlToShow += msg;
    } else if (type == "img") {
        QString img = "<a href=\"%1\"><img src=\"%1\"></a>";
        htmlToShow += img.arg("file:///" + QCoreApplication::applicationDirPath() + "/images/" + msg);
    }
    htmlToShow += "<br>";
    ui->chatBrowser->setHtml(htmlToShow);
    ui->chatBrowser->moveCursor(QTextCursor::End);
}
// 主窗口发来的成员列表槽函数
void ChatWindow::onGroupMemberSignal(const QList<QString> &list) {
    if (memberWindow != nullptr) {
        memberWindow->onGroupMemberSignal(list);
    }
}
// 主窗口发来的群文件列表槽函数
void ChatWindow::onGroupFileSignal(const QList<GroupFile> &list) {
    if (groupFileWindow != nullptr) {
        groupFileWindow->refreshFileList(list);
    }
}

void ChatWindow::onSendMessageSuccessSignal(const QString &msg, const QString &type) {
    onNewMessageSignal(selfName, msg, type);
    ui->sendButton->setDisabled(false);
}
