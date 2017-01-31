#ifndef OBTAININGSERVER_H
#define OBTAININGSERVER_H

#include <QThread>
#include <QVector>
#include "serverconfig.h"
#include <QMap>
#include <QtNetwork>
#include <QtCore>

class Server : public QObject
{
public:
    explicit Server(const QByteArray & address,
                    const quint16 port,
                    QObject *parent = 0) :
        QObject(parent),
        Address(address),
        Port(port)
    {}
    const QByteArray Address;
    const quint16 Port;
};

class ObtainingServer : public QThread
{
    Q_OBJECT
private:
    const QByteArray mask;
    QVector<const Server *> serversGLE;
    QVector<const Server *> servers;
    QMap<QByteArray, QByteArray> dec;
    QVector<quint16> newLoop;
    int now_index = 0;
    int count_repeat = 0;

    void run();
    void decodeINI(const QByteArray & code, QByteArray & output);
    bool getAddress(const QString & key, const QByteArray & data, QString & ip, quint16 & port);

public:
    explicit ObtainingServer(QObject *parent = 0);
    bool isError = false;

signals:
    void error(QString err);
    void finishedRequest();

private slots:
    void readyRead();
};

#endif // OBTAININGSERVER_H
