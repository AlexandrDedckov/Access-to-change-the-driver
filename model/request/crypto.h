#ifndef CRYPTO_H
#define CRYPTO_H

#include <QObject>

class Crypto : public QObject
{
    Q_OBJECT
private:
    char decode_char(const QByteArray & cc);
    void encode_char(char c, QByteArray & output);

public:
    explicit Crypto(QObject *parent = 0);
    void decrypt(const QByteArray & input, QByteArray & output);
    void encrypt(const QByteArray & input, QByteArray & output);

signals:

public slots:

};

#endif // CRYPTO_H
