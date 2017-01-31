#include "user.h"
#include "model/request/jsonparser.h"
#include "model/request/obtainingserver.h"
#include "txt_constants.h"

User * User::m_current = NULL;

User::User(QObject *parent) :
    IRequest(parent)
{
    ObtainingServer * m_os = new ObtainingServer(this);
    connect(m_os, SIGNAL(finishedRequest()), this, SLOT(setVersion()));
    m_os->start();
}

void User::Login()
{
    Request * request = MainWindow::getRequest();
    QByteArray hash;
    request->md5((m_login + m_password).toUtf8(), hash);
    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=authorization&login=" + QUrl::toPercentEncoding(m_login) +
               "&lpass=" + QUrl::toPercentEncoding(m_password) +
               "&hash=" + hash,
               Request::UserLogin);
    /*request->setData(this, "type_query=login&login=" + QUrl::toPercentEncoding(m_login) +
                     "&lpass=" + QUrl::toPercentEncoding(m_password) + "&hash=" + hash,
                     Request::UserLogin);
    request->start();*/
}

void User::setVersion()
{
    Request * request = MainWindow::getRequest();
    QByteArray hash;
    request->md5((TXT_VERSION).toUtf8(), hash);
    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=current_version&version=" + TXT_VERSION +
               "&hash=" + hash, Request::SetVersion);
}

void User::SelectUsers()
{
    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=select_users",
                     Request::UserSelect);
}

void User::setResponse(QByteArray response, Request::Command cmd)
{
    switch (cmd)
    {
    case Request::UserLogin:
    {
        if(response.indexOf(TXT_QUERY_OK) < 0)    //Потом ВЕРНУТЬ
        {
            QString err = "";
            parseError(response, err);
            emit error(err);
        }
        else
        {
            QJsonObject root;
            if(!JsonParser::getObject(response, root))
            {
                emit error(MSG_ERROR_SERVER);
                return;
            }
            m_id = root.value("id").toString().toULongLong();
            m_fio = root.value("fio").toString();
            m_access = (Access)root.value("access").toString().toInt();
            m_id_park = root.value("id_park").toString().toLongLong();
            m_current = this;
            emit completed(cmd);
        }
    }
        break;
    case Request::UserSelect:
    {
        foreach (User * user, m_users)  user->deleteLater();
        m_users.clear();

        QJsonObject root;
        if(!JsonParser::getObject(response, root)) return;
        QJsonArray array = root.value("array").toArray();
        foreach (const QJsonValue & item , array)
        {
            if (!item.isObject()) continue;
            QJsonObject obj = item.toObject();
            User * user = new User(this);
            user->setId(obj.value("id").toString().toULongLong());
            user->setLogin(obj.value("login").toString());
            user->setPassword(obj.value("password").toString());
            user->setFio(obj.value("fio").toString());
            user->setAccess((Access)obj.value("access").toString().toInt());
            user->setIdPark(obj.value("id_park").toString().toLongLong());
            m_users.append(user);
        }
        emit completed(cmd);
    }
        break;
    case Request::UserInsert:
    {
        QJsonObject root;
        if(!JsonParser::getObject(response, root)) return;
        this->setId(root.value("id").toString().toULongLong());
    }
        break;
    case Request::SetVersion:
    {
        QJsonObject root;
        if(!JsonParser::getObject(response, root))  return;
        QJsonArray array = root.value("array").toArray();
        foreach (const QJsonValue & item , array)
        {
            if (!item.isObject()) continue;
            QJsonObject obj = item.toObject();
            version = obj.value("version").toString();
        }

        QStringList list = version.split(".");
        int countVersion = 0;
        for (QString & str : list)
            countVersion += str.toInt();
    }
    default:
        break;
    }
}

void User::setId(quint64 id)
{
    m_id = id;
}

void User::setAccess(User::Access access)
{
    m_access = access;
}

void User::setLogin(const QString & login)
{
    m_login = login;
}

void User::setPassword(const QString & password)
{
    m_password = password;
}

void User::setFio(const QString & fio)
{
    m_fio = fio;
}

void User::setIdPark(qint64 id_park)
{
    m_id_park = id_park;
}

User * User::Create()
{
    User * user = new User(this);
    m_users.append(user);

    return user;
}

void User::Remove(User * user)
{
    if(!m_users.contains(user)) return;
    m_users.removeAll(user);
    user->Delete();
    user->deleteLater();
}

void User::parseError(const QByteArray & resp, QString & err)
{
    QJsonObject root;
    if(!JsonParser::getObject(resp, root) ||
       !root.contains(TXT_ERR_TITLE))
    {
        err = MSG_ERROR_SERVER;
        return;
    }
    err = root[TXT_ERR_TITLE].toString();
}

void User::Insert()
{
    Request * request = MainWindow::getRequest();
    QByteArray hash;
    request->md5((m_login + m_password + m_fio +
                  QString::number((int)m_access) +
                  QString::number(m_id_park)).toUtf8(), hash);

    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=user_add&login=" + QUrl::toPercentEncoding(m_login) +
                     "&lpass=" + QUrl::toPercentEncoding(m_password) +
                     "&fio=" + QUrl::toPercentEncoding(m_fio) +
                     "&access=" + QString::number((int)m_access) +
                     "&id_park=" + QString::number(m_id_park) +
                     "&hash=" + hash,
                     Request::UserInsert);
}

void User::Update()
{
    Request * request = MainWindow::getRequest();
    QByteArray hash;
    request->md5((QString::number(m_id) + m_login + m_password + m_fio +
                  QString::number((int)m_access) + QString::number(m_id_park)).toUtf8(), hash);

    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=user_update&id=" + QString::number(m_id) +
                     "&login=" + QUrl::toPercentEncoding(m_login) +
                     "&lpass=" + QUrl::toPercentEncoding(m_password) +
                     "&fio=" + QUrl::toPercentEncoding(m_fio) +
                     "&access=" + QString::number((int)m_access) +
                     "&id_park=" + QString::number(m_id_park) +
                     "&hash=" + hash,
               Request::UserUpdate);
}

void User::Delete()
{
    Request * request = MainWindow::getRequest();
    QByteArray hash;
    request->md5(QString::number(m_id).toUtf8(), hash);

    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=user_delete&id=" + QString::number(m_id) +
                     "&hash=" + hash,
                     Request::UserDelete);
}
