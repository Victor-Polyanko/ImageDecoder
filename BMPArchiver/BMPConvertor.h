#pragma once
#ifndef BMPConvertor_H
#define BMPConvertor_H
#include "BaseConvertor.h"
#include <qimage.h>
#include <qstring.h>

class BMPConvertor : public BaseConvertor
{
protected:
    virtual void read(const QString& aFileName) override;
    virtual QString convertFileName(const QString& aFileName) override;
    virtual void convertData() override;
    virtual void write(const QString& aFileName) override;

private:
    QImage mBMPImage;
};
#endif

