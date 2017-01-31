#include "signalingupdate.h"
#include <QNetworkInterface>
#include "mainwindow.h"

SignalingUpdate::SignalingUpdate(QObject *parent) :
    QObject(parent)
{
    ip = getIp();
}

void SignalingUpdate::start(int id_park)
{
    if (IsRun())
        stop();
    is_run = true;
    m_id_park = id_park;
    m_pTcpSocket = new QTcpSocket(this);
    ServerConfig * serverConfig = MainWindow::getServerConfig();
    m_pTcpSocket->connectToHost(serverConfig->API_UPDATE_IP, serverConfig->API_UPDATE_PORT);
    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReadyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotError(QAbstractSocket::SocketError)));
}

void SignalingUpdate::stop()
{
    if(m_pTcpSocket != NULL)
    {
        m_pTcpSocket->close();
        m_pTcpSocket->deleteLater();
    }
    m_pTcpSocket = NULL;
    is_run = false;
}

void SignalingUpdate::slotReadyRead()
{
    if(m_pTcpSocket == NULL)
        return;
    QByteArray arr = m_pTcpSocket->readAll();
//    qDebug() << "[UPDATE]" << QDateTime::currentDateTime() << arr << endl;
    if(m_sendData.length() > 0)
    {
        m_pTcpSocket->write(m_sendData);
        m_sendData.clear();
    }
    if(arr.length() == 0)
        return;

    Buffer buff;
    ::memcpy(&buff, arr.data(), sizeof(buff));

    QString t_data(buff.Message);
    int startIndex = t_data.indexOf(cdata);
    int secondIndex = t_data.indexOf("|", startIndex + cdata.length());
    int endIndex = t_data.indexOf("]");
    if(startIndex == -1 ||
       secondIndex == -1 ||
       endIndex == -1 ||
       secondIndex <= (startIndex + cdata.length()) ||
       endIndex <= secondIndex)
        return;
    startIndex += cdata.length();
    QString t_ip = t_data.mid(startIndex, secondIndex - startIndex);
    secondIndex += 1;
    int t_id_park = t_data.mid(secondIndex, endIndex - secondIndex).toInt();
    if(ip == t_ip || t_id_park != m_id_park)
        return;
    emit update();
}

void SignalingUpdate::slotError(QAbstractSocket::SocketError err)
{
    QString strError = "Автообновление: " + (err == QAbstractSocket::HostNotFoundError ?
                    "Запрашиваемый узел не найден." : err == QAbstractSocket::RemoteHostClosedError ?
                    "Удаленный узел закрылся." : err == QAbstractSocket::ConnectionRefusedError ?
                    "Соединение было отклонено. " : QString(m_pTcpSocket == NULL ? "" : m_pTcpSocket->errorString())
                    );

    if(m_pTcpSocket != NULL)
        m_pTcpSocket->deleteLater();
    m_pTcpSocket = NULL;

    if(++indexConnections < 5)
    {
        start(m_id_park);
        return;
    }

    isConnected = false;
    emit error(strError);
}

void SignalingUpdate::slotConnected()
{
    isConnected = true;
    indexConnections = 0;
    if(m_pTcpSocket != NULL)
        m_pTcpSocket->write("-1|-1");
}

void SignalingUpdate::sUpdate()
{
    if(!isConnected)    return;

    ip = getIp();
    m_sendData.clear();
    m_sendData.append(cdata + ip + "|" + QString::number(m_id_park) + "]\n");
}

QString SignalingUpdate::getIp()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    QString ip;

    for(int nIter=0; nIter<list.count(); nIter++)
    {
        if(list[nIter].isLoopback())  continue;
        if (list[nIter].protocol() != QAbstractSocket::IPv4Protocol ) continue;
        ip = list[nIter].toString();
    }

    return ip;
}
