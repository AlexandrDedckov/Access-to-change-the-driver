#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QJsonDocument>
#include <QFile>
#include <QDebug>

class JsonParser
{
public:
    static bool getObject(const QByteArray & array, QJsonObject & obj)
    {
        QJsonDocument doc = QJsonDocument::fromJson(array);
        if (doc.isEmpty())
            return false;
        QJsonObject root = doc.object();
        if(root.isEmpty())
            return false;
        obj = root;
        return true;
    }

private:
    JsonParser(){}
};

#endif // JSONPARSER_H
