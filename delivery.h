#ifndef DELIVERY_H
#define DELIVERY_H

#include "model/irequest.h"
#include "model/cars.h"
#include "model/drivers.h"
#include "model/car.h"

class Delivery : public IRequest
{
    Q_OBJECT

private:

    class ItemDriverIdDay : QObject
    {
    public:
        explicit ItemDriverIdDay(QObject *parent = nullptr) :
            QObject(parent) { }
        Driver * driver = NULL;
        int id = 0;
    };

    AutoParks * m_autopark;
    Drivers * m_drivers;
    Cars * m_cars;

    bool completeDrivers = false;
    bool completeCars = false;
    bool isRun = false;

    QMultiMap <QTime, ItemDriverIdDay*> mapTime;
    QList <ItemDriverIdDay*> listItems;
    DayValue* statusDayValue(Driver* driver);
    QQueue <QString> queryRequest;

    quint8 day = 1;
    QDate date;
    QTime time;
    QTimer *timer;

public:
    explicit Delivery(Drivers *drivers, Cars *cars, QObject *parent = nullptr);
    void setResponse(QByteArray response, Request::Command cmd);
    void setAutoPark(AutoParks *autoparks);
    void clear();
    void startTimerDelivery(bool start = false);

private:
    int timeOut();
    int timeChange(DayValue *dv);
    void createMap(bool nowDay);
    void clearMapTime();
    void requestItem();

    void run();
    void test();

signals:
    void newDay();

public slots:
    void completeDownload();

private slots:
    void timerMap();
    void fillingMapTime();
    void nextItem();

};

#endif // DELIVERY_H
