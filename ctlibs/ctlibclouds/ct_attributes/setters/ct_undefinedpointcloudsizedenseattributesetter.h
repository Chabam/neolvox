#ifndef CT_UNDEFINEDPOINTCLOUDSIZEDENSEATTRIBUTESETTER_H
#define CT_UNDEFINEDPOINTCLOUDSIZEDENSEATTRIBUTESETTER_H

#include "ct_cloud/registered/ct_stdcloudregisteredt.h"
#include "ct_cloud/ct_standardcloudstdvectort.h"
#include "ct_cloud/ct_bitcloud.h"
#include "ct_pointcloud/tools/ct_standardundefinedsizepointcloud.h"

/**
 * @brief The CT_UndefinedPointCloudSizeDenseAttributeSetter class is the setter of attributes
 *        to use when the user has created a point cloud with an undefined size.
 */
template<typename T>
class CT_UndefinedPointCloudSizeDenseAttributeSetter
{
public:
    using PtrAttributesCollectionType = QSharedPointer<CT_StdCloudRegisteredT<CT_StandardCloudStdVectorT<T>>>;
    using PtrAttributeBitCollectionType = QSharedPointer<CT_StdCloudRegisteredT<CT_BitCloud>>;

    /**
     * @brief Create a setter that must use the "undefined size point cloud" object to use to know where is the last value
     *        and limits of local indexes, the cloud of attributes to use and the cloud of bits to use to set if an
     *        attributes has been set or not.
     * @param uspc : the "undefined size point cloud" object created by the user
     * @param attributes : the attributes cloud created by the manager
     * @param exists : the bits cloud created by the manager
     */
    CT_UndefinedPointCloudSizeDenseAttributeSetter(CT_AbstractUndefinedSizePointCloud& uspc,
                                                   PtrAttributesCollectionType attributes,
                                                   PtrAttributeBitCollectionType exists);

    /**
     * @brief Set the value of the attribute with a local index.
     * @param localIndex : the local index of the attribute. Must be between [uspc.beginIndex(); uspc.endIndex()].
     * @param value : the value to set
     */
    void setValueWithLocalIndex(const size_t& localIndex, const T& value);

    /**
     * @brief Set the value of the last attribute (attribute at uspc.endIndex())
     * @param value : the value to set
     */
    void setLastValue(const T& value);

    /**
     * @brief Set the value of the attribute with a local index and with help of a stream (QDataStream or QTextStream per example).
     * @param localIndex : the local index of the attribute. Must be between [0; cir.size()[
     * @param stream : the stream to use to get the value
     */
    template<typename Stream>
    void setValueWithStreamWithLocalIndex(const size_t& localIndex, Stream& stream);

    /**
     * @brief Set the value of the last attribute (attribute at uspc.endIndex()) with help of a stream (QDataStream or QTextStream per example).
     * @param stream : the stream to use to get the value
     */
    template<typename Stream>
    void setLastValueWithStream(Stream& stream);

    /**
     * @brief Unset the attribute at specified local index. This will not remove the attribute but set
     *        in the bits cloud that the attribute has not been set.
     *
     * @param localIndex : the local index of the attribute. Must be between [uspc.beginIndex(); uspc.endIndex()].
     */
    void unSetValueWithLocalIndex(const size_t& localIndex);

    /**
     * @brief Unset the value of the last attribute (attribute at uspc.endIndex()). This will not remove the attribute but set
     *        in the bits cloud that the attribute has not been set.
     * @param value : the value to set
     */
    void unSetLastValue();

private:
    CT_AbstractUndefinedSizePointCloud&     mUSPC;
    PtrAttributesCollectionType             mAttributes;
    PtrAttributeBitCollectionType           mBits;
    const int                               mResizeOptimValue;

    /**
     * @brief Set the value of the attribute with a global index.
     * @param globalIndex : the global index of the attribute.
     * @param value : the value to set
     */
    void internalSetValueWithGlobalIndex(const size_t& globalIndex, const T& value);

    /**
     * @brief Set the value of the attribute with a global index and with the help of a stream.
     * @param globalIndex : the global index of the attribute.
     * @param stream : the stream to use
     */
    template<typename Stream>
    void internalSetValueWithStreamWithGlobalIndex(const size_t& globalIndex, Stream& stream);

