#ifndef USER_H
#define USER_H

#include <QObject>
#include "model/request/request.h"
#include "mainwindow.h"

class User : public IRequest
{
    Q_OBJECT
public:
    enum Access
    {
        NoSet = 0,
        ReadOnly = 1,
        Full = 2,
        Admin = 3,
        PlannerAutopark = 4
    };

private:
    quint64 m_id = 0;
    QString m_login;
    QString m_fio;
    QString m_password;
    Access m_access = NoSet;
    qint64 m_id_park = -1;
    QList<User *> m_users;
    static User * m_current;
    QString version;

    void parseError(const QByteArray & resp, QString & err);

public:
    static const int LenPass = 6;
    static const int LenLogin = 3;
    static User * Instance()                    { return m_current;     }

    explicit User(QObject *parent = 0);
    ~User()                                     { emit objRemove();     }
    void setId(quint64 id);
    void setAccess(User::Access access);
    void setLogin(const QString & login);
    void setPassword(const QString & password);
    void setFio(const QString & fio);
    void setIdPark(qint64 id_park);

    quint64 getId()                             { return m_id;          }
    User::Access getAccess()                    { return m_access;      }
    QString getLogin()                          { return m_login;       }
    QString getPassword()                       { return m_password;    }
    QString getFio()                            { return m_fio;         }
    qint64 getIdPark()                          { return m_id_park;     }
    QList<User *> * getUsers()                  { return &m_users;      }

    User * Create();
    void Remove(User * user);
    void Login();
    void SelectUsers();
    void Insert();
    void Delete();

    void setResponse(QByteArray response, Request::Command cmd);
    void Update();

signals:

public slots:
    void setVersion();
};

#endif // USER_H
