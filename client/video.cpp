#include "video.h"
#include "ui_video.h"
#include "indexwindow.h"
#include <QPalette>

Video::Video(RequestToServer *client, QString username, QWidget *parent) : QWidget(parent), ui(new Ui::Video), _client(client), username(username) {
    ui->setupUi(this);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0, 0, 0, 0));
    setPalette(pal);
    setAttribute(Qt::WA_DeleteOnClose);
    player = new QMediaPlayer;
    videoWidget = new QVideoWidget;
    player->setMedia(QUrl("qrc:///launch.avi"));
//    player->setVolume(0);
    player->setVideoOutput(videoWidget);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);
    layout->addWidget(videoWidget);
    connect(player, &QMediaPlayer::stateChanged, this, &Video::onPlayerStateChange);
    player->play();
}

Video::~Video() {
    videoWidget->deleteLater();
    disconnect(player);
    player->deleteLater();
    delete ui;
}

void Video::onPlayerStateChange() {
    if (player->state() == QMediaPlayer::StoppedState) {
        IndexWindow *w = new IndexWindow(username, _client->getLocalAddress(), _client); //转入主界面
        w->show();
        emit close();
    }
}
