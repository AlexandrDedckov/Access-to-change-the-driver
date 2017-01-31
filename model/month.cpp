#include "car.h"
#include "mainwindow.h"

Month::Month(QObject *parent) : IRequest(parent)
{
}

void Month::sendDayValue(quint8 day, DayValue * dv, DayValue * lastDv, Request::Command cmd)
{
    if(dv == NULL || day == 0 || day > 31)  return;
    QByteArray hash;
    Request * req = MainWindow::getRequest();
    QDate currentDateMonth;
    if(getCurrentDriver()->getCar() == NULL &&
            dv->driver_replaceable != NULL &&
            dv->driver_replaceable->getCar() != NULL)
        currentDateMonth = dv->driver_replaceable->getCar()->getCurrentDateMonth();
    else
        currentDateMonth = getCurrentDriver()->getCurrentDateMonth();
    QString t_date = QString::number(
                QDateTime(
                    QDate(currentDateMonth.year(),
                          currentDateMonth.month(),
                          day)).toTime_t()
                );
    QString t_id_driver = QString::number(m_id_driver);
    QString t_hours = QString::number((int)dv->hours);
    QString t_notify = dv->notify ? "1" : "0";
    QString t_state = QString::number((int)dv->state);
    QString t_id_driver_replaced = QString::number(dv->id_driver_replaced);
    QString t_id_driver_replaceable = QString::number(dv->id_driver_replaceable);
    QString t_node = dv->node;
    QString id_park = QString::number(m_id_park);
    req->md5((t_id_driver + id_park + t_date + t_hours + t_notify + t_state + t_node + t_id_driver_replaced + t_id_driver_replaceable).toUtf8(), hash);

    QList <QVariant> list;
    list.append(day);
    list.append((quint64)lastDv);
    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=month_set_day_value&id_driver=" + t_id_driver +
                     "&id_park=" + id_park +
                     "&day_work=" + t_date +
                     "&hours_enum=" + t_hours +
                     "&notify=" + t_notify +
                     "&attendance=" + t_state +
                     "&node=" + QUrl::toPercentEncoding(t_node) +
                     "&id_driver_replaced=" + t_id_driver_replaced +
                     "&id_driver_replaceable=" + t_id_driver_replaceable +
                     "&hash=" + hash,
               cmd, list);
}

void Month::undo(Request::Command cmd, QVariant data)
{
    switch(cmd)
    {
    case Request::MonthSetDriverReplaced:
    case Request::MonthSetDriverReplaceable:
    case Request::MonthSetDriverNullReplaceable:
    case Request::MonthSetDayValue:
    {
        QList<QVariant> list = data.toList();
        if(list.count() != 2)   return;
        quint8 day = list[0].toUInt();
        if(day == 0)    return;
        DayValue * t_dv = (DayValue *)list[1].toULongLong();
        if(t_dv == NULL)    return;
        DayValue * dv = NULL;

        if(m_schedule.contains(day))
        {
            dv = m_schedule[day];
            dv->hours = t_dv->hours;
            dv->state = t_dv->state;
            dv->notify = t_dv->notify;
            dv->node = t_dv->node;

            dv->id_driver_replaced = t_dv->id_driver_replaced;
            dv->id_driver_replaceable = t_dv->id_driver_replaceable;
            dv->driver_replaced = t_dv->driver_replaced;
            dv->driver_replaceable = t_dv->driver_replaceable;
        }
        else
        {
            dv = t_dv->clone(this);
            dv->driver_replaceable = t_dv->driver_replaceable;
            dv->driver_replaced = t_dv->driver_replaced;
            m_schedule.insert(day, dv);
        }
        emit dataChanged(day, dv);
    }
        break;
    default: break;
    }
}

void Month::addScheduleValue(quint8 day, Hours hours, StateDriver state,
                             const QString & node, bool notify, quint64 id_replaced,
                             quint64 id_replaceable, quint64 id)
{
    DayValue * dv = new DayValue(this);
    dv->hours = hours;
    dv->notify = notify;
    dv->state = state;
    dv->node = node;
    dv->id_driver_replaced = id_replaced;
    dv->id_driver_replaceable = id_replaceable;
    dv->id = id;
    m_schedule.insert(day, dv);

    emit dataChanged(day, dv);
}

