#include "request.h"
#include "txt_constants.h"
#include "mainwindow.h"
#include "model/request/qcompressor.h"
#include "md5.h"
#include "model/request/jsonparser.h"

Request::Request(QObject *parent) :
    QThread(parent),
    api_general(tr("/api/api_general_taxi_spb.php")),
    api_secondary(tr("/api/api_autoplan_taxi_spb.asp"))
    //     локальный сервер
//  api_secondary(tr("/api/api_autoplan_taxi_spb.php"))
{
    qRegisterMetaType<Request::Command>("Request::Command");
    connect(this, SIGNAL(finished()), SLOT(sFinished()));
}

void Request::run()
{

    now_index = 0;
    is_finished = false;
    isRun = true;
    isErr = false;
    do
    {
        QString address = "";
        quint16 port = 80;

        if(m_cmd == AutoParksSelectIds ||
                m_cmd == DriversSelect ||
                m_cmd == CarsSelect ||
                m_cmd == DriverAttachCar ||
                m_cmd == DriverRemoveCar ||
                m_cmd == CarSetTimeChangeDriver)
        {

            if (server_1)
                address = tr("http://192.168.211.30") + api_general;
            else if (server_2)
                address = tr("http://192.168.212.30") + api_general;
            else if (server_3)
                address = tr("http://192.168.213.30") + api_general;
            port = 81;
        }
        else
        {
            // локальный сервер
            //            address = tr("http://192.168.52.128") + api_secondary;
            //            port = 80;

            if (server_1)
                address = tr("http://192.168.211.30") + api_secondary;
            else if (server_2)
                address = tr("http://192.168.212.30") + api_secondary;
            else if (server_3)
                address = tr("http://192.168.213.30") + api_secondary;
            port = 81;
        }

        //qDebug() << "[ADDRESS]: " << address << "[PORT]: " << port << "[COMMAND]: " << m_cmd;
        QString data;
        QByteArray t_query;

        m_crypto.encrypt(m_query.toLocal8Bit(), t_query);
        data = tr("*=") + t_query;

        QUrl url(address);
        url.setPort(port);

        QNetworkRequest req(url);
        req.setRawHeader("Accept-Encoding", "gzip, deflate");
        req.setRawHeader("Authorization", "Basic " + QByteArray(QString("CallBackUser:WilFjh1GgT2To").toLatin1()).toBase64());

        req.setHeader(QNetworkRequest::ContentTypeHeader, tr("application/x-www-form-urlencoded"));
        req.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(data.length()));
        QNetworkAccessManager netManager;

        QNetworkReply * netReply = netManager.post(req, data.toUtf8());
        connect(netReply, SIGNAL(finished()), this, SLOT(readyRead()));
        ++now_index;
        exec();
    }
    while(count_repeat > now_index && isRun);
}

void Request::readyRead()
{
//    qDebug() << "[REQUEST]" << m_cmd;
    QNetworkReply * netReply = qobject_cast<QNetworkReply *>(QObject::sender());
    if (netReply->error() == QNetworkReply::NoError)
    {
        QByteArray jsonData;
        QByteArray data = netReply->readAll();
        netReply->deleteLater();
        if(data.indexOf("status") > -1)
            setResponse(data);
        else
        {
            if(QCompressor::gzipDecompress(data, jsonData))
                setResponse(jsonData);
            else
            {
                if(now_index >= count_repeat)
                    isErr = true;
                exit();
            }
        }
    }
    else
    {
        qDebug() << netReply->errorString();
        netReply->deleteLater();
        if (count_repeat == 2)
        {
            server_1 = false;
            server_2 = true;
            server_3 = false;
        }
        else if (count_repeat == 3)
        {
            server_1 = false;
            server_2 = false;
            server_3 = true;
        }
        if(now_index >= count_repeat)
            isErr = true;
        exit();
    }
}

void Request::setResponse(const QByteArray & data)
{
    if(data.indexOf(TXT_QUERY_OK) > -1)
        m_response = data;
    else
    {
        QString err = "";
        parseError(data, err);
        isErr = true;
        emit error(err);
    }
    isRun = false;
    if(!isFinished())
        exit();
}

void Request::sFinished()
{
    if(!isErr && isRun) return;

    if(m_currentObj->obj == NULL)
    {
        is_finished = true;
        QueueItem * obj = m_currentObj;
        m_currentObj = NULL;
        emit finishedRequest(obj);
        return;
    }

    try
    {
        disconnect(m_currentObj->obj, SIGNAL(objRemove()),
                   this, SLOT(queryObjRemove()));
        QueueItem * obj = m_currentObj;
        m_currentObj = NULL;
        if(!isErr)
        {
            obj->obj->setResponse(m_response, m_cmd);

            switch(m_cmd) {
            case MonthSetDayValue:
            case MonthSetDriverNullReplaceable:
            case MonthSetDriverReplaceable:
            case MonthSetDriverReplaced:
            case DriverAttachCar:
            case DriverRemoveCar:
            case DriverSetSchedule:
            case DriverSetWork:
            case DriverSetNotWork:
            case DriverAddNode:
            case DriverEditNode:
            case DriverRemoveNode:
            case DriverSetState:
            case DriverSetNotResponding: // not responding
            case DriverTimeToSleep:
            case DriverIntercession:
            case CarSetTimeChangeDriver:
            case InsertDriverSpoofingSelect:
                emit sendUpdate();
                break;
            default:
                break;
            };
        }

        is_finished = true;
        emit finishedRequest(obj);
    }
    catch(...) {}
}

void Request::setData(QueueItem * item)
{
    m_query = tr("name=") + api_name + tr("&pass=") + api_pass + tr("&") + item->query;
    m_cmd = item->cmd;

    m_currentObj = item;

    if(m_currentObj->obj != NULL)
    {
        connect(m_currentObj->obj, SIGNAL(objRemove()),
                SLOT(queryObjRemove()));
    }
}

void Request::parseError(const QByteArray & resp, QString & err)
{
    QJsonObject root;
    qDebug() << endl << "[REQUEST ERROR]" << m_cmd << endl;
    if(!JsonParser::getObject(resp, root) ||
            !root.contains(TXT_ERR_TITLE))
    {
        err = MSG_ERROR_SERVER;
        return;
    }
    err = root[TXT_ERR_TITLE].toString();
}

void Request::queryObjRemove()
{
    IRequest* ireq = qobject_cast<IRequest*>(QObject::sender());
    if(ireq == NULL)    return;

    if(m_currentObj->obj == ireq)
        m_currentObj->obj = NULL;
}

void Request::md5(const QByteArray & data, QByteArray & output)
{
    const char * t_ascii = data.data();
    MD5 t_md5;
    output = t_md5.digestString(t_ascii);
}
