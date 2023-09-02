#include "testwindow.h"
#include "ui_testwindow.h"

TestWindow::TestWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestWindow)
{
    ui->setupUi(this);
}

TestWindow::~TestWindow()
{
    delete ui;
}

void TestWindow::on_toReceiveWindow_clicked()
{
    QWidget *test = new ReceiveFileWindow("不科学御兽.txt",8907813,31416);
    test->show();
}

void TestWindow::on_toSendWindow_clicked()
{
    QWidget *test = new TransferFileWindow("123", "127.0.0.1", 31416);
    test->show();
}
