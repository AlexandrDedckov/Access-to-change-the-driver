#include "drivers.h"
#include "mainwindow.h"
#include "model/request/jsonparser.h"


Drivers::Drivers(bool start_timer, QObject *parent) :
    IRequest(parent)
{
    if(start_timer)
    {
        timerWork = new QTimer(this);
        connect(timerWork, &QTimer::timeout,
                this, [this]() {
            foreach (Driver * driver, m_drivers) {
                if(driver->getCar() != NULL &&
                   driver->getIdCar() != 0)
                    driver->refreshState();
            }
        });
        timerWork->start(5*60*1000);
    }

    connect (this, SIGNAL(complete()), this, SLOT(Select()));
    connect (this, SIGNAL(completeSelectData()), this, SLOT(slotSelectMonth()));
}

void Drivers::setAutoPark(QList<int> ids_autopark)
{
    for (int id : ids_autopark)
        m_int_autopark.enqueue(id);
    startDownloadDrivers();
}

Driver * Drivers::contains(Driver * a_driver)   const
{
    foreach (Driver * driver, m_drivers)
    {
        if(driver->toString() == a_driver->toString())
            return driver;
    }
    return NULL;
}

Driver * Drivers::contains(quint64 id)   const
{
    foreach (Driver * driver, m_drivers)
    {
        if(driver->getId() == id)
            return driver;
    }
    return NULL;
}

void Drivers::add(Driver * driver)
{
    if(m_drivers.count() == 0)
    {
        m_drivers.append(driver);
        return;
    }
    for(int i = 0; i < m_drivers.count(); i++)
    {
        if(m_drivers[i]->getFIO().compare(driver->getFIO(), Qt::CaseInsensitive) > 0)
        {
            m_drivers.insert(i, driver);
            return;
        }
    }

    m_drivers.append(driver);
}

void Drivers::remove(Driver *driver)
{
    qDebug() << "[REMOVE DRIVER]" << driver->getFIO();
    m_drivers.removeAll(driver);
    driver->getMonth()->clear();
    delete driver;
}

void Drivers::getDrivers(QList<Driver *> & a_drivers)
{
    a_drivers = m_drivers;
}

void Drivers::getDrivers(Driver::TypeGroupDrivers type, QList<Driver *> & a_drivers)
{
    a_drivers.clear();
    foreach (Driver * driver, m_drivers)
    {
        if(driver->getState() == type && driver->getIdCar() == 0)
            a_drivers.append(driver);
    }
}

void Drivers::getAttachedDrivers(QList<Driver *> & a_drivers)
{
    a_drivers.clear();
    foreach(Driver * driver, m_drivers)
    {
        if(driver->getIdCar() == 0 || driver->getCar() == NULL)
            continue;
        a_drivers.append(driver);
    }
}

int Drivers::count()    const
{
    return m_drivers.count();
}

