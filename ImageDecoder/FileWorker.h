#pragma once
#ifndef FILEWORKER_H
#define FILEWORKER_H

#include <qthread.h>

class FileWorker : public QObject
{
    Q_OBJECT

public slots:
    void processFile(const QString& aFileName, const int& aInputFileId = 0);

signals:
    void succed(const QString& aResultMessage, const int& aInputFileId);
    void failed(const QString& aResultMessage, const int& aInputFileId);
};

#endif // FILEWORKER_H