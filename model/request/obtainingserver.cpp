#include "obtainingserver.h"
#include "qcompressor.h"
#include "mainwindow.h"
#include "model/request/jsonparser.h"
#include "txt_constants.h"
#include <QDebug>

ObtainingServer::ObtainingServer(QObject *parent) :
    QThread(parent),
    mask("http://@ip/t/@city/autoplan/@c.html")
{
    //serversGLE.append(new Server("185.71.65.78", 8884, this));
    serversGLE.append(new Server("api.taxovichkof.ru", 8884, this));

    servers.append(new Server("192.168.0.30", 81, this));
    servers.append(new Server("192.168.0.32", 81, this));

    dec.insert("fGud", "0");        dec.insert("Hidjo", "0");   dec.insert("yUyyud", "0");
    dec.insert("ftUhj", "1");       dec.insert("HjisiI", "1");  dec.insert("TydoO", "1");
    dec.insert("kiOfk", "2");       dec.insert("adfdyT", "2");  dec.insert("uuiOpd", "2");
    dec.insert("DfdgT", "3");       dec.insert("gtYudi", "3");  dec.insert("uwIIeh", "3");
    dec.insert("Fghyi", "4");       dec.insert("Iodpei", "4");  dec.insert("Rterd", "4");
    dec.insert("NhfUi", "5");       dec.insert("JJkdud", "5");  dec.insert("OpdOOp", "5");
    dec.insert("GhdyT", "6");       dec.insert("YudiU", "6");   dec.insert("qAsqwa", "6");
    dec.insert("fgdttT", "7");      dec.insert("ASzsa", "7");   dec.insert("xzsaA", "7");
    dec.insert("ffff", "8");        dec.insert("FdrTh", "8");   dec.insert("LpdlK", "8");
    dec.insert("yuIo", "9");        dec.insert("hBvgd", "9");   dec.insert("NBbVCg", "9");
    dec.insert("Tyipf", ".");       dec.insert("Ijdji", ".");   dec.insert("Hjfiejy", ".");
    dec.insert("HdudjeyY", ".");    dec.insert("Hjdens", ".");  dec.insert("JhftFaS", ".");
}

void ObtainingServer::run()
{
    bool state = false;
    QVector<quint16> loop;
    ServerConfig * serverConfig = MainWindow::getServerConfig();
    serverConfig->Loop = &loop;
    QVector<const Server *> * curr_servers = NULL;

    state = serverConfig->Load();
    if(serverConfig->IsGLE)
        curr_servers = &serversGLE;
    else
        curr_servers = &servers;

    if(state || loop.count() != curr_servers->count())
    {
        for(quint16 i = 0; i < curr_servers->count(); i++)
            loop.append(i);
    }

    newLoop.clear();
    newLoop = loop;
    newLoop.detach();

    count_repeat = curr_servers->count()-1;
    for (now_index = 0; now_index < count_repeat+1; ++now_index)
    {
        const Server * curr_server = curr_servers->at((quint16)(loop[now_index]));
        //IP
        QByteArray addr = QByteArray(mask).replace("@ip", curr_server->Address);

        //ГОРОД
        if(serverConfig->ServerId == 0) addr = addr.replace("@city", "spb");

        //УДАЛЕННО-ВНУТРЕННЕ
        if (serverConfig->IsGLE) addr = addr.replace("@c", "glec");
        else addr = addr.replace("@c", "c");

        QUrl url(tr(addr.data()));
        //PORT
        url.setPort(curr_server->Port);
        QNetworkRequest req(url);
        req.setRawHeader("Accept-Encoding", "gzip, deflate");

        QNetworkAccessManager netManager;
        QNetworkReply * netReply = netManager.get(req);
        connect(netReply, SIGNAL(finished()), this, SLOT(readyRead()));
        exec();
    }
}

void ObtainingServer::readyRead()
{
    QNetworkReply * netReply = qobject_cast<QNetworkReply *>(QObject::sender());
    if (netReply->error() == QNetworkReply::NoError)
    {
        QByteArray jsonData, data;
        data = netReply->readAll();
        netReply->deleteLater();
        if(QCompressor::gzipDecompress(data, jsonData))
        {
            QString ip;
            quint16 port;
            if(!getAddress("\"UnsnI34jsfjjmUHNhu\":{", jsonData, ip, port)) return;
            ServerConfig * serverConfig = MainWindow::getServerConfig();
            serverConfig->API_GENERAL_IP = tr("http://") + ip;
            serverConfig->API_GENERAL_PORT = port;

            if(!getAddress("\"siuerjmpovsdpojerYe4\":{", jsonData, ip, port)) return;
            serverConfig->API_UPDATE_IP = ip;
            serverConfig->API_UPDATE_PORT = port;

            serverConfig->Loop = &newLoop;
            serverConfig->Save();
            terminate();
            emit finishedRequest();
        }
        else
        {
            if(now_index >= count_repeat)
            {
                isError = true;
                emit error(MSG_INVALID_RESPONSE);
            }
            exit();
            return;
        }
    }
    else
    {
        //ЗАМЕНА МЕСТАМИ ОБХОДА IP
        if (newLoop.count() > 2 && now_index < newLoop.count() - 2)
        {
            quint16 tmp = newLoop[now_index];
            newLoop[now_index] = newLoop[newLoop.count() - 2];
            newLoop[newLoop.count() - 2] = tmp;
        }
        else if(now_index >= count_repeat)
        {
            isError = true;
            emit error(MSG_NO_RESPONSE_IS_RECIVED);
        }
        exit();
    }
}

bool ObtainingServer::getAddress(const QString & key, const QByteArray & data, QString & ip, quint16 & port)
{
    int startIndex = data.indexOf(key);
    int endIndex = data.indexOf("}", startIndex);
    QByteArray t_data;
    if(startIndex > -1 && endIndex > -1 && endIndex > startIndex)
    {
        t_data = data.mid(startIndex, endIndex - startIndex + 1);
        t_data = t_data.mid(t_data.indexOf("{"));
    }
    else
    {
        if(now_index >= count_repeat)
        {
            isError = true;
            emit error(MSG_INVALID_RESPONSE);
        }
        exit();
        return false;
    }
    QJsonObject obj;
    if(!JsonParser::getObject(t_data, obj))
    {
        if(now_index >= count_repeat)
        {
            isError = true;
            emit error(MSG_INVALID_RESPONSE);
        }
        exit();
        return false;
    }
    decodeINI(obj[tr("hashKey")].toString().toUtf8(), t_data);
    ip = tr(t_data.data());

    decodeINI(obj[tr("pass")].toString().toUtf8(), t_data);
    port = t_data.toUInt();
    return true;
}

void ObtainingServer::decodeINI(const QByteArray & code, QByteArray & output)
{
    output = "";
    int curr = 0;
    int len = code.length();
    for (int i = 1; i <= len; ++i)
    {
        if (dec.contains(code.mid(curr, i - curr)))
        {
            output += dec[code.mid(curr, i - curr)];
            curr = i;
        }
    }
}
