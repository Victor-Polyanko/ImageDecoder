#include "FileWorker.h"
#include "BMPArchiver.h"

void FileWorker::processFile(const QString& aInputName, const int& aInputFileId)
{
    try {
        BMPArchiver archiver;
        auto outputName = archiver.convert(aInputName);
        emit succed(outputName, aInputFileId);
    }
    catch (QString message)
    {
        emit failed(message, aInputFileId);
    }
}
