#pragma once
#ifndef BMPArchiver_H
#define BMPArchiver_H
#include <qstring.h>
#include "bmparchiver_global.h"

class BMPARCHIVER_EXPORT BMPArchiver
{
public:
    BMPArchiver();
    QString convert(const QString& aFileName);
};
#endif
