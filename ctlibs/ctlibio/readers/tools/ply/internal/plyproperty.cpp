#include "plyproperty.h"

PlyProperty::PlyProperty() : PlyProperty("",
                                         PLY_unknown)
{

}

PlyProperty::PlyProperty(const QString &propertyName,
                         const PlyProperty::Type &valuePropertyType) : PlyProperty(propertyName,
                                                                                   PLY_unknown,
                                                                                   valuePropertyType)
{

}

PlyProperty::PlyProperty(const QString &propertyName,
                         const PlyProperty::Type &listSizeType,
                         const PlyProperty::Type &valuePropertyType)
{
    m_name = propertyName;
    m_listType = listSizeType;
    m_type = valuePropertyType;
}

bool PlyProperty::isValid() const
{
    if(m_name.isEmpty() || (m_type == PLY_unknown))
        return false;

    return isASingleValue() || (m_listType != PLY_unknown);
}

bool PlyProperty::isASingleValue() const
{
    return (m_listType == PLY_unknown);
}

bool PlyProperty::isAList() const
{
    return !isASingleValue();
}

QString PlyProperty::getDisplayableName() const
{
    return m_name;
}

PlyProperty::Type PlyProperty::getValueType() const
{
    return m_type;
}

PlyProperty::Type PlyProperty::getListSizeType() const
{
    return m_listType;
}
