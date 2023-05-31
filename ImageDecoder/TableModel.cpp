#include "tablemodel.h"
#include "FileWorker.h"
#include <qdatetime.h>

namespace
{
    const QString FileName = "Name";
    const QString FileType = "Type";
    const QString FileDate = "Date";
    const QString FileSize = "Size";
    const QString ImageAtributes[] = { FileName, FileType, FileDate, FileSize };
}

TableModel::TableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    FileWorker* worker = new FileWorker;
    worker->moveToThread(&mWorkerThread);
    connect(&mWorkerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &TableModel::processFile, worker, &FileWorker::processFile);
    connect(worker, &FileWorker::succed, this, &TableModel::processSucces);
    connect(worker, &FileWorker::failed, this, &TableModel::processFailure);
    mWorkerThread.start();
}

TableModel::~TableModel() {
    mWorkerThread.quit();
    mWorkerThread.wait();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal)
            return ImageAtributes[section];
    }
    return QVariant();
}

int TableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return mHeaders.size();
}

int TableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return ImageAtributes->size();
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role == Qt::DisplayRole
        && index.row() >= 0 && index.row() < rowCount()
        && index.column() >= 0 && index.column() < columnCount())
        return mHeaders[index.row()][ImageAtributes[index.column()]];
    return QVariant();
}

void TableModel::init(const QString& aPath, const QStringList& aFiles)
{
    for (const auto& file : aFiles)
    {
        auto fullName = aPath + file;
        addHeader(fullName);
    }
    mPath = aPath;
}

void TableModel::addHeader(const QString& aName)
{
    ImageHeader header;
    auto info = QFileInfo(aName);
    header[FileName] = info.completeBaseName();
    header[FileType] = info.completeSuffix();
    header[FileDate] = info.created().toString();
    header[FileSize] = QString::number(info.size());
    mHeaders.push_back(header);
}

void TableModel::onClick(const int& aRow)
{
    if (mActiveFiles.find(aRow) != mActiveFiles.end())
        return;
    auto fullName = mPath + mHeaders[aRow][FileName] + '.' + mHeaders[aRow][FileType];
    addDescription(aRow);
    emit processFile(fullName, aRow);
}

void TableModel::addDescription(const int& aInputFileId)
{
    mActiveFiles[aInputFileId] = mHeaders[aInputFileId][FileName];
    mHeaders[aInputFileId][FileName] += mHeaders[aInputFileId][FileType] == QString("barch")
                                      ? QString(" decoding...")
                                      : QString(" coding...");
    QModelIndex index = this->index(aInputFileId, 0);
    emit dataChanged(index, index);
}

void TableModel::removeDescription(const int& aInputFileId)
{
    mHeaders[aInputFileId][FileName] = mActiveFiles[aInputFileId];
    mActiveFiles.remove(aInputFileId);
    QModelIndex index = this->index(aInputFileId, 0);
    emit dataChanged(index, index);
}

void TableModel::processSucces(const QString& aNewName, const int& aInputFileId)
{
    removeDescription(aInputFileId);
    const auto baseName = QFileInfo(aNewName).completeBaseName();
    for (const auto& header : mHeaders)
        if (header[FileName] == baseName)
            return;
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    addHeader(aNewName);
    endInsertRows();
}

void TableModel::processFailure(const QString& aMessage, const int& aInputFileId)
{
    removeDescription(aInputFileId);
    showMessageBox(aMessage);
}