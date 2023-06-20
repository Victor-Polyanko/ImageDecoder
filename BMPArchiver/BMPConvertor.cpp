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
    const auto standardBlockAmount = mWidth / cStandardBlockSize;
    const auto lastBlockSize = mWidth % cStandardBlockSize;
    const auto maxBitsUsed = standardBlockAmount * (2 + cStandardBlockSize * cBitsInByte) + (2 + lastBlockSize * cBitsInByte);
    QVector<unsigned char> lastPixels;
    lastPixels.resize(cStandardBlockSize);
    for (int y = 0; y < mHeight; ++y)
    {
        int x = 0;
        int bitsUsed = 0;
        bool isRowBlank = true;
        auto blockSize = cStandardBlockSize;
        QBitArray row(maxBitsUsed);
        auto processPixel = [&]()
        {
            for (int i = 0; i < blockSize; ++i)
                lastPixels[i] = mBMPImage.pixel(x + i, y);
            if (lastPixels[0] == cWhite)
            {
                if (std::find_if(lastPixels.begin() + 1, lastPixels.begin() + blockSize, [&](uchar p) {return p != cWhite; }) == lastPixels.end())
                {
                    row.setBit(bitsUsed++, false);
                    return;
                }
            }
            else if (lastPixels[0] == cBlack)
            {
                if (std::find_if(lastPixels.begin() + 1, lastPixels.begin() + blockSize, [&](uchar p) {return p != cBlack; }) == lastPixels.end())
                {
                    row.setBit(bitsUsed++, true);
                    row.setBit(bitsUsed++, false);
                    isRowBlank = false;
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
        };
        for (int block = 0; block < standardBlockAmount; ++block, x += cStandardBlockSize)
            processPixel();
        blockSize = lastBlockSize;
        if (lastBlockSize > 0)
            processPixel();
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