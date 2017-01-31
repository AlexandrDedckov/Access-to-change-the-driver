#include "queuerequests.h"
#include "mainwindow.h"
#include "model/car.h"

QueueRequests::QueueRequests(QObject *parent) :
    QObject(parent)
{
    m_request = MainWindow::getRequest();
}

void QueueRequests::connectRequest()
{
    if(is_connected)    return;
    connect(m_request, SIGNAL(finishedRequest(QueueItem *)),
            SLOT(nextRequest(QueueItem *)));
    is_connected = true;
}

void QueueRequests::add(IRequest * obj, const QString & query, Request::Command cmd, QVariant lastData, QueueRequests::Priority priority)
{
    QueueItem * item = new QueueItem(this);
    item->obj = obj;
    item->query = query;
    item->cmd = cmd;
    item->priority = (int)priority;
    item->lastData = lastData;

    if(item->obj != NULL)
    {
        connect(item->obj, SIGNAL(objRemove()),
            SLOT(queryObjRemove()));
    }

    if(priority == QueueRequests::High)
    {
        switch(cmd)
        {
        case Request::DriverSelectMonth:
            addQueueItem(item);
            break;
        case Request::DriverAttachCar:
        {
            if(lastRequests.count() == 1 &&
               lastRequests[0]->cmd == Request::DriverRemoveCar)
                lastRequests.append(item);
            else
            {
                if(lastRequests.count() > 0)
                    addLastQueueItems();
                lastRequests.append(item);
            }
            emit undoSetEnabled(true);
        }
            break;
        case Request::DriverRemoveCar:
        case Request::CarSetTimeChangeDriver:
        case Request::DriverSetSchedule:
        case Request::DriverSetWork:
        case Request::DriverSetNotWork:
        case Request::MonthSetDayValue:
        {
            if(lastRequests.count() > 0) //если отменяемый запрос, и последний был отменяемым
            {
                addLastQueueItems();
                lastRequests.append(item);
            }
            else                        //если отменяемый запрос, и последний не был отменяемым
                lastRequests.append(item);
            emit undoSetEnabled(true);
        }
            break;
        case Request::DriverSetState:
        {
            if(lastRequests.count() == 1 &&
               (lastRequests[0]->cmd == Request::DriverRemoveCar ||
                lastRequests[0]->cmd == Request::DriverAttachCar))
                lastRequests.insert(0, item);
            else if(lastRequests.count() == 1 &&
                    lastRequests[0]->cmd == Request::DriverSetState)
                lastRequests.append(item);
            else
            {
                if(lastRequests.count() > 0)
                    addLastQueueItems();
                lastRequests.append(item);
            }
            emit undoSetEnabled(true);
        }
            break;
        case Request::MonthSetDriverNullReplaceable:
        case Request::MonthSetDriverReplaced:
        {
            foreach (QueueItem * lastitem, lastRequests)
            {
                if(lastitem->cmd != Request::MonthSetDriverNullReplaceable &&
                   lastitem->cmd != Request::MonthSetDriverReplaced &&
                   lastitem->cmd != Request::MonthSetDriverReplaceable)
                    addQueueItem(lastitem);
            }
            lastRequests.append(item);
        }
            break;
        case Request::MonthSetDriverReplaceable:
        {
            lastRequests.append(item);
            emit undoSetEnabled(true);
        }
            break;
        default:
        {
            if(lastRequests.count() > 0)
                addLastQueueItems();
            addQueueItem(item);
            emit undoSetEnabled(false);
        }
            break;
        }
    }
    else
        addQueueItem(item);
}

void QueueRequests::undo()
{
    undoLastQueueItems();

    emit undoSetEnabled(false);
}

void QueueRequests::save()
{
    addLastQueueItems();
    emit undoSetEnabled(false);
}

void QueueRequests::addLastQueueItems()
{
    foreach (QueueItem * lastitem, lastRequests)
        addQueueItem(lastitem);
    lastRequests.clear();
}

