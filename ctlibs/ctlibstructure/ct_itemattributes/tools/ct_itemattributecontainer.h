#ifndef CT_ITEMATTRIBUTECONTAINER_H
#define CT_ITEMATTRIBUTECONTAINER_H

#include "ct_itemattributes/abstract/ct_abstractitemattribute.h"
#include "ct_model/inModel/abstract/def_ct_inabstractitemattributemodel.h"

#include <QHash>

/**
 * @brief Just a container that manage item attributes. It use a QHash
 *        with the key => the result that contains the item attribute
 *        with the value => a list of item attributes (contained in the result [KEY])
 */
class CTLIBSTRUCTURE_EXPORT CT_ItemAttributeContainer
{
public:
    using ItemAttributesVisitor = std::function<bool(const CT_AbstractItemAttribute*)>;

    CT_ItemAttributeContainer();
    virtual ~CT_ItemAttributeContainer();

    /**
     * @brief Add an attribute
     * @return false if the attribute already exist (model uniqueName already exist) in this container
     */
    bool addItemAttribute(CT_AbstractItemAttribute *att);

    /**
     * @brief Remove an attribute
     */
    void removeItemAttribute(CT_AbstractItemAttribute *att);

    /**
     * @brief Remove all attributes contained in the result passed in parameter
     */
    void removeItemAttributeFromResult(const CT_AbstractResult *result);

    /**
     * @brief Call it to visit all default item attributes that was added by a specific type of item
     *        and classes that he inherit.
     * @return True if no item attribute has been visited, otherwise return the result of the visitor.
     */
    bool visitItemAttributes(const ItemAttributesVisitor& visitor) const;

    /**
     * @brief Call it to visit all default item attributes that was added by a specific type of item
     *        and classes that he inherit.
     * @return True if no item attribute has been visited, otherwise return the result of the visitor.
     */
    bool visitItemAttributesOfResult(const CT_AbstractResult* result, const ItemAttributesVisitor& visitor) const;

    /**
     * @brief Call it to visit all default item attributes that was added by a specific type of item
     *        and classes that he inherit.
     * @return True if no item attribute has been visited, otherwise return the result of the visitor.
     */
    bool visitItemAttributesOfOutModel(const CT_OutAbstractItemAttributeModel* outModel, const ItemAttributesVisitor& visitor) const;

    /**
     * @brief Call it to visit all default item attributes that was added by a specific type of item
     *        and classes that he inherit.
     * @return True if no item attribute has been visited, otherwise return the result of the visitor.
     */
    bool visitItemAttributesOfInModel(const CT_InAbstractItemAttributeModel* inModel, const ItemAttributesVisitor& visitor) const;

    /**
     * @brief Returns the number of item attributes
     */
    int nItemAttributes() const;

    /**
     * @brief Returns all attributes
     */
    QList<CT_AbstractItemAttribute*> itemAttributes() const;

    /**
     * @brief Returns all attributes contained in the result passed in parameter
     */
    QList<CT_AbstractItemAttribute*> itemAttributesFromResult(const CT_AbstractResult* result) const;

    /**
     * @brief Returns the item attribute corresponding to the OUTPUT model passed in parameter
     * @return nullptr if item attribute don't exist in this singular item
     */
    CT_AbstractItemAttribute* itemAttributeFromOutModel(const CT_OutAbstractItemAttributeModel* outModel) const;

    /**
     * @brief Returns a list of item attributes corresponding to the possibility selected of the INPUT model passed in parameter
     * @return nullptr if item attribute don't exist in this singular item
     */
    QList<CT_AbstractItemAttribute*> itemAttributesFromInModel(const CT_InAbstractItemAttributeModel* inModel) const;

    /**
     * @brief Returns the first item attribute corresponding to the possibility selected of the INPUT model passed in parameter
     * @return nullptr if item attribute don't exist in this singular item
     */
    CT_AbstractItemAttribute* firstItemAttributeFromInModel(const CT_InAbstractItemAttributeModel* inModel) const;

    /**
     * @brief Clear all attributes from the container
     */
    void clear();

private:
    QHash<CT_AbstractResult*, QList<CT_AbstractItemAttribute*>*> m_attributes;
};

#endif // CT_ITEMATTRIBUTECONTAINER_H
