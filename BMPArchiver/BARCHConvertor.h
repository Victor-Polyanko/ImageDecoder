#pragma once
#ifndef BARCHConvertor_H
#define BARCHConvertor_H
#include <qstring.h>
#include "BaseConvertor.h"

class BARCHConvertor : public BaseConvertor
{
protected:
    virtual void read(const QString& aFileName) override;
    virtual QString convertFileName(const QString& aFileName) override;
    virtual void convertData() override;
    virtual void write(const QString& aFileName) override;

private:
    QByteArray mBMPData;
};
#endif

