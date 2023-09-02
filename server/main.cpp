#include <QCoreApplication>
#include "server.h"
#include <iostream>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    quint16 port;
    if (argc >= 2) {
        port = atoi(argv[1]);
    } else {
        qDebug() << "enter port:";
        std::cin >> port;
    }

    Server server(port);
    return a.exec();
}
