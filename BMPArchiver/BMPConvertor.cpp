#include "BMPConvertor.h"
#include <qfile.h>

void BMPConvertor::read(const QString& aFileName)
{
    mBMPImage = QImage(aFileName);
    mHeight = mBMPImage.height();
    mWidth = mBMPImage.width();
    if (mHeight <= 0 || mWidth <= 0)
        throw QString("Unsupported file");
}

QString BMPConvertor::convertFileName(const QString& aFileName)
{
    auto suffixPos = aFileName.lastIndexOf('.');
    return aFileName.left(suffixPos) + "packed.barch";
}

void BMPConvertor::convertData()
{
    if (mBMPImage.isNull())
        return;
    mBARCHData.reserve(mHeight);
    const size_t standardBlockAmount = mWidth / cStandardBlockSize;
    const size_t lastBlockSize = mWidth % cStandardBlockSize;
    size_t maxBitsUsed = standardBlockAmount * (2 + cStandardBlockSize * cBitsInByte) + (2 + lastBlockSize * cBitsInByte);
    for (int y = 0; y < mHeight; ++y)
    {
        int x = 0;
        size_t bitsUsed = 0;
        bool isRowBlank = true;
        QBitArray row(maxBitsUsed);
        auto processPixel = [&](size_t blockSize)
        {
            QVector<unsigned char> lastPixels;
            lastPixels.reserve(blockSize);
            for (int i = 0; i < blockSize; ++i)
                lastPixels.push_back(mBMPImage.pixel(x + i, y));
            if (lastPixels[0] == cWhite)
            {
                size_t i = 1;
                for (; i < blockSize; ++i)
                    if (lastPixels[i] != cWhite)
                        break;
                if (i == blockSize)
                {
                    row.setBit(bitsUsed++, false);
                    lastPixels.clear();
                    return;
                }
            }
            else if (lastPixels[0] == cBlack)
            {
                size_t i = 1;
                for (; i < blockSize; ++i)
                    if (lastPixels[i] != cBlack)
                        break;
                if (i == blockSize)
                {
                    row.setBit(bitsUsed++, true);
                    row.setBit(bitsUsed++, false);
                    isRowBlank = false;
                    lastPixels.clear();
                    return;
                }
            }
            row.setBit(bitsUsed++, true);
            row.setBit(bitsUsed++, true);
            for (auto& pixel : lastPixels)
            {
                for (int i = cBitsInByte - 1; i >= 0; --i)
                {
                    auto value = (pixel >> i) & 1;
                    row.setBit(bitsUsed++, value);
                }
            }
            isRowBlank = false;
            lastPixels.clear();
        };
        for (int block = 0; block < standardBlockAmount; ++block, x += cStandardBlockSize)
            processPixel(cStandardBlockSize);
        if (lastBlockSize > 0)
            processPixel(lastBlockSize);
        if (isRowBlank)
            mBARCHData.push_back(cBlankRow);
        else
        {
            row.resize(bitsUsed);
            mBARCHData.push_back(row);
        }
    }
}

void BMPConvertor::write(const QString& aFileName)
{
    if (mBARCHData.empty() || mBMPImage.isNull())
        return;
    QFile file(aFileName);
    if (!file.open(QIODevice::WriteOnly))
        return;
    QDataStream out(&file);
    out << mWidth << mHeight;
    for (const auto& row : mBARCHData)
        out << row.size() << row;
    file.close();
}