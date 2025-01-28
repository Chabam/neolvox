#ifndef PLYELEMENT_H
#define PLYELEMENT_H

#include <QList>

#include "plyproperty.h"

/**
 * @brief Represent an element readed in a ply file
 */
class PlyElement {
public:
    PlyElement();

    /**
     * @brief Construct an element
     * @param elementName : name of the element (element [Vertex] 10)
     * @param numberOfElements : number of element (element Vertex [10])
     */
    PlyElement(const QString& elementName,
               const size_t& numberOfElements);

    /**
     * @brief Returns true if the type is not unknown and the size is > 0 and all properties is valid
     */
    bool isValid() const;

    /**
     * @brief Returns the type of this element
     */
    PlyElementType getType() const;

    /**
     * @brief Returns the displayable name of this element
     */
    QString getDisplayableName() const;

    /**
     * @brief Returns the number of elements that you must read in the file
     */
    size_t getNumberOfElements() const;

    /**
     * @brief Add a new property to this element
     */
    void addProperty(const PlyProperty& newProperty);

    /**
     * @brief Returns all properties (in the order they were added)
     */
    QList<PlyProperty> getProperties() const;

private:
    QString             m_name;
    size_t              m_size;
    QList<PlyProperty>  m_properties;
};

#endif // PLYELEMENT_H
