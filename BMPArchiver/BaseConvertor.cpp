#include "BaseConvertor.h"

BaseConvertor::BaseConvertor()
    : mWidth(0)
    , mHeight(0)
{}

QString BaseConvertor::convert(const QString& aFileName)
{
    read(aFileName);
    auto convertedName = convertFileName(aFileName);
    convertData();
    write(convertedName);
    return convertedName;
}