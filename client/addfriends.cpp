#include "addfriends.h"
#include "ui_addfriends.h"

AddFriends::AddFriends(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddFriends)
{
    ui->setupUi(this);
}

AddFriends::~AddFriends()
{
    delete ui;
}
