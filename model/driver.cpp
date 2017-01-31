#include "car.h"
#include "mainwindow.h"
#include <QDebug>
#include "model/request/jsonparser.h"

quint8 Driver::getCountTruancy()
{
    QMap<quint8, DayValue *> * schedule = m_month->getSchedule();
    quint8 count = 0;
    foreach (DayValue * dv, *schedule)
        if(dv->state == stateTruancy) count++;

    return count;
}

bool Driver::IsWork(quint8 day)
{
    if(m_currentCar != NULL && m_currentCar->IsSetWork())
        return isWork;
    else
        return m_month->IsWork(day);
}

bool Driver::IsWork()
{
    if(m_currentCar != NULL && m_currentCar->IsSetWork())
        return isWork;
    else
        return m_month->IsWork();
}

void Driver::setFIO(const QString & fio)
{
    this->m_fio = fio;
    emit dataChanged(Driver::ProperyFIO, fio);
}

void Driver::setPhone(const QString & phone)
{
    this->m_phone = phone;
    emit dataChanged(Driver::ProperyPhone, phone);
}

void Driver::setState(Driver::TypeGroupDrivers type)
{
    this->m_type = type;
    emit dataChanged(Driver::ProperyTypeGroup, m_type);
    return;
}

void Driver::setSchedule(const QString & schedule, bool is_send)
{
    if(!is_send)
    {
        this->m_schedule = schedule;
        emit dataChanged(Driver::ProperySchedule, m_schedule);
        return;
    }

    QByteArray hash;
    Request * req = MainWindow::getRequest();
    QString id_park = QString::number(m_id_park);
    req->md5((QString::number(m_id) + id_park + schedule).toUtf8(), hash);

    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=driver_set_schedule&id=" + QString::number(m_id) +
                     "&id_park=" + id_park +
                     "&schedule=" + QUrl::toPercentEncoding(schedule) +
                     "&hash=" + hash,
               Request::DriverSetSchedule,
               m_schedule);

    this->m_schedule = schedule;
    emit dataChanged(Driver::ProperySchedule, m_schedule);
}

// запись данных в переменную и отправка запроса на php сервер не на связи
void Driver::setNotResponding(const QString & notResponding)
{
    m_NotResponding = notResponding;
    emit dataChanged(Driver::ProperyNotResponding, notResponding);
}

void Driver::setSequence(QString sequence)
{
    m_Sequence = sequence;
    emit dataChanged(Driver::ProperySequence, sequence);
}

void Driver::setSleep(const QString & sleep)
{
    this->m_Sleep = sleep;
    emit dataChanged(Driver::ProperySleep, sleep);
}

void Driver::setIntercession(const QString &comment)
{
    m_Intercession = comment;
    emit dataChanged(Driver::ProperyIntercession, comment);
}

void Driver::setAutoDayNextMonth()
{
    QByteArray hash;
    Request * req = MainWindow::getRequest();
    QString schedule = NULL;
    if (m_schedule == TXT_SCHEDULE_1_1)
        schedule = "2";
    else if ((m_schedule == TXT_SCHEDULE_1_2) || (m_schedule == TXT_SCHEDULE_2_1))
        schedule = "3";
    else if ((m_schedule == TXT_SCHEDULE_1_3) || (m_schedule == TXT_SCHEDULE_2_2)  || (m_schedule == TXT_SCHEDULE_3_1))
        schedule = "4";
    else if (m_schedule == TXT_SCHEDULE_3_3)
        schedule = "6";
    else if ((m_schedule == TXT_SCHEDULE_5_2) || (m_schedule == TXT_SCHEDULE_6_1)
             || (m_schedule == TXT_SCHEDULE_12d) || (m_schedule == TXT_SCHEDULE_12n))
        schedule = "7";
    QString id_park = QString::number(m_id_park);
    QString newYear;
    QString newMonth;
    if (m_id_car != 0)
    {
        auto date = m_currentDateMonth->addMonths(1);
         newYear = QString::number(date.year());
         newMonth = QString::number(date.month());

         _year = QString::number(m_currentDateMonth->year());
         _month = QString::number(m_currentDateMonth->month());
    }
    else
    {
        auto date = m_currentDateMonth2.addMonths(1);
        newYear = QString::number(date.year());
        newMonth = QString::number(date.month());

        _year = QString::number(m_currentDateMonth2.year());
        _month = QString::number(m_currentDateMonth2.month());
    }

    req->md5((QString::number(m_id) + id_park + _year + _month + newYear + newMonth + schedule).toUtf8(), hash);

    QueueRequests * queue = MainWindow::getQueue();
    QString query = QString("type_query=auto_day_next_month&id=") +
            QString::number(m_id) +
            "&id_park=" + id_park +
            "&year=" + _year +
            "&month=" + _month +
            "&newYear=" + newYear +
            "&newMonth=" + newMonth +
            "&schedule=" + schedule +
            "&hash=" + hash;
    qDebug() << query << endl;
    queue->add(this, query, Request::AutoDayNextMonth);
}

