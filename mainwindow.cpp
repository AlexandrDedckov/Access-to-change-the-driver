#include "mainwindow.h"
#include "txt_constants.h"


MainWindow * MainWindow::current = NULL;
Request * MainWindow::m_request = NULL;
QueueRequests * MainWindow::m_queue = NULL;
ServerConfig * MainWindow::m_serverConfig = NULL;
SignalingUpdate * MainWindow::m_sUpdate = NULL;
Drivers * MainWindow::m_drivers = NULL;
Cars * MainWindow::m_cars = NULL;
Delivery * MainWindow::m_del = NULL;

MainWindow::MainWindow(QObject *parent) :
    QObject(parent)
{
    if (!createSystemFileTimeOut())
        qDebug() << "Ошибка создания файла таймаута рассылки";

    m_serverConfig = new ServerConfig(this);

    m_request = new Request(this);
    connect(m_request, SIGNAL(error(QString)),
            SLOT(showError(QString)));
    m_queue = new QueueRequests(this);
    m_queue->connectRequest();

    m_autoParks = new AutoParks(this);
    connect(m_autoParks, SIGNAL(completed(Request::Command)),
            SLOT(completed(Request::Command)));
    connect(m_autoParks, &AutoParks::getStarted,
            this, &MainWindow::startedDownloadInformation);
    m_autoParks->Select();

    m_drivers = new Drivers(this);
    m_cars = new Cars(this);
    m_del = new Delivery(m_drivers, m_cars, this);
    m_del->setAutoPark(m_autoParks);

    connect(m_drivers, SIGNAL(completeDrivers()), m_del, SLOT(completeDownload()));
    connect(m_cars, SIGNAL(completeCars()), m_del, SLOT(completeDownload()));

    timerUpdate = new QTimer(this);
    connect(timerUpdate, SIGNAL(timeout()), m_autoParks, SLOT(getStartProgram()));

    timerUpdate->start(4*60*1000);

    dateStart = QDate::currentDate();
}

void MainWindow::selectAll()
{
    qDebug() << endl << "[START UPDATE]" << QDateTime::currentDateTime().toString();
    QList<Driver*> drivers;
    m_drivers->getDrivers(drivers);
    for (Driver *driver : drivers)
        driver->getMonth()->clear();

    QMap<int, QString>* autoparks = m_autoParks->getAutoParks();
    qDebug() << "[AUTOPARK COUNT]" << autoparks->count();

    QList <int> list
    {
        0
    };

    m_drivers->clear();
    m_drivers->setAutoPark(autoparks->keys());
//    m_drivers->setAutoPark(list);
    m_drivers->Select();

//
    m_cars->clear();
    m_cars->setAutoPark(autoparks->keys());
//    m_cars->setAutoPark(list);
    m_cars->Select();
}

void MainWindow::startedDownloadInformation(bool flag)
{
    if (flag)
    {
        m_del->startTimerDelivery(true);
        selectAll();
    }
    else
    {
        qDebug() << "NOT STARTED DOWNLOAD INFO";
        m_del->startTimerDelivery(false);
        m_del->clear();
    }
}

bool MainWindow::createSystemFileTimeOut()
{
    QFile file("config.ini");

    if(file.exists())
        return true;

    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Ошибка открытия для записи";
        return false;
    }

    QByteArray byteArray = "60";
    file.write(byteArray);
    file.close();

    return true;
}

void MainWindow::completed(Request::Command cmd)
{
    switch (cmd)
    {
    case Request::AutoParksSelectNames:
        m_autoParks->getStartProgram();
        break;
    }
}

void MainWindow::showError(QString err)
{
    qDebug() << TXT_ERROR << err;
}

MainWindow::~MainWindow()
{
}
