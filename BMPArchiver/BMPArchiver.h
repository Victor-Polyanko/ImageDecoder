#pragma once
#ifndef BMPArchiver_H
#define BMPArchiver_H
#include <qstring.h>
#include <qbytearray.h>
#include <qimage.h>
#include <qvector.h>
#include <qbitarray.h>
#include "bmparchiver_global.h"

class BMPARCHIVER_EXPORT BMPArchiver
{
public:
    BMPArchiver();
    QString convert(QString aFileName);

  private:
      void readBMP(QString aFileName);
      void BMPtoBARCH();
      void writeBARCH(QString aFileName);
      void readBARCH(QString aFileName);
      void BARCHtoBMP();
      void writeBMP(QString aFileName);
  
  private:
      int mWidth;
      int mHeight;
      QByteArray mBMPData;
      QImage mBMPImage;
      QVector<QBitArray> mBARCHData;
};
#endif
