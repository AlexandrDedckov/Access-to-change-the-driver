#ifndef QCOMPRESSOR_H
#define QCOMPRESSOR_H

#include <zlib.h>
#include <QByteArray>

//#define GZIP_WINDOWS_BIT 15 + 16
#define GZIP_CHUNK_SIZE 32 * 1024
#define TUPE_DECOMPRESS1 47
#define TUPE_DECOMPRESS2 -8

class QCompressor
{
public:
    static bool gzipCompress(const QByteArray & input, QByteArray &output, int level = -1);
    static bool gzipDecompress(const QByteArray & input, QByteArray &output, int type = TUPE_DECOMPRESS1);
};

#endif // QCOMPRESSOR_H
