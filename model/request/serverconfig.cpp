#include "serverconfig.h"
#include <QFile>
#include <QStringList>

ServerConfig::ServerConfig(QObject *parent) :
    QObject(parent)
{

}

bool ServerConfig::Load()
{
    QFile file(tr("server_config.conf"));
    if (!file.exists()) return false;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))  return false;
    QString data = tr(file.readAll().data());
    file.close();

    QStringList arr = data.split(tr("\n"));
    if(arr.count() != 3)    return false;
    IsGLE = arr[1] == tr("1");
    ServerId = arr[2].toUInt();
    arr = arr[0].split(tr(","));
    if(arr.count() == 0 || Loop == NULL)    return false;
    Loop->clear();
    foreach (QString str, arr) Loop->append(str.toUInt());
    return true;
}

bool ServerConfig::Save()
{
    QFile file(tr("server_config.conf"));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))  return false;

    if(Loop->count() == 0)   return false;
    QString str = tr("");
    foreach (quint16 index, *Loop)   str += QString::number(index) + tr(",");
    str = str.mid(0, str.length()-1);
    Loop = NULL;

    file.write(str.toUtf8() + "\n");
    file.write(IsGLE ? "1\n" : "0\n");
    file.write(QString::number(ServerId).toUtf8());
    file.close();
    return true;
}

ServerConfig::~ServerConfig()
{
}
