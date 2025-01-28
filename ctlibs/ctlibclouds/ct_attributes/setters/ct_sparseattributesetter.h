#ifndef CT_CLOUDPARSEATTRIBUTESSETTER_H
#define CT_CLOUDPARSEATTRIBUTESSETTER_H

#include "ct_defines.h"

#include "ct_cloudindex/registered/ct_standardmodifiablecloudindexregisteredt.h"
#include "ct_cloudindex/ct_cloudindexstdmapt.h"

/**
 * @brief The CT_SparseAttributeSetter class is the setter of attributes
 *        to use when the user has created a cloud (points, faces or edges) registered (with
 *        the size defined).The difference between the "parse" and "not parse" class is that
 *        this version use a map to store only values that are set.
 */
template<typename T, typename PEF>
class CT_SparseAttributeSetter
{
public:
    template<typename PEF2>
    class ParseAttributesCollection : public CT_CloudIndexStdMapT<PEF2, T>
    {
    public:
        ParseAttributesCollection() = default;
    };

    using PtrParseAttributesCollectionType = CT_MCIR;

    CT_SparseAttributeSetter(CT_CIR cir,
                             PtrParseAttributesCollectionType attributes);

    /**
     * @brief Set the value of the attribute with a global index.
     * @param globalIndex : the global index of the attribute.
     * @param value : the value to set
     */
    void setValueWithGlobalIndex(const size_t& globalIndex, const T& value);

    /**
     * @brief Set the value of the attribute with a local index.
     * @param localIndex : the local index of the attribute. Must be between [0; cir.size()[
     * @param value : the value to set
     */
    void setValueWithLocalIndex(const size_t& localIndex, const T& value);

    /**
     * @brief Set the value of the attribute with a global index and with help of a stream (QDataStream or QTextStream per example).
     * @param globalIndex : the global index of the attribute.
     * @param stream : the stream to use to get the value
     */
    template<typename Stream>
    void setValueWithStreamWithGlobalIndex(const size_t& globalIndex, Stream& stream);

    /**
     * @brief Set the value of the attribute with a local index and with help of a stream (QDataStream or QTextStream per example).
     * @param localIndex : the local index of the attribute. Must be between [0; cir.size()[
     * @param stream : the stream to use to get the value
     */
    template<typename Stream>
    void setValueWithStreamWithLocalIndex(const size_t& localIndex, Stream& stream);

    /**
     * @brief Unset the attribute at specified global index. This will not remove the attribute but set
     *        in the bits cloud that the attribute has not been set.
     *
     * @param globalIndex : the global index of the attribute.
     */
    void unSetValueWithGlobalIndex(const size_t& globalIndex);

    /**
     * @brief Unset the attribute at specified local index. This will not remove the attribute but set
     *        in the bits cloud that the attribute has not been set.
     *
     * @param localIndex : the local index of the attribute. Must be between [0; cir.size()[
     */
    void unSetValueWithLocalIndex(const size_t& localIndex);

private:
    CT_CIR                                  mCloudRegistered;
    PtrParseAttributesCollectionType        mAttributes;
};

template<typename T, typename PEF>
CT_SparseAttributeSetter<T, PEF>::CT_SparseAttributeSetter(CT_CIR cir,
                                                           PtrParseAttributesCollectionType attributes) :
    mCloudRegistered(cir),
    mAttributes(attributes)
{
}

template<typename T, typename PEF>
void CT_SparseAttributeSetter<T, PEF>::setValueWithGlobalIndex(const size_t& globalIndex, const T& value)
{
    dynamic_cast<ParseAttributesCollection<PEF>*>(mAttributes->abstractModifiableCloudIndex())->insertIndexAndValue(globalIndex, value);
}

template<typename T, typename PEF>
void CT_SparseAttributeSetter<T, PEF>::setValueWithLocalIndex(const size_t& localIndex, const T& value)
{
    setValueWithGlobalIndex(mCloudRegistered->abstractCloudIndex()->indexAt(localIndex), value);
}

template<typename T, typename PEF>
template<typename Stream>
void CT_SparseAttributeSetter<T, PEF>::setValueWithStreamWithGlobalIndex(const size_t& globalIndex, Stream& stream)
{
    T value;
    stream >> value;
    setValueWithGlobalIndex(globalIndex, value);
}

template<typename T, typename PEF>
template<typename Stream>
void CT_SparseAttributeSetter<T, PEF>::setValueWithStreamWithLocalIndex(const size_t& localIndex, Stream& stream)
{
    setValueWithStreamWithGlobalIndex(mCloudRegistered->abstractCloudIndex()->indexAt(localIndex), stream);
}

template<typename T, typename PEF>
void CT_SparseAttributeSetter<T, PEF>::unSetValueWithGlobalIndex(const size_t& globalIndex)
{
    dynamic_cast<ParseAttributesCollection<PEF>*>(mAttributes->abstractModifiableCloudIndex())->removeIndex(globalIndex);
}

template<typename T, typename PEF>
void CT_SparseAttributeSetter<T, PEF>::unSetValueWithLocalIndex(const size_t& localIndex)
{
    unSetValueWithGlobalIndex(mCloudRegistered->abstractCloudIndex()->indexAt(localIndex));
}

#endif // CT_CLOUDPARSEATTRIBUTESSETTER_H
