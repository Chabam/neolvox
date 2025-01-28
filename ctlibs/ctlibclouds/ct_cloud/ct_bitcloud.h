#ifndef CT_BITCLOUD_H
#define CT_BITCLOUD_H

#include "ct_cloud/abstract/ct_abstractcloud.h"

/**
 * @brief The CT_BitCloud class is a cloud of flag that can be synchronized with a cloud of points, edges
 *        or faces. It is a vector of quint8 but one bit correspond to one point, edge or face. This type of
 *        cloud is useful to set a flag to a point, an edge or a face.
 *
 *        It is like a dynamic bit set.
 */
class CTLIBCLOUDS_EXPORT CT_BitCloud : virtual public CT_AbstractCloud
{
public:
    CT_BitCloud(const size_t& numberOfBits = 0);

    size_t size() const final;

    size_t memoryUsed() const final;

    CT_AbstractCloud* copy() const final;

    void set(const size_t& bitIndex, bool value);

    bool value(const size_t& bitIndex) const;

    void fill(bool value);

    void reset();

protected:
    std::vector<quint8>     m_collection;
    size_t                  m_numberOfBits;

    template<typename A, typename CLOUD> friend class CT_GlobalCloudManagerT;
    template<typename T> friend class CT_UndefinedPointCloudSizeDenseAttributeSetter;
    friend class TestClouds;

    /**
     * @brief delete all elements of the cloud between beginIndex and (beginIndex+size-1) included.
     */
    void erase(const size_t& indexOfFirstBitToErase, const size_t& numberOfBitsToErase) final;

    /**
     * @brief resize the collection
     */
    void resize(const size_t& newNumberOfBits) final;

    /**
     * @brief resize the collection
     */
    void reserve(const size_t& newNumberOfBits) final;

    void moveBitsWhenIndexOfFirstBitToEraseIsZero(quint8* dst, quint8* src, const quint8& indexOfFirstBitToMoveInFirstByteToMove, const size_t& numberOfBytesToMove);

    /**
     * @brief copy data
     */
    //void internalCopyData(const size_t &destIndex, const size_t &srcIndex, const size_t &size);
};

#endif // CT_BITCLOUD_H
