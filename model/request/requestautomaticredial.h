#pragma once

#include <QThread>
#include <QtNetwork>
#include <QtCore>


class RequestAutomaticRedial : public QThread
{
    Q_OBJECT
public:
    explicit RequestAutomaticRedial(QObject *parent = nullptr);
    void setData(const QString& data);

private:
    void run();

private:
    QString m_data;
    const QString api_name = "CallBackUser";
    const QString api_pass = "WilFjh1GgT2To";

signals:
    void finishThreed();

private slots:
    void readyRead();
};

