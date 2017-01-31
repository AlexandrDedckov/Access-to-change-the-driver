#ifndef AUTOPARKS_H
#define AUTOPARKS_H

#include "model/irequest.h"
#include "model/request/request.h"
#include <QMap>

class AutoParks : public IRequest
{
    Q_OBJECT

private:
    QMap<int,QString> m_autoparks;

    void parseAutoParksSelectIds(const QByteArray & response);
    void parseAutoParksSelectNames(const QByteArray & response);

public:
    explicit AutoParks(QObject *parent = 0);
    QMap<int,QString> * getAutoParks()  { return &m_autoparks; }

    void setResponse(QByteArray response, Request::Command cmd);
    void Select();
    void Update();

signals:
    void getStarted(bool start);

public slots:
    void getStartProgram();
};

#endif // AUTOPARKS_H
