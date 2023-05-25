#include "FileWorker.h"
#include "BMPArchiver.h"

void FileWorker::processFile(const QString& aInputName)
{
    try {
        BMPArchiver archiver;
        auto outputName = archiver.convert(aInputName);
        emit succed(outputName);
    }
    catch (QString message)
    {
        emit failed(message);
    }
}