void QueueRequests::undoLastQueueItems()
{
    std::list<QueueItem *> listLastRequest = lastRequests.toStdList();
    for(std::list<QueueItem *>::reverse_iterator i = listLastRequest.rbegin(); i!=listLastRequest.rend(); ++i)
    {
        QueueItem * lastitem = *i;
        switch(lastitem->cmd)
        {
        case Request::CarSetTimeChangeDriver:
        case Request::DriverSetSchedule:
        case Request::DriverAttachCar:
        case Request::DriverRemoveCar:
        case Request::DriverSetWork:
        case Request::DriverSetNotWork:
        case Request::DriverSetState:
        case Request::MonthSetDayValue:
        case Request::MonthSetDriverNullReplaceable:
        case Request::MonthSetDriverReplaceable:
        case Request::MonthSetDriverReplaced:
        {
            lastitem->obj->undo(lastitem->cmd, lastitem->lastData);
            if(lastitem->cmd == Request::MonthSetDayValue ||
               lastitem->cmd == Request::MonthSetDriverNullReplaceable ||
               lastitem->cmd == Request::MonthSetDriverReplaceable ||
               lastitem->cmd == Request::MonthSetDriverReplaced)
            {
                QList<QVariant> list = lastitem->lastData.toList();
                DayValue * dv = (DayValue *)list[1].toULongLong();
                if(dv != NULL)  dv->deleteLater();
            }
            lastitem->deleteLater();
        }
            break;
        default:    break;
        }
    }
    lastRequests.clear();
}

void QueueRequests::addQueueItem(QueueItem * item)
{
    if(lastRequests.contains(item))
    {
        if(item->cmd == Request::MonthSetDayValue ||
           item->cmd == Request::MonthSetDriverNullReplaceable ||
           item->cmd == Request::MonthSetDriverReplaceable ||
           item->cmd == Request::MonthSetDriverReplaced)
        {
            QList<QVariant> list = item->lastData.toList();
            DayValue * dv = (DayValue *)list[1].toULongLong();
            if(dv != NULL)
                dv->deleteLater();
        }
        lastRequests.removeAll(item);
    }

    m_requests.append(item);

    if(m_requests.count() == 1 &&
        m_request->IsFinished())
        startRequest();
}

void QueueRequests::startRequest()
{
    if(m_requests.count() == 0)
    {
        emit finished();
        return;
    }
    bool is_high = false;

    if(m_requests[0]->priority == (int)QueueRequests::Low)
    {
        foreach (QueueItem * item, m_requests)
        {
            if(item->priority != (int)QueueRequests::Low)
            {
                is_high = true;
                break;
            }
        }

        for(int i = 0; i < m_requests.count() && is_high; i++)
        {
            QueueItem * item = m_requests[i];

            if(item->priority == (int)QueueRequests::Low)
            {
                m_requests.removeAll(item);
                item->deleteLater();
                i = -1;
            }
        }
    }

    m_request->setData(m_requests[0]);
    m_request->start();

    if(is_high)
        emit newUserRequest();
}

int QueueRequests::stopAutoUpdateRequests()
{
    for(int i = 0; i < m_requests.count(); i++)
    {
        QueueItem * item = m_requests[i];

        if(item->priority == (int)QueueRequests::Low)
        {
            m_requests.removeAll(item);
            item->deleteLater();
            i = -1;
        }
    }

    return m_requests.count();
}

void QueueRequests::clear()
{
    foreach (QueueItem * lastitem, lastRequests)
    {
        QList<QVariant> list = lastitem->lastData.toList();
//        DayValue * dv = (DayValue *)list[1].toULongLong();
//        if(dv != NULL)  dv->deleteLater();
        lastitem->deleteLater();
    }
    lastRequests.clear();

    for(int i = 0; i < m_requests.count(); i++)
    {
        QueueItem * item = m_requests[i];

        m_requests.removeAll(item);
        item->deleteLater();
        i = -1;
    }
}

void QueueRequests::nextRequest(QueueItem * obj)
{
    if(m_requests.indexOf(obj) > -1)
        m_requests.removeAll(obj);
    obj->deleteLater();

    startRequest();
}

bool QueueRequests::isFinished()
{
    return m_requests.count() == 0 && m_request->isFinished();
}

void QueueRequests::queryObjRemove()
{
    IRequest* ireq = qobject_cast<IRequest*>(QObject::sender());
    if(ireq == NULL)    return;

    foreach (QueueItem * item, m_requests)
    {
        if(item->obj == ireq)
        {
            item->obj = NULL;
            break;
        }
    }

    foreach (QueueItem * item, lastRequests)
    {
        if(item->obj == ireq)
        {
            item->obj = NULL;
            break;
        }
    }
}
