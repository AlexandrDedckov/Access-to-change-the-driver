#ifndef SERVERCOFIG_H
#define SERVERCOFIG_H

#include <QObject>
#include <QVector>

class ServerConfig : public QObject
{
    Q_OBJECT
public:
    explicit ServerConfig(QObject *parent = 0);
    QVector<quint16> * Loop = NULL;
    bool IsGLE = true;
    quint16 ServerId = 0;
    QString API_GENERAL_IP;
    quint16 API_GENERAL_PORT = 80;
    QString API_UPDATE_IP;
    quint16 API_UPDATE_PORT = 80;

    bool Load();
    bool Save();

    ~ServerConfig();
};

#endif // SERVERCOFIG_H
