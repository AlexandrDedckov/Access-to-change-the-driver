#ifndef REQUEST_H
#define REQUEST_H

#include <QThread>
#include <QtNetwork>
#include <QtCore>
#include "model/request/crypto.h"

class IRequest;
class QueueItem;

class Request : public QThread
{
    Q_OBJECT
public:
    enum Command
    {
        NoSet = 0,
        AutoParksSelectIds,
        AutoParksSelectNames,
        AutoDayNextMonth,
        MonthSetDayValue,
        MonthSetDriverReplaced,
        MonthSetDriverReplaceable,
        MonthSetDriverNullReplaceable,
        DriversSelect,
        DriversSpoofingSelect,
        InsertDriverSpoofingSelect,
        DriversSelectData,
        DriversSelectMonth,
        DriversSelectNodes,
        DriverSelectMonth,
        DriverAttachCar,
        DriverRemoveCar,
        DriverSetSchedule,
        DriverSetWork,
        DriverSetNotWork,
        DriverAddNode,
        DriverEditNode,
        DriverRemoveNode,
        DriverSetState,
        CarsSelect,
        CarSetTimeChangeDriver,
        DriverSetNotResponding,
        DriverTimeToSleep,
        DriverIntercession,
        SequenceDriversOnCar,
        StartProgram,
        TEST
    };

private:
    const QString api_general;
    const QString api_secondary;
    const QString api_name = "autoplan";
    const QString api_pass = "Fghge325FgheJ";
    const quint8 count_repeat = 3;
    QString m_query;
    Crypto m_crypto;
    quint8 now_index = -1;
    quint8 now_index_count = -1;
    bool isRun = true;
//    bool isRunTest = false;
    bool isErr = false;

    void run();
    void parseError(const QByteArray & resp, QString & err);
	void setResponse(const QByteArray & data);

    QByteArray m_response = "";
    Request::Command m_cmd = Request::NoSet;
    QueueItem * m_currentObj = NULL;
    bool is_finished = true;


    bool server_1 = true;
    bool server_2 = false;
    bool server_3 = false;
public:
    explicit Request(QObject *parent = 0);
    void setData(QueueItem * item);
    //void setData(IRequest * obj, const QString & query, Request::Command cmd);
    bool IsFinished()   { return is_finished; }
    void md5(const QByteArray & data, QByteArray & output);

signals:
    void error(QString err);
    void finishedRequest(QueueItem * obj);
    void sendUpdate();

private slots:
    void queryObjRemove();
    void sFinished();
    void readyRead();
};

class QueueItem : public QObject
{
    Q_OBJECT
public:
    explicit QueueItem(QObject *parent = 0):
        QObject(parent) {}
    IRequest * obj;
    QString query;
    Request::Command cmd;
    int priority;
    QVariant lastData;
};

#include "model/irequest.h"

#endif // REQUEST_H