    /**
     * @brief Unset the value of the attribute with a global index. This will not remove the attribute but set
     *        in the bits cloud that the attribute has not been set.
     * @param globalIndex : the global index of the attribute.
     */
    void internalUnSetValueWithGlobalIndex(const size_t& globalIndex);
};

template<typename T>
CT_UndefinedPointCloudSizeDenseAttributeSetter<T>::CT_UndefinedPointCloudSizeDenseAttributeSetter(CT_AbstractUndefinedSizePointCloud& uspc,
                                                                                                  PtrAttributesCollectionType attributes,
                                                                                                  PtrAttributeBitCollectionType exists) :
    mUSPC(uspc),
    mAttributes(attributes),
    mBits(exists),
    mResizeOptimValue(1000)
{
}


template<typename T>
void CT_UndefinedPointCloudSizeDenseAttributeSetter<T>::setValueWithLocalIndex(const size_t& localIndex, const T& value)
{
    internalSetValueWithGlobalIndex(mUSPC.beginIndex() + localIndex, value);
}

template<typename T>
void CT_UndefinedPointCloudSizeDenseAttributeSetter<T>::setLastValue(const T& value)
{
    internalSetValueWithGlobalIndex(mUSPC.lastIndex(), value);
}

template<typename T>
template<typename Stream>
void CT_UndefinedPointCloudSizeDenseAttributeSetter<T>::setValueWithStreamWithLocalIndex(const size_t& localIndex, Stream& stream)
{
    internalSetValueWithStreamWithGlobalIndex(mUSPC.beginIndex() + localIndex, stream);
}

template<typename T>
template<typename Stream>
void CT_UndefinedPointCloudSizeDenseAttributeSetter<T>::setLastValueWithStream(Stream& stream)
{
    internalSetValueWithStreamWithGlobalIndex(mUSPC.lastIndex(), stream);
}

template<typename T>
void CT_UndefinedPointCloudSizeDenseAttributeSetter<T>::unSetValueWithLocalIndex(const size_t& localIndex)
{
    internalUnSetValueWithGlobalIndex(mUSPC.beginIndex() + localIndex);
}

template<typename T>
void CT_UndefinedPointCloudSizeDenseAttributeSetter<T>::unSetLastValue()
{
    internalUnSetValueWithGlobalIndex(mUSPC.lastIndex());
}

template<typename T>
void CT_UndefinedPointCloudSizeDenseAttributeSetter<T>::internalSetValueWithGlobalIndex(const size_t& globalIndex, const T& value)
{
    const size_t totalSize = mUSPC.beginIndex() + mUSPC.size();

    if(totalSize == 0)
        return;

    if(mAttributes->cloudT()->size() < totalSize)
    {
        mAttributes->cloudT()->resize(totalSize + mResizeOptimValue);
        mBits->cloudT()->resize(totalSize + mResizeOptimValue);
    }

    (*mAttributes->cloudT())[globalIndex] = value;
    (*mBits->cloudT()).set(globalIndex, true);
}

template<typename T>
template<typename Stream>
void CT_UndefinedPointCloudSizeDenseAttributeSetter<T>::internalSetValueWithStreamWithGlobalIndex(const size_t& globalIndex, Stream& stream)
{
    const size_t totalSize = mUSPC.beginIndex() + mUSPC.size();

    if(totalSize == 0)
        return;

    if(mAttributes->cloudT()->size() < totalSize)
    {
        mAttributes->cloudT()->resize(totalSize + mResizeOptimValue);
        mBits->cloudT()->resize(totalSize + mResizeOptimValue);
    }

    stream >> (*mAttributes->cloudT())[globalIndex];
    (*mBits->cloudT()).set(globalIndex, true);
}

template<typename T>
void CT_UndefinedPointCloudSizeDenseAttributeSetter<T>::internalUnSetValueWithGlobalIndex(const size_t& globalIndex)
{
    if(globalIndex >= mBits->cloudT()->size())
        return;

    (*mBits->cloudT()).set(globalIndex, false);
}

#endif // CT_UNDEFINEDPOINTCLOUDSIZEDENSEATTRIBUTESETTER_H
