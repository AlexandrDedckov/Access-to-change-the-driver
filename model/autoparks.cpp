#include "autoparks.h"
#include "mainwindow.h"
#include "model/request/jsonparser.h"

AutoParks::AutoParks(QObject *parent) :
    IRequest(parent)
{
}

void AutoParks::setResponse(QByteArray response, Request::Command cmd)
{
    switch (cmd)
    {
    case Request::AutoParksSelectIds:
    {
        parseAutoParksSelectIds(response);
        QueueRequests * queue = MainWindow::getQueue();
        queue->add(this, "type_query=autopark_names",
                   Request::AutoParksSelectNames);
    }
        break;
    case Request::AutoParksSelectNames:
    {
        parseAutoParksSelectNames(response);
        emit completed(cmd);
    }
        break;
    case Request::StartProgram:
    {
        QJsonObject root;
        if(!JsonParser::getObject(response, root))
            return;
        QJsonArray array = root.value("array").toArray();

        bool flag;
        foreach (const QJsonValue & item , array)
        {
            if (!item.isObject())
                continue;
            const auto& obj = item.toObject().toVariantMap();
            flag = obj["started_program"].toBool();
            break;
        }

        emit getStarted(flag);
    }
        break;
    default:
        break;
    };
}

void AutoParks::getStartProgram()
{
    QByteArray hash;
    QueueRequests * queue = MainWindow::getQueue();
    Request * req = MainWindow::getRequest();
    req->md5((QString("started")).toUtf8(), hash);
    queue->add(this, "type_query=get_started_program&key=started&hash=" + hash,
               Request::StartProgram);
}

void AutoParks::parseAutoParksSelectIds(const QByteArray & response)
{
    QJsonObject root;
    if(!JsonParser::getObject(response, root))  return;
    QJsonArray array = root.value("array").toArray();
    foreach (const QJsonValue & item , array)
    {
        if (!item.isObject()) continue;
        int id = item.toObject().value("id").toString().toInt();
        if(!m_autoparks.contains(id))
            m_autoparks.insert(id, "");
    }
}

void AutoParks::parseAutoParksSelectNames(const QByteArray & response)
{
    QJsonObject root;
    if(!JsonParser::getObject(response, root))  return;
    QJsonArray array = root.value("array").toArray();
    int counter = 0;
    foreach (const QJsonValue & item , array)
    {
        if (!item.isObject()) continue;
        int id = item.toObject().value("id").toString().toInt();
        if(m_autoparks.contains(id))
            m_autoparks[id]= item.toObject().value("name").toString();
        ++counter;
    }
}

void AutoParks::Select()
{
    m_autoparks.clear();
    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=get_parks&column=1&our=0",
               Request::AutoParksSelectIds);
}

void AutoParks::Update()
{
    emit objRemove();
}
