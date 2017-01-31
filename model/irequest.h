#ifndef IREQUEST_H
#define IREQUEST_H

#include <QObject>
#include "model/request/request.h"
#include <QJsonDocument>

class IRequest : public QObject
{
    Q_OBJECT
public:
    explicit IRequest(QObject *parent = 0) : QObject(parent) { }
    virtual void setResponse(QByteArray, Request::Command) { }
    virtual void Update() { }
    virtual void undo(Request::Command, QVariant) { }

signals:
    void objRemove();
    void completed(Request::Command cmd);
    void error(QString err);
};

#endif // IREQUEST_H
