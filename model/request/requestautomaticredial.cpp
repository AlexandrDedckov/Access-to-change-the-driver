#include "requestautomaticredial.h"
#include "txt_constants.h"

RequestAutomaticRedial::RequestAutomaticRedial(QObject *parent) :
    QThread(parent)
{
}

void RequestAutomaticRedial::setData(const QString& data)
{
    m_data = data.toLocal8Bit();
}

void RequestAutomaticRedial::run()
{
    const QString address = tr("http://192.168.212.36/api");
    const int port = 9090;

    QUrl url(address);
    url.setPort(port);
    QNetworkRequest request(url);
    request.setRawHeader("Accept-Encoding", "gzip, deflate");
    request.setRawHeader("Authorization", "Basic " + QByteArray(QString("%1:%2").arg(api_name).arg(api_pass).toLatin1()).toBase64());

    request.setHeader(QNetworkRequest::ContentTypeHeader, tr("application/x-www-form-urlencoded"));
    request.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(m_data.length()));

    QNetworkAccessManager netManager;
    QNetworkReply * netReply = netManager.post(request, m_data.toUtf8());
    connect(netReply, SIGNAL(finished()), this, SLOT(readyRead()));
    exec();
}

void RequestAutomaticRedial::readyRead()
{
    QNetworkReply *netReply = qobject_cast<QNetworkReply *>(QObject::sender());
    if (netReply->error() == QNetworkReply::NoError)
    {
        const QByteArray data = netReply->readAll();
        qDebug() << data;
        netReply->deleteLater();
        if(data.indexOf(TXT_QUERY_OK) <= -1)
            qDebug() << "[ERROR RequestAutomaticRedial]" << data;
    }
    else
        qDebug() << netReply->errorString();
    emit finishThreed();
}
