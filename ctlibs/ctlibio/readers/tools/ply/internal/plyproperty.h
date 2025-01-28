#ifndef PLYPROPERTY_H
#define PLYPROPERTY_H

#include <QString>

#include "plytype.h"

/**
 * @brief Represent a property of an element readed in a ply file
 */
class PlyProperty {

public:
    typedef PlyType Type;

    PlyProperty();

    /**
     * @brief Construct a single property
     * @param propertyName : name defined in the ply file
     * @param valuePropertyType : type of value defined in the ply file
     */
    PlyProperty(const QString& propertyName, const Type& valuePropertyType);

    /**
     * @brief Construct a list property
     * @param propertyName : name defined in the ply file
     * @param listSizeType : type of the value that indicate the list size defined in the ply file (property list [uchar] int XXXX)
     * @param valuePropertyType : type of values defined in the ply file (property list uchar [int] XXXX)
     */
    PlyProperty(const QString& propertyName, const Type& listSizeType, const Type& valuePropertyType);

    /**
     * @brief Returns true if the property has not unknown type and a not empty name
     */
    bool isValid() const;

    /**
     * @brief Returns true if this property is not a list but a single value
     */
    bool isASingleValue() const;

    /**
     * @brief Returns true if this property is a list
     */
    bool isAList() const;

    /**
     * @brief Returns the displayable name of this property
     */
    QString getDisplayableName() const;

    /**
     * @brief Returns the type of value(s)
     */
    Type getValueType() const;

    /**
     * @brief Returns the type of the value that indicate the list size (if this property is a list)
     */
    Type getListSizeType() const;

private:
    QString m_name;
    Type    m_listType;
    Type    m_type;
};

#endif // PLYPROPERTY_H
