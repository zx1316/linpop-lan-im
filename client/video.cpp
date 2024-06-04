#include <QPalette>
#include "video.h"
#include "ui_video.h"

Video::Video(IndexWindow *w, QWidget *parent) : QWidget(parent), ui(new Ui::Video), w(w) {
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0, 0, 0, 0));
    setPalette(pal);
    setAttribute(Qt::WA_DeleteOnClose);
    player = new QMediaPlayer;
    videoWidget = new QVideoWidget;
    player->setMedia(QUrl("qrc:///video/launch.avi"));
    player->setVideoOutput(videoWidget);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    setLayout(layout);
    layout->addWidget(videoWidget);
    connect(player, &QMediaPlayer::stateChanged, this, &Video::onPlayerStateChange);
    player->play();
}

Video::~Video() {
    delete videoWidget;
    delete player;
    delete ui;
}

void Video::onPlayerStateChange() {
    if (player->state() == QMediaPlayer::StoppedState) {
        w->show();
        this->close();
    }
}
