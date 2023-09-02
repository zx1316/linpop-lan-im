#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QMovie"
#include "QTimer"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->movie_label->setParent(this);
    ui->movie_label->setScaledContents(true);
    QMovie* movie = new QMovie("C:\\Users\\29050\\Desktop\\R-C.gif");
    movie->setScaledSize(QSize(ui->movie_label->width(),ui->movie_label->height()));
    ui->movie_label->setMovie(movie);
    movie->start();
    connect(movie, &QMovie::frameChanged, [=](int frameNumber) {
            // GIF 动画执行一次就结束
            if (frameNumber == movie->frameCount() - 100000000) {
                movie->stop();
            }
    });
    timer = new QTimer();
    timer->start(10000);
    connect(timer,SIGNAL(timeout()),this,SLOT(onTimeout()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::onTimeout(){
    close();
    timer->stop();
    delete timer;
}
