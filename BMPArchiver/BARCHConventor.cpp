#include "BARCHConvertor.h"
#include <qdatastream.h>
#include <qfile.h>

void BARCHConvertor::read(const QString& aFileName)
{
    mBARCHData.clear();
    QFile file(aFileName);
    if (!file.open(QIODevice::ReadOnly))
        return;
    QDataStream in(&file);
    in >> mWidth >> mHeight;
    if (mHeight <= 0 || mWidth <= 0)
        throw QString("Unsupported file");
    mBARCHData.resize(mHeight);
    for (auto& row : mBARCHData)
    {
        int rowSize;
        in >> rowSize;
        row.resize(rowSize);
        in >> row;
    }
    file.close();
}

QString BARCHConvertor::convertFileName(const QString& aFileName)
{
    auto suffixPos = aFileName.lastIndexOf('.');
    return aFileName.left(suffixPos) + "unpacked.bmp";
}

void BARCHConvertor::convertData() {
    if (mBARCHData.empty())
        return;
    mBMPData.clear();
    mBMPData.reserve(mWidth * mHeight);
    const int standardBlocksAmount = mWidth / cStandardBlockSize;
    const int lastBlockSize = mWidth % cStandardBlockSize;
    for (const auto& row : mBARCHData)
    {
        if (row == cBlankRow)
        {
            mBMPData.append(mWidth + cBMPDataTail, cWhite);
            continue;
        }
        int bit = 0;
        auto processBlock = [&](const int& blockSize)
        {
            if (!row[bit++])
            {
                mBMPData.append(blockSize, cWhite);
                return;
            }
            if (!row[bit++])
            {
                mBMPData.append(blockSize, cBlack);
                return;
            }
            for (int b = 0; b < blockSize; ++b)
            {
                unsigned char value = 0;
                for (int i = 0; i < cBitsInByte; ++i)
                    value = (value << 1) + row[bit++];
                mBMPData.append(value);
            }
        };
        for (int block = 0; block < standardBlocksAmount; ++block)
            processBlock(cStandardBlockSize);
        if (lastBlockSize > 0)
            processBlock(lastBlockSize);
        mBMPData.append(cBMPDataTail, cWhite);
    }
}

void BARCHConvertor::write(const QString& aFileName)
{
    auto BMPImage = QImage((uchar*)mBMPData.data(), mWidth, mHeight, QImage::Format::Format_Grayscale8);
    BMPImage.save(aFileName);
}