#ifndef SIGNALINGUPDATE_H
#define SIGNALINGUPDATE_H

#include <QObject>
#include <QTcpSocket>
#include <QFile>

struct Buffer
{
    int ID;
    char Message[501];
    Buffer() : ID(0) { ::memset(&Message, 0, 501); }
};

class SignalingUpdate : public QObject
{
    Q_OBJECT
private:
    const QString cdata = "-1|-1:[auto_update|";
    QTcpSocket * m_pTcpSocket = NULL;
    bool isConnected = false;
    QString ip;
    int indexConnections = 0;
    int m_id_park;
    QByteArray m_sendData;
    bool is_run = false;

    QString getIp();

public:
    explicit SignalingUpdate(QObject *parent = 0);
    bool IsRun()   { return is_run; }
    void start(int id_park);
    void stop();
    ~SignalingUpdate() {}

signals:
    void error(QString err);
    void update();

public slots:
    void sUpdate();

private slots:
    void slotConnected();
    void slotReadyRead();
    void slotError(QAbstractSocket::SocketError);
};

#endif // SIGNALINGUPDATE_H
