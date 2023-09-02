#ifndef VIDEO_H
#define VIDEO_H

#include <QWidget>
#include <QtMultimediaWidgets>
#include <QMediaPlayer>
#include <QCoreApplication>
#include <requesttoserver.h>

namespace Ui {
class Video;
}

class Video : public QWidget
{
    Q_OBJECT

public:
    explicit Video(RequestToServer *client, QString username, QWidget *parent = nullptr);
    ~Video();

private:
    Ui::Video *ui;
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    RequestToServer *_client;
    QString username;
public slots:
    void onPlayerStateChange();
};

#endif // VIDEO_H
