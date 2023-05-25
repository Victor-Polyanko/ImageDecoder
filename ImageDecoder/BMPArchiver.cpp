#include "BMPArchiver.h"
#include <qfile.h>
#include <qimage.h>
#include <qbitarray.h>
#include <qbuffer.h>

namespace
{
    const uchar cBlack = 0x00;
    const uchar cWhite = 0xff;
    const int cBitsInByte = 8;
    const int cStandardBlockSize = 4;
    const int cBMPDataTail = 3;
    const QBitArray cBlankRow({ true });

    bool isFileBARCH(QString aName)
    {
        return aName.endsWith(QString(".barch"), Qt::CaseInsensitive);
    }

    QString nameFromBMPtoBARCH(QString aBMPName)
    {
        auto suffixPos = aBMPName.lastIndexOf('.');
        return aBMPName.left(suffixPos) + "packed.barch";
    }

    QString nameFromBARCHtoBMP(QString aBARCHName)
    {
        auto suffixPos = aBARCHName.lastIndexOf('.');
        return aBARCHName.left(suffixPos) + "unpacked.bmp";
    }
}

QString BMPArchiver::convert(QString aFileName)
{
    QString convertedName;
    if (isFileBARCH(aFileName))
    {
        readBARCH(aFileName);
        BARCHtoBMP();
        convertedName = nameFromBARCHtoBMP(aFileName);
        writeBMP(convertedName);
    }
    else
    {
        readBMP(aFileName);
        BMPtoBARCH();
        convertedName = nameFromBMPtoBARCH(aFileName);
        writeBARCH(convertedName);
    }
    return convertedName;
}

void BMPArchiver::readBMP(QString aFileName)
{
    mBMPImage = QImage(aFileName);
    mHeight = mBMPImage.height();
    mWidth = mBMPImage.width();
    if (mHeight <= 0 || mWidth <= 0)
        throw QString("Unsupported file");
}

void BMPArchiver::BMPtoBARCH() 
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

void BMPArchiver::writeBARCH(QString aFileName)
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

void BMPArchiver::readBARCH(QString aFileName)
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

void BMPArchiver::BARCHtoBMP() {
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
            if (row[bit++])
            {
                if (row[bit++])
                    for (int b = 0; b < blockSize; ++b)
                    {
                        unsigned char value = 0;
                        for (int i = 0; i < cBitsInByte; ++i)
                            value = (value << 1) + row[bit++];
                      mBMPData.append(value);
                    }
                else
                    mBMPData.append(blockSize, cBlack);
            }
            else
                mBMPData.append(blockSize, cWhite);
        };
        for (int block = 0; block < standardBlocksAmount; ++block)
            processBlock(cStandardBlockSize);
        if (lastBlockSize > 0)
            processBlock(lastBlockSize);
        mBMPData.append(cBMPDataTail, cWhite);
    }
}

void BMPArchiver::writeBMP(QString aFileName)
{
    mBMPImage = QImage((uchar*)mBMPData.data(), mWidth, mHeight, QImage::Format::Format_Grayscale8);
    mBMPImage.save(aFileName);
}
