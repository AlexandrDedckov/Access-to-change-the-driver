#include "delivery.h"
#include "mainwindow.h"
#include "model/request/requestautomaticredial.h"

bool compareDrivers(Driver *driver_1, Driver * driver_2)
{
    if (driver_1->getSequence().toInt() <  driver_2->getSequence().toInt())
        return true;
    else
        return false;
}

Delivery::Delivery(Drivers *drivers, Cars *cars, QObject *parent) :
    IRequest(parent)
    , m_drivers(drivers)
    , m_cars(cars)
{
    connect(m_drivers, SIGNAL(completeSelectDataMonth()), this, SLOT(fillingMapTime()));
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this,&Delivery::timerMap);


//    test();
}

void Delivery::setResponse(QByteArray response, Request::Command cmd)
{
    switch (cmd) {
    case Request::TEST:
        qDebug() << response;
        break;
    default:
        break;
    }
}

void Delivery::setAutoPark(AutoParks *autoparks)
{
    m_autopark = autoparks;
}

void Delivery::clear()
{
    queryRequest.clear();
    clearMapTime();
}

void Delivery::startTimerDelivery(bool start)
{
    if (start)
    {
        qDebug() << "[TIMER] start";
        timer->start(5*60*1000); // вызов 1 раз в 5 минут
    }
    else
    {
        qDebug() << "[TIMER] stop";
        timer->stop();
    }
}

int Delivery::timeOut()
{
    QFile file("config.ini");

    if(!file.exists())
        return 3 * 60 * 60;

    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Ошибка открытия для чтения";
        return 3 * 60 * 60;
    }

    QByteArray byteArray = file.readAll();
    file.close();

    if (byteArray.isEmpty())
        return 3 * 60 * 60;

    if (!QString(byteArray).toInt())
        return 3 * 60 * 60;

    return QString(byteArray).toInt() * 60;
}

int Delivery::timeChange(DayValue *dv)
{
    switch (dv->hours)
    {
    case h12d:
        return 12*60*60;
    case h12n:
        return 12*60*60;
    case h16:
        return 16*60*60;
    case h24:
        return 24*60*60;
    default:
        return 0;
    }
}

void Delivery::createMap(bool nowDay)
{
    QList<Car*> cars;
    m_cars->getCars(cars);

    if (nowDay)
        date = QDate::currentDate();
    else
        date = QDate::currentDate().addDays(-1);
    day = date.day();

    for (Car *car : cars)
    {
        QList <Driver*> driversToChangeList;
        QList <Driver*> driversCar;
        QMap  <Driver*, DayValue*> driversToChangeMap;

        car->getDivers(driversCar);
        if (!car->getCurrentDateMonth().isValid())
            continue;

        if (!driversCar.count())
            continue;

        for (Driver *driver : driversCar)
        {
            DayValue* dv = statusDayValue(driver);

            if ((dv == NULL) || (dv->hours == hOther) || (dv->hours == hOutput) || (dv->status > -1))
                continue;

            driversToChangeList.append(driver); // list для сортировки
            driversToChangeMap[driver] = dv;    // map что бы вытянуть время смены
        }

        if (!driversToChangeList.count())
            continue;

        qSort(driversToChangeList.begin(), driversToChangeList.end(), compareDrivers); // сортировка водителей
        QTime time = car->getTimeChangeDriver();
        if (!time.isValid())
            continue;
        bool firstDriver = true;

        QDateTime dateTime;
        dateTime.setDate(date);
        dateTime.setTime(time);

        for (Driver *driver : driversToChangeList)
        {
            DayValue* dv = driversToChangeMap[driver];
            ItemDriverIdDay* item = new ItemDriverIdDay(this);
            listItems.append(item);
            if(dv->driver_replaced != NULL && dv->id_driver_replaced > 0)
                item->driver = dv->driver_replaced;
            else
                item->driver = driver;
            item->id = dv->id;

            if (firstDriver)
            {
                QDateTime dateTimeOut = dateTime.addSecs(-timeOut());
                if (dateTimeOut.date() == QDate::currentDate())
                    mapTime.insert(dateTimeOut.time(), item);
            }
            else
            {
                dateTime = dateTime.addSecs(timeChange(dv));
                time = time.addSecs(timeChange(dv));

                QDateTime dateTimeOut = dateTime.addSecs(-timeOut());
                QTime _timeOut = time.addSecs(-timeOut());

                if (dateTimeOut.date() == QDate::currentDate())
                    mapTime.insert(_timeOut, item);
            }

            firstDriver = false;
        }
    }

/*
#ifdef QT_DEBUG
    QFile file("test.txt");
    for (auto i = mapTime.begin(); i != mapTime.end(); ++i)
    {
        QTime time = i.key();
        if (file.open(QIODevice::Append))
        {
            const QString str = time.toString("hh:mm") + "   " + i.value()->driver->getPhone() + "\n";
            file.write(str.toLatin1());
        }
        file.close();
    }
#endif
*/
}

void Delivery::clearMapTime()
{
    for (ItemDriverIdDay *item : listItems)
    {
        if ((item->id == 0) && (item->driver == NULL))
            continue;
        delete item;
    }
    listItems.clear();
    mapTime.clear();
}

