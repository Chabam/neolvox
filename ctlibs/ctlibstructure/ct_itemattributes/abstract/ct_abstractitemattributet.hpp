#include "ct_abstractitemattributet.h"

template <typename DataT>
CT_AbstractItemAttributeT<DataT>::CT_AbstractItemAttributeT(const CT_AbstractCategory *category) : SuperClass(category)
{
}

template <typename DataT>
CT_AbstractItemAttributeT<DataT>::CT_AbstractItemAttributeT(const QString& categoryName) : SuperClass(categoryName)
{
}

template <typename DataT>
bool CT_AbstractItemAttributeT<DataT>::toBool(const CT_AbstractItem* item, bool* ok) const
{
    Q_UNUSED(item)

    if(ok != nullptr)
        *ok = false;

    return false;
}

template <typename DataT>
double CT_AbstractItemAttributeT<DataT>::toDouble(const CT_AbstractItem* item, bool* ok) const
{
    Q_UNUSED(item)

    if(ok != nullptr)
        *ok = false;

    return 0.0;
}

template <typename DataT>
float CT_AbstractItemAttributeT<DataT>::toFloat(const CT_AbstractItem* item, bool* ok) const
{
    Q_UNUSED(item)

    if(ok != nullptr)
        *ok = false;

    return 0.0f;
}

template <typename DataT>
long double CT_AbstractItemAttributeT<DataT>::toLongDouble(const CT_AbstractItem* item, bool* ok) const
{
    Q_UNUSED(item)

    if(ok != nullptr)
        *ok = false;

    return static_cast<long double>(0.0);
}

template <typename DataT>
int CT_AbstractItemAttributeT<DataT>::toInt(const CT_AbstractItem* item, bool* ok) const
{
    Q_UNUSED(item)

    if(ok != nullptr)
        *ok = false;

    return 0;
}

template <typename DataT>
quint64 CT_AbstractItemAttributeT<DataT>::toUInt64(const CT_AbstractItem* item, bool* ok) const
{
    Q_UNUSED(item)

    if(ok != nullptr)
        *ok = false;

    return 0;
}

template <typename DataT>
size_t CT_AbstractItemAttributeT<DataT>::toSizeT(const CT_AbstractItem* item, bool* ok) const
{
    Q_UNUSED(item)

    if(ok != nullptr)
        *ok = false;

    return 0;
}

template <typename DataT>
QString CT_AbstractItemAttributeT<DataT>::toString(const CT_AbstractItem* item, bool* ok) const
{
    if(ok != nullptr)
        *ok = true;

    return CT_NumericToStringConversionT<DataT>::toString(data(item));
}

template <typename DataT>
CT_AbstractCategory::ValueType CT_AbstractItemAttributeT<DataT>::valueType() const
{
    return CT_AbstractCategory::staticValueTypeToCategoryType<DataT>();
}

template <typename DataT>
QString CT_AbstractItemAttributeT<DataT>::valueTypeToString() const
{
    return CT_AbstractCategory::staticValueTypeToCategoryTypeString<DataT>();
}
