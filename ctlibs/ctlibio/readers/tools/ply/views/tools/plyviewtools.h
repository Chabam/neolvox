#ifndef PLYVIEWTOOLS_H
#define PLYVIEWTOOLS_H

#include <QComboBox>
#include <functional>

#include "readers/tools/ply/plyheader.h"
#include "readers/tools/ply/internal/plytype.h"

/**
 * @brief Tool class for fill combobox, etc... in ply views
 */
class PlyViewTools
{
public:

    /**
     * @brief Fill a combobox with all elements in the header
     */
    static void staticFillElementsInComboBox(QComboBox* cb, const PlyHeader& header) {
        QList<PlyElement> elements = header.getElements();

        int elementIndex = 0;
        foreach (const PlyElement& element, elements) {
            cb->addItem(element.getDisplayableName(), elementIndex++);
        }
    }

    /**
     * @brief Fill a combobox with all properties in the element
     */
    static void staticFillPropertiesInComboBox(QComboBox* cb, const PlyElement& element) {
        QList<PlyProperty> properties = element.getProperties();

        int propertyIndex = 0;
        foreach (const PlyProperty& property, properties) {
            if(property.isASingleValue())
                cb->addItem(property.getDisplayableName() + " [" + PlyTypeConverter::staticPlyTypeToDisplayableString(property.getValueType()) + "]", propertyIndex++);
            else
                cb->addItem(property.getDisplayableName() + " (List) [" + PlyTypeConverter::staticPlyTypeToDisplayableString(property.getValueType()) + "]", propertyIndex++);
        }
    }

    /**
     * @brief Use a function to get a score (int) for a property of the element. The
     *        property that have the best score is selected in the combobox.
     */
    static void staticFindPropertyAndSetCurrentIndexInComboBox(QComboBox* cb,
                                                               const PlyElement& element,
                                                               std::function<int (const PlyProperty& property)> criteriaFunc) {
        QList<PlyProperty> properties = element.getProperties();

        int maxMatch = 0;
        int maxPropertyIndexMatch = -1;

        int propertyIndex = 0;
        foreach (const PlyProperty& property, properties) {
            const int m = criteriaFunc(property);

            if(m > maxMatch) {
                maxMatch = m;
                maxPropertyIndexMatch = propertyIndex;
            }

            ++propertyIndex;
        }

        cb->setCurrentIndex(cb->findData(maxPropertyIndexMatch));
    }
};

#endif // PLYVIEWTOOLS_H
