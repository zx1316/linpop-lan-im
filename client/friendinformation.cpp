#include "friendinformation.h"
#include "ui_friendinformation.h"
#include <QCryptographicHash>

/*
 * 函数名称：FriendInformation(构造函数)
 * 功能描述：初始化FriendInformation信息，并根据参数设置QLabel显示内容，连接信号和槽函数等
 * 作者:林方裕
*/
FriendInformation::FriendInformation(Friend& f,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FriendInformation)
{
    ui->setupUi(this);
    //根据好友信息设置相应标签内容
    ui->username_label->setText(f._name);
    ui->ip_label->setText(f._ip);
    _state = f._state;
    _real = f._real;
    ui->picture_label->setIcon(f._real?QIcon(mapStringToPicture(f._name)):QIcon("://6.png"));
    this->setStyleSheet(_state?"#widget{border:1px solid black;background:Cyan}":"#widget{border:1px solid black;background:gray}");

    //将undefinedButton取出,置为公有,以便于接入函数
    this->undefined_button = ui->undefinedButton;

    //将信号和槽函数连接
    connect(ui->username_label,&DoubleClickedLabel::doubleClickedSignal,this,&FriendInformation::onDoubleClickedSignal);
    connect(ui->ip_label,&DoubleClickedLabel::doubleClickedSignal,this,&FriendInformation::onDoubleClickedSignal);
    connect(ui->picture_label,&DoubleClickedLabel::doubleClickedSignal,this,&FriendInformation::onDoubleClickedSignal);
    connect(ui->undefinedButton,SIGNAL(clicked()),this,SLOT(onUndefinedButtonClicked()));
}


FriendInformation::~FriendInformation()
{
    qDebug("delete FriendInformation");
    delete ui;
}

QString FriendInformation::mapStringToPicture(QString &inputString) {
    static QMap<int,QString> map={{0,QString("://0.jpg")},{1,QString("://1.ico")},{2,QString("://2.png")},{3,QString("://3.png")},
                                  {4,QString("://4.jpg")}};
    QByteArray hash = QCryptographicHash::hash(inputString.toUtf8(), QCryptographicHash::Sha1);

    int hashValue = static_cast<unsigned char>(hash.at(0));

    int mappedNumber = hashValue % 5;


    return map.find(mappedNumber).value();
}

void FriendInformation::onUndefinedButtonClicked(){
    emit undefinedButtonClickedSignal(this);
}

void FriendInformation::onDoubleClickedSignal(){
    emit doubleClickedSignal(this);
}

QString FriendInformation::username(){
    return ui->username_label->text();
}
void FriendInformation::setBackgroundColor(bool flag){
    if(flag){
        this->setStyleSheet("#widget{border:1px solid black;background-color:chartreuse}");
    }else{
        this->setStyleSheet(_state?"#widget{border:1px solid black;background:Cyan}":"#widget{border:1px solid black;background:gray}");
    }
}
