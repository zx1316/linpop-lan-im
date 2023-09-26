#include "loginwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // 单例常驻主函数栈
    Network network;
    MiHoYoLauncher launcher;
    launcher.startScan();
    LoginWindow *w = new LoginWindow(&network, &launcher, "");
    w->show();
    return a.exec();
}
