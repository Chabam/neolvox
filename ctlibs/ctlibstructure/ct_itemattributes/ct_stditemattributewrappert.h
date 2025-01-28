#ifndef CT_STDITEMATTRIBUTEWRAPPERT_H
#define CT_STDITEMATTRIBUTEWRAPPERT_H

#include "ct_itemattributes/abstract/ct_abstractitemattributet.h"

/**
 * @brief Standard class for an item attribute that can call a member method of a class. So the
 *        data can change. Use this class with a CT_IACreator per example :
 *
 *        CT_StdItemAttributeWrapperT<CT_Line, float>* attribute = CT_IACreator::create<CT_Line, float>(CATEGORY_NAME, &CT_Line::getP1_X);
 */
template <class ClassType, typename VType>
class CT_StdItemAttributeWrapperT : public CT_AbstractItemAttributeT<VType>
{
    using SuperClass = CT_AbstractItemAttributeT<VType>;

public:
    /**
     * @brief Pointer to a member method of the specified class
     */
    typedef VType (ClassType::*getter)() const;

    /**
     * @brief Constructor for model
     */
    CT_StdItemAttributeWrapperT();

    /**
     * @brief Create an item attribute with a category and a pointer to the member method of specified class.
     * @param category : the category of the item attribute. Must not be nullptr !
     * @param gMethod : the pointer to the member method of the specified class.
     */
    CT_StdItemAttributeWrapperT(const CT_AbstractCategory* category, getter gMethod);

    /**
     * @brief Copy contructor
     */
    CT_StdItemAttributeWrapperT(const CT_StdItemAttributeWrapperT<ClassType, VType>& other) = default;

    /**
     * @brief Returns a copy of this item attribute
     */
    CT_AbstractItemAttribute* copy(const DEF_CT_OutAbstractIAModel* model, const CT_AbstractResult* result) const override;

    /**
     * @brief Returns the data
     */
    VType data(const CT_AbstractItem* item) const override;

private:
    getter  m_gMethod;
};

#include "ct_stditemattributewrappert.hpp"

#endif // CT_STDITEMATTRIBUTEWRAPPERT_H
