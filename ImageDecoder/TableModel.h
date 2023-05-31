#pragma once
#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QAbstractTableModel>
#include <qfileinfo.h>
#include <qthread.h>

typedef QMap<QString, QString> ImageHeader;

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TableModel(QObject* aParent = nullptr);
    ~TableModel();
    Q_INVOKABLE QVariant headerData(int aSection, Qt::Orientation aOrientation, int aRole = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& aParent = QModelIndex()) const override;
    int columnCount(const QModelIndex& aParent = QModelIndex()) const override;
    QVariant data(const QModelIndex& aIndex, int aRole = Qt::DisplayRole) const override;

    void init(const QString& aPath, const QStringList& aFiles);
    Q_INVOKABLE void onClick(const int& aRow);

public slots:
    void processSucces(const QString& aNewName, const int& aInputFileId);
    void processFailure(const QString& aMessage, const int& aInputFileId);

private:
    void addDescription(const int& aInputFileId);
    void removeDescription(const int& aInputFileId);
    void addHeader(const QString& aInfo);

signals:
    void processFile(const QString& aName, const int& aInputFileId);
    void showMessageBox(const QString& aName);

private:
    QThread mWorkerThread;
    QString mPath;
    QVector<ImageHeader> mHeaders;
    QMap<int, QString> mActiveFiles;
};

#endif // TABLEMODEL_H