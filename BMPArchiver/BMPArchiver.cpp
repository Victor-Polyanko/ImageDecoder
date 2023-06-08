#include "BMPArchiver.h"
#include "BARCHConvertor.h"
#include "BMPConvertor.h"
#include <memory>

namespace
{
    bool isFileBARCH(const QString& aName)
    {
        return aName.endsWith(QString(".barch"), Qt::CaseInsensitive);
    }
}

BMPArchiver::BMPArchiver()
{}

QString BMPArchiver::convert(const QString& aFileName)
{
    std::shared_ptr<BaseConvertor> convertor;
    if (isFileBARCH(aFileName))
        convertor = std::make_shared<BARCHConvertor>();
    else
        convertor = std::make_shared<BMPConvertor>();
    return convertor->convert(aFileName);
}