void Drivers::setResponse(QByteArray response, Request::Command cmd)
{
    switch (cmd)
    {
    case Request::DriversSelect:
    {
        QJsonObject root;
        if(!JsonParser::getObject(response, root))  return;
        QJsonArray array = root.value("array").toArray();
        foreach (const QJsonValue & item , array)
        {
            if (!item.isObject())
                continue;
            QJsonObject obj = item.toObject();
            quint64 id = obj.value("id").toString().toULongLong();
            //if(contains(id) != NULL)    continue;
            Driver * driver = this->contains(id);
            bool stateNew = false;
            if (!driver)
            {
                driver = new Driver(m_tab_index, this);
                stateNew = true;
            }

            if (removeDrivers.count())
                removeDrivers.removeAll(driver);

            driver->setAutoPark(m_id_autopark);
            driver->setId(id);
            driver->setIdCar(obj.value("id_auto").toString().toULongLong());
            driver->setFIO(obj.value("driver_family").toString());
            driver->setPhone(obj.value("phone").toString());

//            qDebug() << driver->getAutoPark() << driver->getId() << driver->getFIO();

            if (stateNew)
            {
                connect(driver, &Driver::dataChanged,
                        this, [this](Driver::TypePropery type, QVariant t_data)
                {
                    Driver * driver = qobject_cast<Driver *>(sender());
                    if(driver == NULL)
                        return;
                    QList<QVariant> list;
                    list.append((quint64)driver);
                    list.append(t_data);
                    emit dataChanged(type, list);
                });
                add(driver);
            }
        }
        SelectSpoofing();
    }
        break;
//--------------------------------------------------------------------------------------
    case Request::DriversSpoofingSelect:
    {
        QJsonObject root;
        if(!JsonParser::getObject(response, root))  return;
        QJsonArray array = root.value("array").toArray();
        foreach (const QJsonValue & item , array)
        {
            if (!item.isObject()) continue;
            QJsonObject obj = item.toObject();
            quint64 id = obj.value("ID").toString().toULongLong();
            //if(contains(id) != NULL)    continue;
            Driver * driver = this->contains(id);
            bool stateNew = false;
            if (!driver)
            {
                driver = new Driver(m_tab_index, this);
                stateNew = true;
            }

            if (removeDrivers.count())
                removeDrivers.removeAll(driver);

            driver->setAutoPark(m_id_autopark);
            driver->setId(id);
            driver->setIdCar(obj.value("id_auto").toString().toULongLong());
            driver->setFIO(obj.value("driver_family").toString());
            driver->setPhone(obj.value("phone").toString());

//            qDebug() << driver->getAutoPark() << driver->getId() << driver->getFIO();

            if (stateNew)
            {
                connect(driver, &Driver::dataChanged,
                        this, [this](Driver::TypePropery type, QVariant t_data)
                {
                    Driver * driver = qobject_cast<Driver *>(sender());
                    if(driver == NULL)  return;
                    QList<QVariant> list;
                    list.append((quint64)driver);
                    list.append(t_data);
                    emit dataChanged(type, list);
                });
                add(driver);
            }
        }

        emit complete();
    }
        break;
    case Request::DriversSelectData:
    {
        QJsonObject root;
        if(!JsonParser::getObject(response, root))  return;
        QJsonArray array = root.value("array").toArray();
        foreach (const QJsonValue & item , array)
        {
            if (!item.isObject())
                continue;
            QJsonObject obj = item.toObject();
            Driver * driver = contains(obj.value("id").toString().toULongLong());
            if(driver == NULL)
                continue;
            driver->setSchedule(obj.value("schedule").toString(), false);
            driver->setState((Driver::TypeGroupDrivers)obj.value("state").toString().toInt());
            bool t_isWork = obj.value("is_work").toString() != "0" && !obj.value("is_work").toString().isEmpty();
            if(t_isWork)
                driver->setWork(t_isWork, false);
            else
                driver->setIsWork(t_isWork);
            driver->setNotResponding(obj.value("not_responding").toString());
            driver->setSequence(obj.value("sequence").toString());
        }
        emit completeSelectData();
        emit signalSortSequenceDrivers();
    }
        break;
    case Request::DriversSelectMonth:
    {
        QJsonObject root;
        if(!JsonParser::getObject(response, root))
            return;
        QDate endDate = selectedDate;
        endDate.setDate(selectedDate.year(), selectedDate.month(), selectedDate.daysInMonth());
        QDate firstDate = selectedDate;
        firstDate.setDate(selectedDate.year(), selectedDate.month(), 1);
        QJsonArray array = root.value("array").toArray();
        foreach (const QJsonValue & item , array)
        {
            if (!item.isObject())
                continue;
            QJsonObject obj = item.toObject();
            Driver * driver = contains(obj.value("id_driver").toString().toULongLong());
            if(driver == NULL)
                continue;

            Month * month = driver->getMonth();
//            month->clear();

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
        setDriversReplace();
        SelectData();
    }
        break;
    default:
        break;
    };
}

void Drivers::setDriversReplace()
{
    foreach (Driver * driver1, m_drivers)
    {
        QMap<quint8, DayValue *> * schedule = driver1->getMonth()->getSchedule();
        foreach (quint8 day, schedule->keys())
        {
            DayValue * dv = schedule->value(day);
            if(dv->id_driver_replaceable > 0)
            {
                Driver * driver2 = contains(dv->id_driver_replaceable);
                dv->driver_replaceable = driver2;
            }
            if(dv->id_driver_replaced > 0)
            {
                Driver * driver2 = contains(dv->id_driver_replaced);
                dv->driver_replaced = driver2;
            }
        }
    }
}

void Drivers::clear()
{
    while(m_drivers.count() > 0)
    {
        Driver * dr = m_drivers.first();
        m_drivers.removeAll(dr);
        dr->getMonth()->clear();
        delete dr;
    }
}

void Drivers::Select(bool is_high)
{
    if (m_int_autopark.isEmpty())
    {
        endDownloadDrivers();
        qDebug() << "[DRIVERS COUNT]" << m_drivers.count();
        emit completeDrivers();
        return;
    }

    m_id_autopark = m_int_autopark.dequeue();
    QueueRequests * queueRequests = MainWindow::getQueue();
    queueRequests->add(this, "type_query=get_drivers_data&our=0&park=" +
                     QString::number(m_id_autopark),
                     Request::DriversSelect,
                       is_high ? QueueRequests::High : QueueRequests::Low);
}

void Drivers::SelectSpoofing(bool is_high)
{
    QueueRequests * queueRequests = MainWindow::getQueue();
    queueRequests->add(this, "type_query=select_substitution&our=0&id_park=" +
                       QString::number(m_id_autopark),
                       Request::DriversSpoofingSelect,
                       is_high ? QueueRequests::High : QueueRequests::Low);
}

void Drivers::SelectData(bool is_high)
{
    if(m_drivers.count() == 0)
        return;

    QByteArray hash;
    Request * req = MainWindow::getRequest();
    QString id_park = QString::number(m_id_autopark);
    req->md5(id_park.toUtf8(), hash);

    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=drivers_select_data&id_park=" + id_park + "&hash=" + hash,
               Request::DriversSelectData,
               is_high ? QueueRequests::High : QueueRequests::Low);
}

