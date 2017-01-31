#include "car.h"
#include "model/request/jsonparser.h"
#include "mainwindow.h"

void Car::setName(const QString & name)
{
    this->m_name = name;
    emit Car::dataChanged(ProperyName, name);
}

void Car::setNumber(const QString & number)
{
    this->m_number = number;
    emit Car::dataChanged(ProperyNumber, number);
}

void Car::setTimeChangeDriver(const QTime & time)
{
    if(!time.isValid())
        return;
    m_timeChangeDriver = time;
}

void Car::setRepair(bool repair)
{
    isRepair = repair;
    emit Car::dataChanged(ProperyRepair, repair);
}

void Car::setLease(bool lease)
{
    isLease = lease;
    emit Car::dataChanged(ProperyLease, lease);
}

bool Car::IsSetWork()
{
    foreach (Driver * driver, m_drivers) {
        if(driver->IsSetWork())  return true;
    }
    return false;
}

// метод для подсчёта последнего значения в списке последовательности водителей на машине
int Car::lastValue()
{
    QList <Driver *> sortDrivers;
    sortDrivers = sort(m_drivers);
    int lastValue = sortDrivers.last()->getSequence().toInt() + 1;
    return lastValue;
}

void Car::compare(Car * car)
{
    if(car == NULL || m_id != car->m_id) return;

    if(m_name != car->m_name)
        setName(car->m_name);
    if(m_number != car->m_number)
        setNumber(car->m_number);
    if(m_timeChangeDriver != car->m_timeChangeDriver)
        setTimeChangeDriver(car->m_timeChangeDriver);
    if(isRepair != car->isRepair)
        setRepair(car->isRepair);
    if(isLease != car->isLease) // перезапись значения в состоянии раскат
        setLease(car->isLease);
}

Car * Car::clone(QObject *parent)
{
    Car * car = new Car(parent);
    car->m_currentDateMonth = m_currentDateMonth;
    car->m_id = m_id;
    car->m_name = m_name;
    car->m_number = m_number;
    car->m_timeChangeDriver = m_timeChangeDriver;
    car->isRepair = isRepair;
    car->isLease = isLease;
    //car->IsSetDriverWork = IsSetDriverWork;
    return car;
}

void Car::setResponse(QByteArray /*response*/, Request::Command cmd)
{
    switch (cmd) {
    default:
        break;
    }
}
