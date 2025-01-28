#include "ct_bitcloud.h"
#include <cmath>

#include <cmath>
#include <QDebug>

#define TO_NUMBER_OF_BYTES(V) size_t(std::ceil(double(V)/8.0))

CT_BitCloud::CT_BitCloud(const size_t& numberOfBits) :
    m_numberOfBits(numberOfBits)
{
    if(numberOfBits == 0)
        return;

    m_collection.resize(TO_NUMBER_OF_BYTES(numberOfBits), 0);
}

size_t CT_BitCloud::size() const
{
    return m_numberOfBits;
}

size_t CT_BitCloud::memoryUsed() const
{
    return m_collection.size() * sizeof(quint8);
}

CT_AbstractCloud* CT_BitCloud::copy() const
{
    // TODO
    return nullptr;
}

void CT_BitCloud::set(const size_t& bitIndex, bool value)
{
    const size_t indexOfByte = bitIndex / 8;

    Q_ASSERT(indexOfByte < m_collection.size());
    if (indexOfByte >= m_collection.size()) {qDebug() << "CT_BitCloud::set" << ", " <<  "indexOfByte >= m_collection.size()"; return;}

    const quint8 indexOfBitInByte = bitIndex % 8;

    if(value)
        m_collection[indexOfByte] |= (1 << indexOfBitInByte);
    else
        m_collection[indexOfByte] &= ~(1 << indexOfBitInByte);
}

bool CT_BitCloud::value(const size_t& bitIndex) const
{
    const size_t indexOfByte = bitIndex / 8;

    Q_ASSERT(indexOfByte < m_collection.size());
    if (indexOfByte >= m_collection.size()) {qDebug() << "CT_BitCloud::value" << ", " <<  "indexOfByte >= m_collection.size()"; return false;}

    const quint8 indexOfBitInByte = bitIndex % 8;

    return m_collection[indexOfByte] & (1 << indexOfBitInByte);
}

void CT_BitCloud::fill(bool value)
{
    static const std::function<void(quint8&)> funcClear = [](quint8& val) { val = 0; };
    static const std::function<void(quint8&)> funcSet = [](quint8& val) { val = 0xFF; };

    if(value)
        std::for_each(m_collection.begin(), m_collection.end(), funcSet);
    else
        std::for_each(m_collection.begin(), m_collection.end(), funcClear);
}

void CT_BitCloud::reset()
{
    fill(false);
}

