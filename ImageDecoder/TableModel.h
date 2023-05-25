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
    void addNewFile(const QString& aName); 

private:
    void addHeader(const QString& aInfo);

signals:
    void processFile(const QString& aName);
    void showMessageBox(const QString& aName);

private:
    QThread mWorkerThread;
    QString mPath;
    QVector<ImageHeader> mHeaders;
};

#endif // TABLEMODEL_H