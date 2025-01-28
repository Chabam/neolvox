#ifndef CT_ABSTRACTITEMATTRIBUTET_H
#define CT_ABSTRACTITEMATTRIBUTET_H

#include "ct_itemattributes/abstract/ct_abstractitemattribute.h"
#include "ct_categories/abstract/ct_abstractcategory.h"
#include "ct_tools/ct_numerictostringconversiont.h"

/**
 * @brief Base class for item attribute with a specified type (DataT) at compilation time
 */
template <typename DataT>
class CT_AbstractItemAttributeT : public CT_AbstractItemAttribute
{
    using SuperClass = CT_AbstractItemAttribute;

public:
    using DataType = DataT;

    /**
     * @brief Constructor for model
     */
    CT_AbstractItemAttributeT() = default;

    /**
     * @brief Create an attribute with a category.
     * @param category : the category of the item attribute. Must not be nullptr !
     */
    CT_AbstractItemAttributeT(const CT_AbstractCategory *category);
    CT_AbstractItemAttributeT(const QString& categoryName);

    /**
     * @brief Copy contructor
     */
    CT_AbstractItemAttributeT(const CT_AbstractItemAttributeT<DataT>& other) = default;

    /**
      * @brief Type of value
      */
    CT_AbstractCategory::ValueType valueType() const override;

    /**
      * @brief Returns the type of the value in QString
      */
    QString valueTypeToString() const override;

    /**
     * @brief ok = false if the specialization does not exist
     */
    bool toBool(const CT_AbstractItem* item, bool* ok) const override;

    /**
     * @brief ok = false if the specialization does not exist
     */
    double toDouble(const CT_AbstractItem* item, bool* ok) const override;

    /**
     * @brief ok = false if the specialization does not exist
     */
    float toFloat(const CT_AbstractItem* item, bool* ok) const override;

    /**
     * @brief ok = false if the specialization does not exist
     */
    long double toLongDouble(const CT_AbstractItem* item, bool* ok) const override;

    /**
     * @brief ok = false if the specialization does not exist
     */
    int toInt(const CT_AbstractItem* item, bool* ok) const override;

    /**
     * @brief ok = false if the specialization does not exist
     */
    quint64 toUInt64(const CT_AbstractItem* item, bool* ok) const override;

    /**
     * @brief ok = false if the specialization does not exist
     */
    size_t toSizeT(const CT_AbstractItem* item, bool* ok) const override;

    /**
     * @brief ok = true in all case and the value is converted to a QString.
     */
    virtual QString toString(const CT_AbstractItem* item, bool* ok) const override;

    /**
     * @brief Returns the data
     */
    virtual DataT data(const CT_AbstractItem* item) const = 0;
};

//specialization to convert a type to another at compilation time

// BOOLEAN -> BOOLEAN
template<>
inline bool CT_AbstractItemAttributeT<bool>::toBool(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }


// LONG DOUBLE -> LONG DOUBLE
template<>
inline long double CT_AbstractItemAttributeT<long double>::toLongDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }


// DOUBLE -> DOUBLE
template<>
inline double CT_AbstractItemAttributeT<double>::toDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// DOUBLE -> LONG DOUBLE
template<>
inline long double CT_AbstractItemAttributeT<double>::toLongDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }


// FLOAT -> FLOAT
template<>
inline float CT_AbstractItemAttributeT<float>::toFloat(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// FLOAT -> DOUBLE
template<>
inline double CT_AbstractItemAttributeT<float>::toDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// FLOAT -> LONG DOUBLE
template<>
inline long double CT_AbstractItemAttributeT<float>::toLongDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }


// SIGNED INTEGER -> SIGNED INTEGER
template<>
inline int CT_AbstractItemAttributeT<int>::toInt(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// SIGNED INTEGER -> FLOAT
template<>
inline float CT_AbstractItemAttributeT<int>::toFloat(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// SIGNED INTEGER -> DOUBLE
template<>
inline double CT_AbstractItemAttributeT<int>::toDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// SIGNED INTEGER -> LONG DOUBLE
template<>
inline long double CT_AbstractItemAttributeT<int>::toLongDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }


// UNSIGNED INTEGER 8bits -> UNSIGNED INTEGER 64bits
template<>
inline quint64 CT_AbstractItemAttributeT<quint8>::toUInt64(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 8bits -> FLOAT
template<>
inline float CT_AbstractItemAttributeT<quint8>::toFloat(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 8bits -> DOUBLE
template<>
inline double CT_AbstractItemAttributeT<quint8>::toDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 8bits -> LONG DOUBLE
template<>
inline long double CT_AbstractItemAttributeT<quint8>::toLongDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }


// UNSIGNED INTEGER 16bits -> UNSIGNED INTEGER 64bits
template<>
inline quint64 CT_AbstractItemAttributeT<quint16>::toUInt64(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 16bits -> FLOAT
template<>
inline float CT_AbstractItemAttributeT<quint16>::toFloat(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 16bits -> DOUBLE
template<>
inline double CT_AbstractItemAttributeT<quint16>::toDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 16bits -> LONG DOUBLE
template<>
inline long double CT_AbstractItemAttributeT<quint16>::toLongDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }


// UNSIGNED INTEGER 32bits -> UNSIGNED INTEGER 64bits
template<>
inline quint64 CT_AbstractItemAttributeT<quint32>::toUInt64(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 32bits -> FLOAT
template<>
inline float CT_AbstractItemAttributeT<quint32>::toFloat(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 32bits -> DOUBLE
template<>
inline double CT_AbstractItemAttributeT<quint32>::toDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 32bits -> LONG DOUBLE
template<>
inline long double CT_AbstractItemAttributeT<quint32>::toLongDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }


// UNSIGNED INTEGER 64bits -> UNSIGNED INTEGER 64bits
template<>
inline quint64 CT_AbstractItemAttributeT<quint64>::toUInt64(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 64bits -> FLOAT
template<>
inline float CT_AbstractItemAttributeT<quint64>::toFloat(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 64bits -> DOUBLE
template<>
inline double CT_AbstractItemAttributeT<quint64>::toDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// UNSIGNED INTEGER 64bits -> LONG DOUBLE
template<>
inline long double CT_AbstractItemAttributeT<quint64>::toLongDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }


// SIZE_T -> SIZE_T
template<>
inline size_t CT_AbstractItemAttributeT<size_t>::toSizeT(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

#if !(defined(__x86_64__) || _M_X64) // Definition for GCC, Clang, and Intel's compiler or MSVC++
// SIZE_T -> FLOAT
template<>
inline float CT_AbstractItemAttributeT<size_t>::toFloat(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// SIZE_T -> DOUBLE
template<>
inline double CT_AbstractItemAttributeT<size_t>::toDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

// SIZE_T -> LONG DOUBLE
template<>
inline long double CT_AbstractItemAttributeT<size_t>::toLongDouble(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }
#endif

// QSTRING -> QSTRING
template<>
inline QString CT_AbstractItemAttributeT<QString>::toString(const CT_AbstractItem* item, bool* ok) const { if(ok != nullptr) { *ok = true; } return data(item); }

#include "ct_abstractitemattributet.hpp"

#endif // CT_ABSTRACTITEMATTRIBUTET_H
