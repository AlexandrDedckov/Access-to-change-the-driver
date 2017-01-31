#include "mainwindow.h"
#include <QCoreApplication>
#include <QDebug>

class Application : public QCoreApplication
{
public:
    Application(int &argc, char ** argv) :
        QCoreApplication(argc, argv)
    { }

    // ~MyApplication();
private:
    bool notify(QObject *receiver, QEvent *event)
    {
        try { return QCoreApplication::notify(receiver, event); }
        catch(std::exception& e)
        {
            qCritical() << "Exception thrown:" << e.what();
        }
        return true;
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MainWindow w;

    return a.exec();
}