void CT_BitCloud::erase(const size_t& indexOfFirstBitToErase, const size_t& numberOfBitsToErase)
{
    if(indexOfFirstBitToErase >= size())
        return;

    const size_t finalNumberOfBitsToErase = std::min(numberOfBitsToErase, size() - indexOfFirstBitToErase);

    if(finalNumberOfBitsToErase == 0)
        return;

    const size_t indexOfFirstBitToMove = std::min(indexOfFirstBitToErase + finalNumberOfBitsToErase, size());
    const size_t numberOfBitsToMove = size() - indexOfFirstBitToMove;

    if(numberOfBitsToMove > 0)
    {
        const size_t indexOfFirstByteToErase = indexOfFirstBitToErase / 8;
        const quint8 indexOfFirstBitToEraseInFirstByteToErase = indexOfFirstBitToErase % 8;

        const size_t indexOfFirstByteToMove = indexOfFirstBitToMove / 8;
        const quint8 indexOfFirstBitToMoveInFirstByteToMove = indexOfFirstBitToMove % 8;

        quint8* data = m_collection.data();
        quint8* dst = data + indexOfFirstByteToErase;
        quint8* src = data + indexOfFirstByteToMove;

        if(indexOfFirstBitToEraseInFirstByteToErase == 0)
        {
            if(indexOfFirstBitToMoveInFirstByteToMove == 0)
            {
                memcpy(dst, src, numberOfBitsToMove / 8);
            }
            else
            {
                const size_t indexOfLastByteToMove = (size() - 1) / 8;
                const size_t numberOfBytesToMove = indexOfLastByteToMove - indexOfFirstByteToMove;
                moveBitsWhenIndexOfFirstBitToEraseIsZero(dst, src, indexOfFirstBitToMoveInFirstByteToMove, numberOfBytesToMove);
            }
        }
        else
        {
            const size_t indexOfLastByteToMove = (size() - 1) / 8;
            size_t numberOfBytesToMove = indexOfLastByteToMove - indexOfFirstByteToMove;

            const quint8 dstExtract = (dst[0] & (0xFF >> (8-indexOfFirstBitToEraseInFirstByteToErase)));
            const quint8 srcExtract = (src[0] & quint8(0xFF << indexOfFirstBitToMoveInFirstByteToMove));

            if(indexOfFirstBitToMoveInFirstByteToMove >= indexOfFirstBitToEraseInFirstByteToErase)
            {
                // missing bits
                const quint8 srcShiftOffsetAndNumberOfMissingBitsForDst = quint8(indexOfFirstBitToMoveInFirstByteToMove - indexOfFirstBitToEraseInFirstByteToErase);

                dst[0] = dstExtract | (srcExtract >> srcShiftOffsetAndNumberOfMissingBitsForDst);

                if(numberOfBytesToMove > 0)
                {
                    if(srcShiftOffsetAndNumberOfMissingBitsForDst == 0)
                    {
                        ++dst;
                        ++src;
                        memcpy(dst, src, numberOfBitsToMove / 8);
                    }
                    else
                    {
                        const quint8 nextSrcShiftOffsetAndNumberOfBitsToExtract = (8 - srcShiftOffsetAndNumberOfMissingBitsForDst);

                        dst[0] = dst[0] | quint8(quint8(src[1] & (0xFF >> nextSrcShiftOffsetAndNumberOfBitsToExtract)) << nextSrcShiftOffsetAndNumberOfBitsToExtract);

                        ++dst;
                        ++src;
                        --numberOfBytesToMove;

                        moveBitsWhenIndexOfFirstBitToEraseIsZero(dst, src, srcShiftOffsetAndNumberOfMissingBitsForDst, numberOfBytesToMove);
                    }
                }
            }
            else
            {
                const quint8 srcShiftOffset = quint8(indexOfFirstBitToEraseInFirstByteToErase - indexOfFirstBitToMoveInFirstByteToMove);
                const quint8 indexOfFirstBitToMoveInNextByteToMove = (indexOfFirstBitToMoveInFirstByteToMove + (8 - indexOfFirstBitToEraseInFirstByteToErase));

                // extra bits
                dst[0] = dstExtract | quint8(srcExtract << srcShiftOffset);

                ++dst;

                moveBitsWhenIndexOfFirstBitToEraseIsZero(dst, src, indexOfFirstBitToMoveInNextByteToMove, numberOfBytesToMove);
            }
        }
    }
    else
    {
        const quint8 indexOfFirstBitToEraseInFirstByteToErase = indexOfFirstBitToErase % 8;

        if(indexOfFirstBitToEraseInFirstByteToErase > 0)
        {
            const size_t indexOfFirstByteToErase = indexOfFirstBitToErase / 8;

            quint8& dst = m_collection[indexOfFirstByteToErase];
            dst = (dst & (0xFF >> (8-indexOfFirstBitToEraseInFirstByteToErase)));
        }
    }

    resize(size() - std::min(finalNumberOfBitsToErase, size()));
}

void CT_BitCloud::resize(const size_t& newNumberOfBits)
{
    const size_t numberOfBytes = TO_NUMBER_OF_BYTES(newNumberOfBits);

    m_numberOfBits = newNumberOfBits;

    if(numberOfBytes == m_collection.size())
        return;

    m_collection.resize(numberOfBytes, 0);
    m_collection.shrink_to_fit();
}

void CT_BitCloud::reserve(const size_t& /*newNumberOfBits*/)
{
    qFatal("CT_BitCloud::reserve can not be used");
}

void CT_BitCloud::moveBitsWhenIndexOfFirstBitToEraseIsZero(quint8* dst, quint8* src, const quint8& indexOfFirstBitToMoveInFirstByteToMove, const size_t& numberOfBytesToMove)
{
    size_t i = 0;
    for(; i<numberOfBytesToMove; ++i)
    {
        dst[i] = quint8(quint8(src[i] >> indexOfFirstBitToMoveInFirstByteToMove) | quint8(src[i+1] << (8 - indexOfFirstBitToMoveInFirstByteToMove)));
    }

    dst[i] = quint8(src[i] >> indexOfFirstBitToMoveInFirstByteToMove);
}