void Drivers::SelectMonth(const QDate &a_date, bool is_high)
{
    if(m_drivers.count() == 0)
        return;

    if (m_int_autopark.isEmpty())
    {
        emit completeSelectDataMonth();
        return;
    }

    m_id_autopark = m_int_autopark.dequeue();

    selectedDate = QDate::currentDate();
    if(a_date.isValid())
        selectedDate = a_date;

    QByteArray hash;
    Request * req = MainWindow::getRequest();
    QString id_park = QString::number(m_id_autopark);
    QString date = QString::number(QDateTime(selectedDate).toTime_t());
    req->md5((id_park+date).toUtf8(), hash);

    QueueRequests * queue = MainWindow::getQueue();
    queue->add(this, "type_query=drivers_select_month&id_park=" +
               id_park + "&date=" + date + "&hash=" + hash,
               Request::DriversSelectMonth,
               is_high ? QueueRequests::High : QueueRequests::Low);
}

void Drivers::setCars(Cars * cars, const QDate &date,  bool is_high)
{
    setCarsDate = date;
    cars->startedUpdateCars();
    foreach (Driver * driver, m_drivers)
    {
        driver->setCurrentDateMonth2(date);
        Car * car = cars->contains(driver->getIdCar());

        if(car == NULL)
        {
            driver->setState(Driver::DriversUndefinedSchedule);
            continue;
        }
        car->setCurrentDateMonth(date);
        car->addDriver(driver);
    }
    cars->finishedUpdateCars();

    SelectMonth(date, is_high);
}

void Drivers::compare(Drivers * udrivers)
{
    bool state = false;
    QList<Driver *> t_udrivers;
    udrivers->getDrivers(t_udrivers);

    QList<Driver *> removeDrivers = m_drivers;

    foreach (Driver * udriver, t_udrivers)
    {
        state = false;
        foreach (Driver * driver, m_drivers)
        {
            if(driver->getId() == udriver->getId())
            {
                driver->compare(udriver);
                removeDrivers.removeAll(driver);
                state = true;
                break;
            }
        }
        if(!state)
        {
            Driver *driver = udriver->clone(this);
            add(driver);
        }
    }

    for (Driver * driver : removeDrivers)
        remove(driver);

    setDriversReplace();
}

Drivers::~Drivers()
{
    emit objRemove();
}

void Drivers::startDownloadDrivers()
{
    // для удаления неришедших
    removeDrivers = m_drivers;
}

void Drivers::endDownloadDrivers()
{
    for (Driver * driver : removeDrivers)
        remove(driver);
    removeDrivers.clear();
}

void Drivers::slotSelectMonth()
{
    SelectMonth(setCarsDate);
}