void Driver::setCar(Car * car, bool is_send)
{
    Car * lastCar = m_currentCar;
    this->m_currentCar = car;
    this->isWork = false;
    if(car == NULL)
    {
        if(!is_send)
        {
            this->m_id_car = 0;
            return;
        }
    }
    this->m_id_car = car->getId();
}

void Driver::attachCar(Car * car)
{
    if(car == NULL)
        setCar(car, false);
    else
    {
        QByteArray t_data = (QString::number(car->getId() - 1) + QString::number(m_id - 2)).toUtf8();
        QByteArray hash;
        Request * req = MainWindow::getRequest();
        req->md5(t_data, hash);

        QueueRequests * queue = MainWindow::getQueue();
        queue->add(this, "type_query=attach_driver_to_auto&auto_id=" +
                   QString::number(car->getId()) + "&driver_id=" + QString::number(m_id) + "&hash=" + hash,
                   Request::DriverAttachCar,
                   (quint64)m_currentCar);

        this->m_currentCar = car;
        this->isWork = false;
        this->m_id_car = car->getId();
        /*m_type = Driver::DriversNotSet;
        emit dataChanged(Driver::ProperyTypeGroup, m_type);*/
    }
}

void Driver::setResponse(QByteArray response, Request::Command cmd)
{
    switch (cmd) {
    case Request::DriverSelectMonth:
    {
        QJsonObject root;
        if(!JsonParser::getObject(response, root))  return;
        QDate t_carDate = m_currentCar->getCurrentDateMonth();
        QDate endDate = t_carDate; endDate.setDate(t_carDate.year(), t_carDate.month(), t_carDate.daysInMonth());
        QDate firstDate = t_carDate; firstDate.setDate(t_carDate.year(), t_carDate.month(), 1);
        QJsonArray array = root.value("array").toArray();
        Month * month = this->getMonth();
        month->clear();
        foreach (const QJsonValue & item , array)
        {
            if (!item.isObject()) continue;
            QJsonObject obj = item.toObject();
            QDate t_date = QDate::fromString(obj.value("day_work").toString(),"yyyy-MM-dd");
            if(endDate < t_date)
            {
                month->setFirstDayNextMonth(t_date,
                                            (Hours)obj.value("hours_enum").toString().toUInt(),
                                            (StateDriver)obj.value("attendance").toString().toUInt(),
                                            obj.value("node").toString(),
                                            obj.value("notify").toString() == "1",
                                            obj.value("id_driver_replaced").toString().toULongLong(),
                                            obj.value("id_driver_replaceable").toString().toULongLong());
            }
            else if(firstDate > t_date)
            {
                month->setLastDayPreviousMonth(t_date,
                                               (Hours)obj.value("hours_enum").toString().toUInt(),
                                               (StateDriver)obj.value("attendance").toString().toUInt(),
                                               obj.value("node").toString(),
                                               obj.value("notify").toString() == "1",
                                               obj.value("id_driver_replaced").toString().toULongLong(),
                                               obj.value("id_driver_replaceable").toString().toULongLong());
            }
            else
            {
                month->addScheduleValue(t_date.day(),
                                    (Hours)obj.value("hours_enum").toString().toUInt(),
                                    (StateDriver)obj.value("attendance").toString().toUInt(),
                                    obj.value("node").toString(),
                                    obj.value("notify").toString() == "1",
                                    obj.value("id_driver_replaced").toString().toULongLong(),
                                    obj.value("id_driver_replaceable").toString().toULongLong(),
                                    obj.value("id").toString().toULongLong());
            }
        }
    }
        break;

    default:
        break;
    }
}