void Delivery::requestItem()
{
    if (isRun)
        return;

    isRun = true;
    qDebug() << "[START] requestItem";
    for (;;)
    {
        if (!queryRequest.count())
            break;

        const QString& strRequest = queryRequest.dequeue();
        RequestAutomaticRedial *request = new RequestAutomaticRedial(this);
        connect(request, &RequestAutomaticRedial::finishThreed, request, &RequestAutomaticRedial::quit);
        connect(request, &RequestAutomaticRedial::finished, request, &RequestAutomaticRedial::deleteLater);
        request->setData(strRequest);
        request->start();
    }
    isRun = false;
}

/*old*/void Delivery::run()
{
    if (isRun)
        return;

    nextItem();
}

/*old*/void Delivery::test()
{
    const QString post = "method=create_call_ivr"
            "&apikey=JDslBakwMzaAZ70sjaGHajqoKf4"
//            "&call_number=79117674404"
            "&call_number=70000000000"
            "&display_number=78123330000"
            "&originate_timeout=40"
            "&ivr_file_start_url=http://127.0.0.1/hold_music.mp3"
            "&ivr_file_end_url=http://127.0.0.1/thanks.mp3"
            "&responce_url=http://192.168.211.30:81/api/api_autoplan_taxi_spb.asp"
            "&id=415"
            "&id_autopark=9";

//    QueueRequests * queue = MainWindow::getQueue();
//    queue->add(this, "type_query=save_radial_status&id=414&status=2&id_autopark=9",
//    queue->add(this, post,
//               Request::TEST,
//               QueueRequests::Low);

    for (auto i = 0; i < 100; ++i)
    {
        RequestAutomaticRedial *request = new RequestAutomaticRedial(this);
        connect(request, &RequestAutomaticRedial::finishThreed, request, &RequestAutomaticRedial::quit);
        connect(request, &RequestAutomaticRedial::finished, request, &RequestAutomaticRedial::deleteLater);
        request->setData(post);
        request->start();
    }
}

/*old*/void Delivery::nextItem()
{
    if (!queryRequest.count())
    {
        isRun = false;
        return;
    }

    qDebug() << "[nextItem]";

    isRun = true;

    const QString strRequest = queryRequest.dequeue();
    RequestAutomaticRedial *request = new RequestAutomaticRedial(this);
    connect(request, &RequestAutomaticRedial::finishThreed, request, &RequestAutomaticRedial::quit);
    connect(request, &RequestAutomaticRedial::finished, request, &RequestAutomaticRedial::deleteLater);
    request->setData(strRequest);
    request->start();
}

void Delivery::completeDownload()
{
    Drivers * drivers = qobject_cast<Drivers *>(QObject::sender());
    if(drivers != NULL)
        completeDrivers = true;

    Cars * cars = qobject_cast<Cars *>(QObject::sender());
    if(cars != NULL)
        completeCars = true;

    if(!completeDrivers || !completeCars)
        return;

    QList <int> list
    {
        0
    };
//    m_drivers->setAutoPark(list);
    m_drivers->setAutoPark(m_autopark->getAutoParks()->keys());
    m_drivers->setCars(m_cars, QDate::currentDate());

    completeDrivers = false;
    completeCars = false;
}

void Delivery::fillingMapTime()
{
    clearMapTime();
    createMap(false);
    createMap(true);
    qDebug() << "[fillingMapTime]" << QTime::currentTime().toString("hh:mm:ss") << mapTime.count();
    timerMap();
}

void Delivery::timerMap()
{
    qDebug() << "[START] timerMap";
    QString templateStr =
            "method=create_call_ivr"
            "&apikey=JDslBakwMzaAZ70sjaGHajqoKf4"
//            "&call_number=79117674404"
            "&call_number=%1"
            "&display_number=78123330000"
            "&originate_timeout=40"
            "&ivr_file_start_url=shout://127.0.0.1/hold_music.mp3"
            "&ivr_file_end_url=shout://127.0.0.1/thanks.mp3"
            "&responce_url=http://192.168.211.30:81/api/api_autoplan_taxi_spb.asp"
            "&id=%2"
            "&id_autopark=%3";

    for (auto itemMap = mapTime.begin(); itemMap != mapTime.end(); ++itemMap)
    {
        QTime time = itemMap.key();
        QTime currentTime = QTime::currentTime();
        QTime currentTimeHourAgo = QTime::currentTime().addSecs(-3600);
        if ((time <= QTime(currentTime.hour(), currentTime.minute()))
                && (time > QTime(currentTimeHourAgo.hour(), currentTimeHourAgo.minute())))
        {
            ItemDriverIdDay *item = itemMap.value();
            if (!item->driver->getPhone().isEmpty())
            {
//                qDebug() << time.toString("hh:mm") << item->driver->getPhone();
                queryRequest.enqueue(templateStr.arg(item->driver->getPhone()).arg(QString::number(item->id)).arg(item->driver->getAutoPark()));
            }
        }
    }

    requestItem(); // vizov
}

DayValue *Delivery::statusDayValue(Driver *driver)
{
    Month* month = driver->getMonth();
    QMap<quint8, DayValue*> * schedule = month->getSchedule();

    if(schedule->count() == 0)
        return NULL;
    if(!schedule->contains(day))
        return NULL;

    DayValue* dv = schedule->value(day);
    return dv;
}
