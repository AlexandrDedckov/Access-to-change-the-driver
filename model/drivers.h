#ifndef DRIVERS_H
#define DRIVERS_H

#include "model/car.h"
#include "model/cars.h"
#include "model/autoparks.h"
#include <QList>
#include <QTimer>

class Drivers : public IRequest
{
    Q_OBJECT
private:
    QList <Driver *> m_drivers;
    QList <Driver*> removeDrivers;
    int m_id_autopark = -1;
    int m_tab_index = -1;
    QTimer * timerWork;
    QDate selectedDate;
    QDate setCarsDate;
    void setDriversReplace();
    QQueue<int> m_int_autopark;

    bool stateUpdate = false;

    bool stateAutoparksDriver = false;
    bool stateAutoparksDriverInfo = false;
    QList<Driver *> listDrivers;

public:
    explicit Drivers(bool start_timer = false, QObject *parent = 0);
    void setAutoPark(QList<int> ids_autopark);
    Driver * contains(Driver * a_driver)    const;
    Driver * contains(quint64 id)    const;
    void add(Driver * driver);
    void remove(Driver * driver);
    void getDrivers(QList<Driver *> & a_drivers);
    void getDrivers(Driver::TypeGroupDrivers type, QList<Driver *> & a_drivers);
    void getAttachedDrivers(QList<Driver *> & a_drivers);
    int count()    const;
    void SelectSpoofing(bool is_high = true);
    void SelectMonth(const QDate &date, bool is_high = true);
    void setCars(Cars * cars, const QDate &date, bool is_high = true);
    void setResponse(QByteArray response, Request::Command cmd);
    void compare(Drivers * udrivers);
    void clear();
    ~Drivers();

public slots:
    void SelectData(bool is_high = true);
    void Select(bool is_high = true);
    void startDownloadDrivers();
    void endDownloadDrivers();

private slots:
    void slotSelectMonth();

signals:
    void complete();
    void completeDrivers();
    void completeSetCars();
    void completeSelectMonth();
    void completeSelectData();
    void completeSelectDataMonth();
    void dataChanged(Driver::TypePropery type, QVariant data);
    void signalInsertSubstitution(Driver * driver);
    void signalSortSequenceDrivers();

    void signalEndDownload();
    void signalEndDownloadInfo();
    void removeDriver(Driver * driver);

    void signalUpdate(); // сигнал обновления
};

#endif // DRIVERS_H
