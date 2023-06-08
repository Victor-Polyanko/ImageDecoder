#pragma once
#ifndef IConvertor_H
#define IConvertor_H
#include <qstring.h>
#include <qbitarray.h>
#include <qimage.h>

class BaseConvertor
{
public:
    BaseConvertor();
    QString convert(const QString& aFileName);

protected:
    virtual void read(const QString& aFileName) = 0;
    virtual QString convertFileName(const QString& aFileName) = 0;
    virtual void convertData() = 0;
    virtual void write(const QString& aFileName) = 0;

protected:
    int mWidth;
    int mHeight;
    QVector<QBitArray> mBARCHData;

    const uchar cBlack = 0x00;
    const uchar cWhite = 0xff;
    const int cBitsInByte = 8;
    const int cBMPDataTail = 3;
    const int cStandardBlockSize = 4;
    const QBitArray cBlankRow = QBitArray({ true });
};
#endif

