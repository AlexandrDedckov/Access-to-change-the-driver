#include "crypto.h"
#include <QMap>
#include <QStringList>

Crypto::Crypto(QObject *parent) :
    QObject(parent)
{
}

char Crypto::decode_char(const QByteArray & cc)
{
    //int len = 64; // 3 + 58-49 + 90-64 + 122-96;
    QMap<int, int> ar;
    ar.insert(40, 0);
    ar.insert(41, 1);
    ar.insert(48, 2);

    QByteArray d = "";
    d += cc[0];

    int i = d.toInt();
    char c = cc[1];

    if ((int)c >= 97) c = (char)((int)c + 37 - 96);
    else if (c >= 65) c = (char)((int)c + 11 - 64);
    else if (c >= 50) c = (char)((int)c - 47);
    else if (ar.contains((int)c)) c = (char)ar[(int)c];

    return i > 0 ? (char)((int)c + 64) : c;
}

void Crypto::encode_char(char c, QByteArray & output)
{
    int i = 0;
    int len = 64; // 3 + 58-49 + 90-64 + 122-96;
    if (c > len - 1)
    {
        c = (char)qAbs(len - c);
        i++;
    }

    char let = '0';
    QByteArray ar = "()0";
    if (c <= 2) let = ar[(int)c];
    else if (c <= 11) let = (char)((int)c + 47);
    else if (c <= 37) let = (char)((int)c - 11 + 64);
    else if (c <= 63) let = (char)((int)c - 37 + 96);

    output = QString::number(i).toLocal8Bit();
    output += c;
    output += let;
}

void Crypto::decrypt(const QByteArray & input, QByteArray & output)
{
    QByteArray t_input = input + "q";
    output = "";
    QList<QByteArray> chars;

    int length = t_input.length();
    int offset = 0;

    // парсим строку на символы
    while (offset < length - 1)
    {
        QByteArray c(t_input.mid(offset, 2));

        if (c[1] == '1') offset += 1;
        else c[1] = '0';

        QByteArray pb = "";
        pb += c[1];
        pb += c[0];

        chars.append(pb);
        offset += 1;
    }

    // расшифровываем символы
    for (int i = 0; i < chars.count(); i++)
    {
        QByteArray c = chars[i];
        char char_dec = decode_char(c);	// символ в нашем списке

        output += char_dec;
    }
}

void Crypto::encrypt(const QByteArray & input, QByteArray & output)
{
    output = "";
    int length = input.count();
    for (int i = 0; i < length; i++)
    {
        QByteArray c;
        encode_char(input[i], c);	// символ в нашем списке

        QByteArray d = "";
        d += c[0];

        output += c[2];
        if (d.toInt() > 0) output += c[0];
    }
}
