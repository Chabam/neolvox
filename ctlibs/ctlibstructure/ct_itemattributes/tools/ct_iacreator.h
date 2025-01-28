#ifndef CT_IACREATOR_H
#define CT_IACREATOR_H

#include "ct_itemattributes/ct_stditemattributewrappert.h"
#include "ct_itemattributes/ct_stditemattributet.h"

/**
 * @brief A tools to create an item attribute according to what your
 *        data type : a const data or a pointer to a member method of your class
 */
class CTLIBSTRUCTURE_EXPORT CT_IACreator
{
public:

    /************* WRAPPER TO MEMBER METHOD OF YOUR CLASS ********************/

    /**
     * @brief Create a item attribute that can use a method of your class to get a data.
     * @param category : the category for the attribute (must not be nullptr)
     * @param gMethod : the method of the itemdrawable to call to get the data.
     *
     * @example CT_AbstractItemAttribute* attribute = CT_IACreator::create(category, &CT_Circle::getRadius);
     *          circle->addAttribute(attribute);
     */
    template <typename ClassType, typename VType>
    static CT_AbstractItemAttribute* create(const CT_AbstractCategory* category, typename CT_StdItemAttributeWrapperT<ClassType, VType>::getter gMethod)
    {
        return new CT_StdItemAttributeWrapperT<ClassType, VType>(category, gMethod);
    }

    /*********************** CONST DATA *********************/

    /**
     * @brief Create a item attribute with a default data.
     * @param model : the model of the attribute (can be nullptr but must be set before finish the step computing by the method setModel(...) )
     * @param category : the category for the attribute (must not be nullptr)
     * @param result : the result where the attribute will be stocked (can be nullptr but must be set before finish the step computing by the method setResult(...) )
     * @param data : the data that will never change
     *
     * @example CT_AbstractItemAttribute *attribute = CT_IACreator::create(category, 5.0f);
     *          item->addAttribute(attribute);
     */
    template <typename VType>
    static CT_AbstractItemAttribute* create(const CT_AbstractCategory* category, const VType& data)
    {
        return new CT_StdItemAttributeT<VType>(category, data);
    }
};

#endif // CT_IACREATOR_H
