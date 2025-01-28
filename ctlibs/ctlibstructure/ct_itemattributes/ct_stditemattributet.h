#ifndef CT_STDITEMATTRIBUTET_H
#define CT_STDITEMATTRIBUTET_H

#include "ct_itemattributes/abstract/ct_abstractitemattributet.h"

/**
 * @brief Standard class for an item attribute that contains a data that will not change. Use
 *        this class with a CT_IACreator per example :
 *
 *        const int myData = 5;
 *        CT_StdItemAttributeT<int>* itemAttribute = CT_IACreator::create<int>(CATEGORY_NAME, myData);
 */
template <typename VType>
class CT_StdItemAttributeT : public CT_AbstractItemAttributeT<VType>
{
    using SuperClass = CT_AbstractItemAttributeT<VType>;

public:
    /**
     * @brief Constructor for model
     */
    CT_StdItemAttributeT();

    /**
     * @brief Create an attribute with a category and a data that will never change.
     * @param category : the category of the item attribute. Must not be nullptr !
     * @param data : the data.
     */
    CT_StdItemAttributeT(const CT_AbstractCategory* category, const VType& data);
    CT_StdItemAttributeT(const QString& categoryName, const VType& data);

    /**
     * @brief Copy contructor
     */
    CT_StdItemAttributeT(const CT_StdItemAttributeT<VType>& other) = default;

    /**
     * @brief Returns a copy of this item attribute
     */
    CT_AbstractItemAttribute* copy(const DEF_CT_OutAbstractIAModel* model, const CT_AbstractResult* result) const override;

    /**
     * @brief Returns the data
     */
    VType data(const CT_AbstractItem* item) const override;

private:
    const VType m_data;
};

#include "ct_stditemattributet.hpp"

#endif // CT_STDITEMATTRIBUTET_H
