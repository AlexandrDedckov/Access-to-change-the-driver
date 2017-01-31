#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <QMainWindow>
#include "model/request/signalingupdate.h"
#include "model/autoparks.h"
#include "model/drivers.h"
#include "model/cars.h"
#include "model/request/queuerequests.h"
#include "model/request/serverconfig.h"
#include "model/request/jsonparser.h"
#include "delivery.h"

class User;

class MainWindow : public QObject
{
    Q_OBJECT
private:
    AutoParks * m_autoParks = NULL;

    static Request * m_request;
    static QueueRequests * m_queue;
    static MainWindow * current;
    static ServerConfig * m_serverConfig;
    static SignalingUpdate * m_sUpdate;
    static Drivers * m_drivers;
    static Cars * m_cars;
    static Delivery * m_del;

    QTimer * timerUpdate;
    QDate dateStart;

public:
    explicit MainWindow(QObject *parent = 0);
    ~MainWindow();

    static Request * getRequest()                   { return m_request;             }
    static QueueRequests * getQueue()               { return m_queue;               }
    static ServerConfig * getServerConfig()         { return m_serverConfig;        }
    static SignalingUpdate * getSignalingUpdate()   { return m_sUpdate;             }
    static AutoParks * getAutoParks()               { return current->m_autoParks;  }
    static Drivers * getDrivers()                   { return current->m_drivers;    }
    static Cars * getCars()                         { return current->m_cars;       }
    static Delivery * getDelivery()                 { return current->m_del;        }
    static MainWindow * getMainWindow()             { return current;               }

private slots:
    void completed(Request::Command);
    void showError(QString err);

    void selectAll();
    void startedDownloadInformation(bool flag);

public slots:

private:
    bool createSystemFileTimeOut();
};

#endif // MAINWINDOW_H
