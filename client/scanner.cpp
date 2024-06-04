#include <QDirIterator>
#include "scanner.h"

Scanner::Scanner(const QString& root, QObject *parent) : QObject(parent), root(root) {

}

void Scanner::scan() {
    QDirIterator it(root, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString item = it.next();
        if (item.endsWith("YuanShen.exe") || item.endsWith("StarRail.exe")) {
            emit found(item);
        }
    }
    emit finish(root);
}