void Driver::SelectMonth()
{
    if(m_currentCar == NULL)
        return;

    QByteArray hash;
    Request * req = MainWindow::getRequest();
    QString date = QString::number(QDateTime(m_currentCar->getCurrentDateMonth()).toTime_t());
    QString idPark = QString::number(this->getAutoPark());
    req->md5((QString::number(m_id)+date+idPark).toUtf8(), hash);

    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=driver_select_month&id=" +
               QString::number(m_id) + "&id_park="+ idPark + "&date=" + date + "&hash=" + hash,
               Request::DriverSelectMonth);
}

void Driver::compare(Driver * driver)
{
    if(driver == NULL || m_id != driver->m_id) return;

    if(m_fio != driver->m_fio)
        setFIO(driver->m_fio);
    if(m_phone != driver->m_phone)
        setPhone(driver->m_phone);
    if(m_schedule != driver->m_schedule)
        setSchedule(driver->m_schedule, false);
    if(driver->isWork != isWork)
    {
        if(driver->isWork)
            setWork(driver->isWork, false);
        else
            setIsWork(driver->isWork);
    }
    if(m_id_car != driver->m_id_car)
    {
        if(driver->m_id_car == 0)
        {
            if(m_currentCar == NULL) return;
            m_currentCar->removeDriver(this);
        }
        else
        {
            QList<Car *> cars;
//            MainWindow::getTable(m_tab_index)->getCars()->getCars(cars);
            foreach (Car * car, cars)
                if(driver->m_id_car == car->getId())
                {
                    car->addDriver(this);
                    break;
                }
            if (m_currentCar != NULL)
                m_currentCar->emitRefresh(this);
        }
    }
    m_month->compare(driver->m_month);

    if(m_type != driver->m_type)
        setState(driver->m_type);
}


void Driver::setWork(bool value, bool is_send)
{
    bool lastIsWork = isWork;
    isWork = value;
    if(m_currentCar == NULL)    return;

    //m_currentCar->IsSetDriverWork = value;
    QList<Driver *> drivers;
    m_currentCar->getDivers(drivers);

    foreach(Driver * driver, drivers)
    {
        if(driver->isWork && driver != this)
            driver->isWork = false;
    }

    emit dataChanged(Driver::ProperyState, isWork);

    if(!is_send)    return;

    if(!isWork)
    {
        QByteArray hash;
        Request * req = MainWindow::getRequest();
        req->md5(QString::number(m_id).toUtf8(), hash);

        QueueRequests * queue = MainWindow::getQueue();
        queue->add(this, "type_query=driver_set_not_work&id=" + QString::number(m_id) +
                         "&hash=" + hash,
                   Request::DriverSetNotWork,
                   lastIsWork);
    }
    else
    {
        if(m_currentCar == NULL)    return;
        QList<Driver *> drivers;
        m_currentCar->getDivers(drivers);
        QString ids = "";
        QString id_park = QString::number(m_id_park);
        QString query = "type_query=driver_set_work&id_work=" + QString::number(m_id) + "&id_park=" + id_park;
        foreach (Driver * driver, drivers) {
            if(driver == this)  continue;
            ids += QString::number(driver->m_id);
            query += "&ids[]=" + QString::number(driver->m_id);
        }
        if(drivers.count() == 1)
        {
            ids = "0";
            query += "&ids[]=0";
        }

        QByteArray hash;
        Request * req = MainWindow::getRequest();
        req->md5((QString::number(m_id)+id_park+ids).toUtf8(), hash);

        QueueRequests * queue = MainWindow::getQueue();
        queue->add(this, query + "&hash=" + hash,
                   Request::DriverSetWork,
                   lastIsWork);
    }
}


Driver * Driver::clone(QObject *parent)
{
    Driver * driver = new Driver(m_tab_index, parent);

    driver->setFIO(m_fio);
    driver->setPhone(m_phone);
    driver->setSchedule(m_schedule, false);
    if(isWork)
        driver->setWork(isWork, false);
    else
        driver->setIsWork(isWork);

    if(m_id_car != 0)
    {
        QList<Car *> cars;
//        MainWindow::getTable(m_tab_index)->getCars()->getCars(cars);
        foreach (Car * car, cars)
        {
            if(driver->m_id_car == car->getId())
            {
                car->addDriver(driver);
                break;
            }
        }
    }

    driver->m_month->compare(m_month);
    driver->setState(m_type);

    return driver;
}
