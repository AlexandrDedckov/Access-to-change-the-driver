#ifndef CARS_H
#define CARS_H

#include "model/car.h"
#include "model/irequest.h"
#include <QList>

bool compareCar(Car * car1, Car * car2);

class Cars : public IRequest
{
    Q_OBJECT
private:
    QList<Car *> m_cars;
    QList<Car *> removeCars;
    int m_id_autopark = -1;
    bool is_high = false;
    QDate m_currentMonth;
    QQueue<int> m_int_autopark;
    QList <Car*> listRemoveMemory;

public:
    explicit Cars(QObject *parent = 0);
    void setAutoPark(QList<int> ids_autopark);
    void add(Car * car);
    Car * contains(Car * a_car)    const;
    Car * contains(quint64 id)   const;
    Car * containsDriver(Driver * driver, Car * start_car = NULL)   const;
    void compare(Cars * cars, QList<Car *> & outcars);
    void getCars(QList<Car *> & a_cars);
    int count()    const;
    void setResponse(QByteArray response, Request::Command cmd);
    void clear();
    void RefreshDriversState()              { emit refreshDriversState();           }
    void sort();
    void startedUpdateCars();
    void finishedUpdateCars();

    void startDownloadCars();
    void endDownloadCars();
    ~Cars();

private:
    void remove(Car * car);

signals:
    void complete();
    void refreshDriversState();
    void completeCars();
    void removeCar(Car *car);

public slots:
    void Select(bool is_high = true);
};

#endif // CARS_H