void Month::setLastDayPreviousMonth(QDate date, Hours hours, StateDriver state, const QString & node, bool notify, quint64 id_replaced, quint64 id_replaceable)
{
    lastDayPreviousMonth = new OtherDV(this);
    lastDayPreviousMonth->date = date;
    lastDayPreviousMonth->dv = new DayValue(lastDayPreviousMonth);
    lastDayPreviousMonth->dv->hours = hours;
    lastDayPreviousMonth->dv->notify = notify;
    lastDayPreviousMonth->dv->state = state;
    lastDayPreviousMonth->dv->node = node;
    lastDayPreviousMonth->dv->id_driver_replaced = id_replaced;
    lastDayPreviousMonth->dv->id_driver_replaceable = id_replaceable;
}

void Month::setFirstDayNextMonth(QDate date, Hours hours, StateDriver state,
                                 const QString & node, bool notify, quint64 id_replaced, quint64 id_replaceable)
{
    firstDayNextMonth = new OtherDV(this);
    firstDayNextMonth->date = date;
    firstDayNextMonth->dv = new DayValue(lastDayPreviousMonth);
    firstDayNextMonth->dv->hours = hours;
    firstDayNextMonth->dv->notify = notify;
    firstDayNextMonth->dv->state = state;
    firstDayNextMonth->dv->node = node;
    firstDayNextMonth->dv->id_driver_replaced = id_replaced;
    firstDayNextMonth->dv->id_driver_replaceable = id_replaceable;
}

void Month::editScheduleValue(quint8 day, Hours hours)
{
    if(hours == hOther)
    {
        if(m_schedule.contains(day))
        {
            DayValue * hv = m_schedule[day];
            hv->deleteLater();
            m_schedule.remove(day);
        }
    }

    DayValue * hv = NULL;
    DayValue * lastDv = NULL;

    if(m_schedule.contains(day))
    {
        hv = m_schedule[day];
        lastDv = hv->clone(this);
        hv->hours = hours;
        if(hours == hOutput)
            hv->notify = false;
    }
    else
    {
        hv = new DayValue(this);
        lastDv = hv->clone(this);
        hv->hours = hours;
        m_schedule.insert(day, hv);
    }


    emit dataChanged(day, hv);

    sendDayValue(day, hv, lastDv);
}

void Month::removeTheReplacement(quint8 day)
{
    if(!m_schedule.contains(day))   return;
    DayValue *dv = m_schedule[day];
    dv->id_driver_replaceable = 0;
    emit dataChanged(day, dv);
}

void Month::setDriverReplaced(quint8 day, Driver * driver_replaced)
{
    if(!m_schedule.contains(day))   return;
    DayValue *dv = m_schedule[day];
    DayValue *lastDv = dv->clone(this);
    lastDv->driver_replaceable = dv->driver_replaceable;
    lastDv->driver_replaced = dv->driver_replaced;

    dv->id_driver_replaced = driver_replaced->getId();
    dv->driver_replaced = driver_replaced;

    emit dataChanged(day, dv);
    sendDayValue(day, dv, lastDv, Request::MonthSetDriverReplaced);
}

void Month::setDriverReplaceable(quint8 day, Driver * driver_replaceable)
{
    DayValue *dv = NULL;
    DayValue *lastDv = NULL;
    if(!m_schedule.contains(day))
    {
        dv = new DayValue(this);
        m_schedule.insert(day, dv);
    }
    else
        dv = m_schedule[day];
    lastDv = dv->clone(this);
    lastDv->driver_replaceable = dv->driver_replaceable;
    lastDv->driver_replaced = dv->driver_replaced;

    if(driver_replaceable != NULL)
        dv->id_driver_replaceable = driver_replaceable->getId();
    else
        dv->id_driver_replaceable = 0;
    dv->driver_replaceable = driver_replaceable;

    emit dataChanged(day, dv);
    sendDayValue(day, dv, lastDv,
                 driver_replaceable == NULL ?
                     Request::MonthSetDriverNullReplaceable :
                     Request::MonthSetDriverReplaceable);
}

void Month::editStateDriverValue(quint8 day, StateDriver state)
{
    if(!m_schedule.contains(day))
        return;

    DayValue * dv = m_schedule[day];
    if(dv->hours == hOutput ||
       dv->hours == hOther)
        return;

    DayValue * lastDv = dv->clone(this);
    lastDv->driver_replaceable = dv->driver_replaceable;
    lastDv->driver_replaced = dv->driver_replaced;

    dv->state = state;
    if(state == stateWent)
        dv->id_driver_replaced = 0;

    emit dataChanged(day, dv);
    sendDayValue(day, dv, lastDv);
}

