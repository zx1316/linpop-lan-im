#include <QApplication>
#include "loginwindow.h"
#include "mihoyolauncher.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MiHoYoLauncher::getInstance().startScan();
    LoginWindow *w = new LoginWindow("");
    w->show();
    return a.exec();
}
