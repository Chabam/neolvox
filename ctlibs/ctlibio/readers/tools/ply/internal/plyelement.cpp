#include "plyelement.h"

PlyElement::PlyElement() : PlyElement("", 0)
{

}

PlyElement::PlyElement(const QString &elementName, const size_t &numberOfElements)
{
    m_name = elementName;
    m_size = numberOfElements;
}

bool PlyElement::isValid() const
{
    if(getType() == PLY_unknown_element_type)
        return false;

    foreach (const PlyProperty& p, m_properties) {
        if(!p.isValid())
            return false;
    }

    return !m_properties.isEmpty();
}

PlyElementType PlyElement::getType() const
{
    return PlyTypeConverter::staticStringToPlyElementType(m_name);
}

QString PlyElement::getDisplayableName() const
{
    return m_name;
}

size_t PlyElement::getNumberOfElements() const
{
    return m_size;
}

void PlyElement::addProperty(const PlyProperty &newProperty)
{
    m_properties.append(newProperty);
}

QList<PlyProperty> PlyElement::getProperties() const
{
    return m_properties;
}
