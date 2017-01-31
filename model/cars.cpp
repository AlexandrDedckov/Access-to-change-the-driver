#include "cars.h"
#include "mainwindow.h"
#include "model/request/jsonparser.h"

Cars::Cars(QObject *parent) :
    IRequest(parent)
{
    connect (this, SIGNAL(complete()), this, SLOT(Select()));
}

void Cars::setAutoPark(QList<int> ids_autopark)
{
    for (int id : ids_autopark)
        m_int_autopark.enqueue(id);
    startDownloadCars();
}

void Cars::add(Car * car)
{
    if(m_cars.count() == 0)
    {
        m_cars.append(car);
        return;
    }

    for(int i = 0; i < m_cars.count(); i++)
    {
        Car * t_car = m_cars[i];
        if(t_car->toString().compare(car->toString(), Qt::CaseInsensitive) > 0)
        {
            m_cars.insert(i, car);
            return;
        }
    }

    m_cars.append(car);
}

Car * Cars::contains(Car * a_car)   const
{
    foreach (Car * car, m_cars)
    {
        if(car->toString() == a_car->toString())
            return car;
    }
    return NULL;
}

Car * Cars::contains(quint64 id)   const
{
    foreach (Car * car, m_cars)
    {
        if(car->getId() == id)
            return car;
    }
    return NULL;
}

Car * Cars::containsDriver(Driver * driver, Car * start_car)   const
{
    foreach (Car * car, m_cars)
    {
        if(start_car != NULL)
        {
           if(car != start_car &&
              car->containsDriver(driver))
              return car;
        }
        else
        {
            if(car->containsDriver(driver))
               return car;
        }
    }

    return NULL;
}

void Cars::getCars(QList<Car *> & a_cars)
{
    a_cars = m_cars;
}

int Cars::count()    const
{
    return m_cars.count();
}

bool compareCar(Car * car1, Car * car2)
{
    QTime time1 = car1->getTimeChangeDriver();
    QTime time2 = car2->getTimeChangeDriver();
    if(time1.isValid())
    {
        if(time2.isValid())
        {
            if(time1 == time2)
                return car2->toString().compare(car1->toString(), Qt::CaseInsensitive) > 0;
            else if(time1 < time2)
                return true;
            else
                return false;
        }
        else
            return true;
    }
    else
        return false;
}

void Cars::setResponse(QByteArray response, Request::Command cmd)
{
    switch (cmd)
    {
    case Request::CarsSelect:
    {
        QJsonObject root;
        if(!JsonParser::getObject(response, root))  return;
        QJsonArray array = root.value("array").toArray();
        foreach (const QJsonValue & item , array)
        {
            if (!item.isObject()) continue;
            QJsonObject obj = item.toObject();
            auto id = obj.value("id").toString().toULongLong();
            Car * car = this->contains(id);
            bool stateNew = false;
            if (!car)
            {
                car = new Car(this);
                stateNew = true;
            }

            if (removeCars.count())
                removeCars.removeAll(car);

            car->setAutoPark(m_id_autopark);
            car->setId(obj.value("id").toString().toULongLong());
            car->setName(obj.value("marka").toString() + " " +
                         obj.value("model").toString());
            car->setNumber(obj.value("number").toString());
            car->setCurrentDateMonth(QDate::currentDate());
            car->setRepair(obj.value("status").toString().toInt() == 31);
            car->setLease(obj.value("auto_raskat").toString().toInt() == 1);
            car->setTimeChangeDriver(QTime::fromString(obj.value("time_change_driver").toString(), "hhmm"));
            //add(car);
            if (stateNew)
                m_cars.append(car);
        }
        sort();
        emit complete();
    }
        break;
    default:
        break;
    };
}

void Cars::sort()
{
    qSort(m_cars.begin(), m_cars.end(), compareCar);
}

void Cars::startedUpdateCars()
{
    for (Car * car : m_cars)
        car->StartUpdateCar();
}

void Cars::finishedUpdateCars()
{
    for (Car * car : m_cars)
        car->FinishUpdateCar();
}

void Cars::clear()
{
    while(m_cars.count() > 0)
    {
        Car * car = m_cars.first();
        m_cars.removeAll(car);
        delete car;
    }
}

void Cars::Select(bool is_high)
{
    if (m_int_autopark.isEmpty())
    {
        endDownloadCars();
        qDebug() << "[CARS COUNT]" << m_cars.count();
        emit completeCars();
        return;
    }

    m_id_autopark = m_int_autopark.dequeue();
    QueueRequests * queueRequests = MainWindow::getQueue();
    queueRequests->add(this, "type_query=get_autos_data&our=0&park=" +
                     QString::number(m_id_autopark),
                     Request::CarsSelect,
                     is_high ? QueueRequests::High : QueueRequests::Low);
    this->is_high = is_high;
}

void Cars::compare(Cars * ucars, QList<Car *> & output)
{
    bool state = false;
    QList<Car *> t_ucars;
    ucars->getCars(t_ucars);
    foreach (Car * ucar, t_ucars)
    {
        state = false;
        foreach (Car * car, m_cars)
        {
            if(car->getId() == ucar->getId())
            {
                car->compare(ucar);
                state = true;
                break;
            }
        }
        if(!state)
        {
            Car *car = ucar->clone(this);
            m_cars.append(car);//add(car);
            output.append(car);
        }
    }
    sort();
}

Cars::~Cars()
{
    emit objRemove();
    /*while(m_cars.count() > 0)
    {
        Car * car = m_cars.first();
        m_cars.removeAll(car);
        delete car;
    }*/
}

void Cars::remove(Car *car)
{
    m_cars.removeAll(car);
    emit removeCar(car);
}

void Cars::startDownloadCars()
{
    while (listRemoveMemory.count())
    {
        delete listRemoveMemory.first();
    }
    listRemoveMemory.clear();
    removeCars = m_cars;
}

void Cars::endDownloadCars()
{
    for (Car * car : removeCars)
    {
        remove(car);
        listRemoveMemory.append(car);
    }
    removeCars.clear();
}
