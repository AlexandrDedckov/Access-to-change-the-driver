#ifndef DRIVERNODE_H
#define DRIVERNODE_H

#include <QObject>
#include <QDateTime>

class DriverNode : public QObject
{
    Q_OBJECT
private:
    quint64 m_id;
    quint64 m_id_driver;
    quint64 m_id_user;
    QDateTime m_date;
    QString m_node;
    QString m_username;

public:
    explicit DriverNode(QObject *parent = 0);
    void setIdDriver(quint64 id)                { m_id_driver = id;                         }
    void setId(quint64 id)                      { m_id = id;                                }
    void setIdUser(quint64 id)                  { m_id_user = id;                           }
    void setDate(const QDateTime & date)
    {
        if(date.isValid())
            m_date = date;
        else
            m_date = QDateTime::currentDateTime();
    }
    void setNode(const QString & node)          { m_node = node;                            }
    void setUserName(const QString & username)  { m_username = username;                    }
    DriverNode * clone()
    {
        DriverNode * node = new DriverNode();
        node->m_id = m_id;
        node->m_id_user = m_id_user;
        node->m_id_driver = m_id_driver;
        node->m_date = m_date;
        node->m_node = m_node;
        node->m_username = m_username;
        return node;
    }

    quint64 getId()                             { return m_id;                                      }
    quint64 getIdUser()                         { return m_id_user;                                 }
    quint64 getIdDriver()                       { return m_id_driver;                               }
    QDateTime getDate()                         { return m_date;                                    }
    QString getNode()                           { return m_node;                                    }
    QString getUserName()                       { return m_username;                                }
    QString toString()                          { return m_date.toString() + m_node + m_username;   }

signals:

public slots:

};

#endif // DRIVERNODE_H
