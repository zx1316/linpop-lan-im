#ifndef VIDEO_H
#define VIDEO_H

#include <QWidget>
#include <QtMultimediaWidgets>
#include <QMediaPlayer>
#include <QCoreApplication>
#include "indexwindow.h"

namespace Ui {
class Video;
}

class Video : public QWidget {
    Q_OBJECT

public:
    explicit Video(IndexWindow *w, QWidget *parent = nullptr);
    ~Video();

private:
    Ui::Video *ui;
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    IndexWindow *w;

private slots:
    void onPlayerStateChange();
};

#endif // VIDEO_H
