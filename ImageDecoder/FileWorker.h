#pragma once
#ifndef FILEWORKER_H
#define FILEWORKER_H

#include <qthread.h>

class FileWorker : public QObject
{
    Q_OBJECT

public slots:
    void processFile(const QString& aFileName);

signals:
    void succed(const QString& aResultMessage);
    void failed(const QString& aResultMessage);
};

#endif // FILEWORKER_H