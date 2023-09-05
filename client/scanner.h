#ifndef SCANNER_H
#define SCANNER_H

#include <QObject>

class Scanner : public QObject {
    Q_OBJECT
public:
    explicit Scanner(QString root, QObject *parent = nullptr);
private:
    QString root;
signals:
    void finish(QString root);
    void found(QString path);
public slots:
    void scan();
};

#endif // SCANNER_H
