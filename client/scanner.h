#ifndef SCANNER_H
#define SCANNER_H

#include <QObject>

class Scanner : public QObject {
    Q_OBJECT
public:
    explicit Scanner(const QString &root, QObject *parent = nullptr);
private:
    QString root;
signals:
    void found(QString path);
    void finish(QString root);
public slots:
    void scan();
};

#endif // SCANNER_H
