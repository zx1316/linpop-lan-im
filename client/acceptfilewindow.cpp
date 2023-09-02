#include "acceptfilewindow.h"
#include "ui_acceptfilewindow.h"

acceptFileWindow::acceptFileWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::acceptFileWindow)
{
    ui->setupUi(this);
}

acceptFileWindow::~acceptFileWindow()
{
    delete ui;
}
