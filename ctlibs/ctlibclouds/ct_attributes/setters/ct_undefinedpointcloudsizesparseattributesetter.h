#ifndef CT_UNDEFINEDPOINTCLOUDSIZESPARSEATTRIBUTESETTER_H
#define CT_UNDEFINEDPOINTCLOUDSIZESPARSEATTRIBUTESETTER_H

#include "ct_cloudindex/registered/ct_standardmodifiablecloudindexregisteredt.h"
#include "ct_cloudindex/ct_cloudindexstdmapt.h"
#include "ct_pointcloud/tools/ct_standardundefinedsizepointcloud.h"

/**
 * @brief The CT_UndefinedPointCloudSizeSparseAttributeSetter class is the setter of attributes
 *        to use when the user has created a point cloud with an undefined size. The difference
 *        between the "parse" and "not parse" class is that this version use a map to store only
 *        values that are set.
 */
template<typename T>
class CT_UndefinedPointCloudSizeSparseAttributeSetter
{
public:
    using ParseAttributesCollectionType = CT_MCIR;

    /**
     * @brief Create a setter that must use the "undefined size point cloud" object to use to know where is the last value
     *        and limits of local indexes, the parse cloud of attributes to use.
     *
     * @param uspc : the "undefined size point cloud" object created by the user
     * @param attributes : the attributes cloud created by the manager
     */
    CT_UndefinedPointCloudSizeSparseAttributeSetter(CT_AbstractUndefinedSizePointCloud& uspc,
                                                    ParseAttributesCollectionType attributes);

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
     * @brief Unset the attribute at specified local index. This will remove the attribute.
     *
     * @param localIndex : the local index of the attribute. Must be between [uspc.beginIndex(); uspc.endIndex()].
     */
    void unSetValueWithLocalIndex(const size_t& localIndex);

    /**
     * @brief Unset the value of the last attribute (attribute at uspc.endIndex()). This will remove the attribute.
     * @param value : the value to set
     */
    void unSetLastValue();

private:
    CT_AbstractUndefinedSizePointCloud&         mUSPC;
    ParseAttributesCollectionType               mAttributes;

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
     * @brief Unset the value of the attribute with a global index. This will remove the attribute.
     * @param globalIndex : the global index of the attribute.
     */
    void internalUnSetValueWithGlobalIndex(const size_t& globalIndex);
};

template<typename T>
CT_UndefinedPointCloudSizeSparseAttributeSetter<T>::CT_UndefinedPointCloudSizeSparseAttributeSetter(CT_AbstractUndefinedSizePointCloud& uspc,
                                                                                                    ParseAttributesCollectionType attributes):
    mUSPC(uspc),
    mAttributes(attributes)
{
}

template<typename T>
void CT_UndefinedPointCloudSizeSparseAttributeSetter<T>::setValueWithLocalIndex(const size_t& localIndex, const T& value)
{
    internalSetValueWithGlobalIndex(mUSPC.beginIndex() + localIndex, value);
}

template<typename T>
void CT_UndefinedPointCloudSizeSparseAttributeSetter<T>::setLastValue(const T& value)
{
    internalSetValueWithGlobalIndex(mUSPC.lastIndex(), value);
}

template<typename T>
template<typename Stream>
void CT_UndefinedPointCloudSizeSparseAttributeSetter<T>::setValueWithStreamWithLocalIndex(const size_t& localIndex, Stream& stream)
{
    internalSetValueWithStreamWithGlobalIndex(mUSPC.beginIndex() + localIndex, stream);
}

template<typename T>
template<typename Stream>
void CT_UndefinedPointCloudSizeSparseAttributeSetter<T>::setLastValueWithStream(Stream& stream)
{
    internalSetValueWithStreamWithGlobalIndex(mUSPC.lastIndex(), stream);
}

template<typename T>
void CT_UndefinedPointCloudSizeSparseAttributeSetter<T>::unSetValueWithLocalIndex(const size_t& localIndex)
{
    internalUnSetValueWithGlobalIndex(mUSPC.beginIndex() + localIndex);
}

template<typename T>
void CT_UndefinedPointCloudSizeSparseAttributeSetter<T>::unSetLastValue()
{
    internalUnSetValueWithGlobalIndex(mUSPC.lastIndex());
}

template<typename T>
void CT_UndefinedPointCloudSizeSparseAttributeSetter<T>::internalSetValueWithGlobalIndex(const size_t& globalIndex, const T& value)
{
    dynamic_cast<CT_CloudIndexStdMapT<CT_PointData, T>*>(mAttributes->abstractModifiableCloudIndex())->insertIndexAndValue(globalIndex, value);
}

template<typename T>
template<typename Stream>
void CT_UndefinedPointCloudSizeSparseAttributeSetter<T>::internalSetValueWithStreamWithGlobalIndex(const size_t& globalIndex, Stream& stream)
{
    T value;
    stream >> value;

    internalSetValueWithGlobalIndex(globalIndex, value);
}

template<typename T>
void CT_UndefinedPointCloudSizeSparseAttributeSetter<T>::internalUnSetValueWithGlobalIndex(const size_t& globalIndex)
{
    dynamic_cast<CT_CloudIndexStdMapT<CT_PointData, T>*>(mAttributes->abstractModifiableCloudIndex())->removeIndex(globalIndex);
}

#endif // CT_UNDEFINEDPOINTCLOUDSIZESPARSEATTRIBUTESETTER_H
