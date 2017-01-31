#ifndef QUEUEREQUESTS_H
#define QUEUEREQUESTS_H

#include <QObject>
#include "model/request/request.h"

class QueueRequests : public QObject
{
    Q_OBJECT
public:
    enum Priority
    {
        High,
        Low
    };

private:
    Request * m_request;
    bool isFreeRequest = false;
    QList<QueueItem *> m_requests;
    void startRequest();
    bool is_connected = false;
    QList<QueueItem *> lastRequests;

    void addQueueItem(QueueItem * item);
    void addLastQueueItems();
    void undoLastQueueItems();

public:
    explicit QueueRequests(QObject *parent = 0);
    void connectRequest();
    void add(IRequest * obj, const QString & query, Request::Command cmd, QVariant lastData = QVariant(), QueueRequests::Priority priority = QueueRequests::High);
    int stopAutoUpdateRequests();
    void clear();
    void undo();
    void save();
    bool isFinished();

signals:
    void newUserRequest();
    void undoSetEnabled(bool value);
    void finished();

private slots:
    void nextRequest(QueueItem * obj);
    void queryObjRemove();

public slots:

};

#endif // QUEUEREQUESTS_H