void Month::setNotify(quint8 day, bool notify)
{
    if(!m_schedule.contains(day))
        return;

    DayValue * dv = m_schedule[day];
    if(dv->hours == hOutput ||
       dv->hours == hOther)
        return;

    DayValue * lastDv = dv->clone(this);
    lastDv->driver_replaceable = dv->driver_replaceable;
    lastDv->driver_replaced = dv->driver_replaced;

    dv->notify = notify;

    emit dataChanged(day, dv);
    sendDayValue(day, dv, lastDv);
}

DayValue* Month::getDayValue(quint8 day)
{
    if(m_schedule.contains(day))
        return m_schedule[day];
    return NULL;
}

int Month::getCountDays()
{
    return getCurrentDriver()->getCurrentDateMonth().daysInMonth();
}

bool Month::IsTruancy()
{
    DayValue* nowDV = nowDayValue();
    if(nowDV == NULL)   return false;
    return nowDV->state == stateTruancy;
}

bool Month::IsReplaced()
{
    DayValue* nowDV = nowDayValue();
    if(nowDV == NULL)   return false;
    return nowDV->id_driver_replaced > 0;
}

bool Month::IsWork()
{
    DayValue* dv = nowDayValue();
    if(dv == NULL)   return false;

    if(dv->hours != h24) return false;
    if(dv->hours != h16) return false;                  // добавлено 14.08
    if(dv->hours != h12d) return false;
    if(dv->hours != h12n) return false;
    return true;
}

bool Month::IsWork(quint8 day)
{    
    DayValue* dv = getDayValue(day);
    if(dv == NULL)   return false;

    /*QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();*/

    if(dv->id_driver_replaceable > 0) return true;
    if(dv->id_driver_replaced > 0) return false;
    if(dv->hours != h24) return false;
    if(dv->hours != h16) return false;                  // добавлено 14.08
    if(dv->hours != h12d) return false;
    if(dv->hours != h12n) return false;
    /*{
        QDateTime dtCurrent(date, time);
        QDateTime dtChangeDriver(date, *m_timeChangeDriver);
        if(nowDV->hours == h12d &&
            dtCurrent >= dtChangeDriver &&
            dtCurrent < dtChangeDriver.addSecs(3600 * 12))
            return true;
        else if(nowDV->hours == h12n &&
                !(dtCurrent >= dtChangeDriver &&
                dtCurrent < dtChangeDriver.addSecs(3600 * 12)))
            return true;
    }*/
    return true;
}

DayValue* Month::nowDayValue()
{
    QDate date = QDate::currentDate();
    QDate cdate = getCurrentDriver()->getCurrentDateMonth();
    if(date.year() != cdate.year() ||
       date.month() != cdate.month())
       return NULL;

    if(!m_schedule.contains(date.day()))
        return NULL;

    return m_schedule.value(date.day());
}

int Month::countTruancy()
{
    int count = 0;
    foreach (DayValue* dv, m_schedule.values())
    {
        if(dv->state == stateTruancy)
            ++count;
    }
    return count;
}

void Month::compare(Month * umonth)
{
    if(umonth == NULL || m_id_driver != umonth->m_id_driver)
        return;

    foreach (quint8 day, umonth->m_schedule.keys())
    {
        DayValue *dv = NULL;
        if(m_schedule.contains(day))
        {
            dv = m_schedule[day];
            if(!dv->compare(umonth->m_schedule[day]))
                dv = NULL;
        }
        else
        {
            dv = umonth->m_schedule[day]->clone(this);
            m_schedule.insert(day, dv);
        }
        if(dv != NULL)  emit dataChanged(day, dv);
    }
}

void Month::setResponse(QByteArray /*response*/, Request::Command cmd)
{
    switch (cmd) {
    default:
        break;
    }
}

void Month::clear()
{
    foreach (quint8 day, m_schedule.keys()) {
//        m_schedule[day]->deleteLater();
        delete m_schedule[day];
    }
    m_schedule.clear();

    if(lastDayPreviousMonth != NULL)
    {
        lastDayPreviousMonth->deleteLater();
        lastDayPreviousMonth = NULL;
    }

    if(firstDayNextMonth != NULL)
    {
        firstDayNextMonth->deleteLater();
        firstDayNextMonth = NULL;
    }
}
