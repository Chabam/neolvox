#ifndef CT_ABSTRACTITEMATTRIBUTE_H
#define CT_ABSTRACTITEMATTRIBUTE_H

#include "ct_item/abstract/ct_abstractitem.h"
#include "ct_categories/abstract/ct_abstractcategory.h"
#include "ct_model/outModel/abstract/def_ct_outabstractitemattributemodel.h"

/**
 * @brief An item attribute is a scalar value of type CT_AbstractCategory::ValueType
 *        and use a category (CT_AbstractCategory) to be classified.
 */
class CTLIBSTRUCTURE_EXPORT CT_AbstractItemAttribute : public PrototypeModelInterfaceInheritTool<CT_AbstractItem, IItemAttributeForModel>
{
    Q_OBJECT
    using SuperClass = PrototypeModelInterfaceInheritTool<CT_AbstractItem, IItemAttributeForModel>;

public:
    using CategoryType = CT_AbstractCategory;

    /**
     * @brief Constructor for model
     */
    CT_AbstractItemAttribute();

    /**
     * @brief Create an attribute with a category.
     * @param category : the category of the item attribute. Must not be nullptr !
     */
    CT_AbstractItemAttribute(const CT_AbstractCategory* category);
    CT_AbstractItemAttribute(const QString& categoryName);

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - Pointer to the category.
     */
    CT_AbstractItemAttribute(const CT_AbstractItemAttribute& other);

    /**
     * @brief Visit childrens
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    bool visitChildrens(const ChildrensVisitor&) const override;

    /**
     * @brief Returns 0 because item attributes doesn't have childrens
     */
    int nChildrens() const override;

    /**
     * @brief Visit childrens that use the specified out model
     * @param outModel : the out model of child
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    //bool visitChildrensThatUseOutModel(const CT_OutAbstractModel*, const ChildrensVisitor&) const override { /* no childrens */ }

    /**
     * @brief Returns a displayable name (by default if model is not null return model()->displayableName(), else
     *        returns category()->displayableName())
     */
    QString displayableName() const;

    /**
     * @brief Set the category
     * @warning Not intended for direct use by plugin developper
     */
    void setCategory(const CT_AbstractCategory* category);

    /**
     * @brief Returns the category of the item attribute
     */
    CT_AbstractCategory* category() const;

    /**
     * @brief Returns the category of the item attribute statically casted to another type
     */
    template<typename CategoryType>
    CategoryType* categoryStaticT() const {
        return static_cast<CategoryType*>(category());
    }

    /**
      * @brief Returns true if the value type is different from CT_AbstractCategory::UNKNOWN.
      */
    bool isValid() const;

    /**
      * @brief Returns the type of value
      */
    virtual CT_AbstractCategory::ValueType valueType() const = 0;

    /**
      * @brief Returns the type of the value in QString
      */
    virtual QString valueTypeToString() const = 0;

    /**
      * @brief Methods to get the value in bool type
      *
      * @param item : the item to which this item attribute belongs
      * @param ok : false if the value can not be converted to the type you want. ok can be nullptr if you don't want
      *        to know the validity of the conversion.
      */
    virtual bool toBool(const CT_AbstractItem* item, bool* ok) const = 0;

    /**
      * @brief Methods to get the value in double type
      *
      * @param item : the item to which this item attribute belongs
      * @param ok : false if the value can not be converted to the type you want. ok can be nullptr if you don't want
      *        to know the validity of the conversion.
      */
    virtual double toDouble(const CT_AbstractItem* item, bool* ok) const = 0;

    /**
      * @brief Methods to get the value in float type
      *
      * @param item : the item to which this item attribute belongs
      * @param ok : false if the value can not be converted to the type you want. ok can be nullptr if you don't want
      *        to know the validity of the conversion.
      */
    virtual float toFloat(const CT_AbstractItem* item, bool* ok) const = 0;

    /**
      * @brief Methods to get the value in long double type
      *
      * @param item : the item to which this item attribute belongs
      * @param ok : false if the value can not be converted to the type you want. ok can be nullptr if you don't want
      *        to know the validity of the conversion.
      */
    virtual long double toLongDouble(const CT_AbstractItem* item, bool* ok) const = 0;

    /**
      * @brief Methods to get the value in int type
      *
      * @param item : the item to which this item attribute belongs
      * @param ok : false if the value can not be converted to the type you want. ok can be nullptr if you don't want
      *        to know the validity of the conversion.
      */
    virtual int toInt(const CT_AbstractItem* item, bool* ok) const = 0;

    /**
      * @brief Methods to get the value in uint64 type
      *
      * @param item : the item to which this item attribute belongs
      * @param ok : false if the value can not be converted to the type you want. ok can be nullptr if you don't want
      *        to know the validity of the conversion.
      */
    virtual quint64 toUInt64(const CT_AbstractItem* item, bool* ok) const = 0;

    /**
      * @brief Methods to get the value in size_t type
      *
      * @param item : the item to which this item attribute belongs
      * @param ok : false if the value can not be converted to the type you want. ok can be nullptr if you don't want
      *        to know the validity of the conversion.
      */
    virtual size_t toSizeT(const CT_AbstractItem* item, bool* ok) const = 0;

    /**
      * @brief Methods to get the value in QString type
      *
      * @param item : the item to which this item attribute belongs
      * @param ok : false if the value can not be converted to the type you want. ok can be nullptr if you don't want
      *        to know the validity of the conversion.
      */
    virtual QString toString(const CT_AbstractItem* item, bool* ok) const = 0;

    /**
     * @brief Change the model of the attribute
     * @warning Not intended for direct use by plugin developper
     */
    void setItemAttributeModel(const DEF_CT_OutAbstractIAModel* model);

    /**
     * @brief Returns the tool for model
     */
    IPrototypeToolForModel* prototypeToolForModel() const override;

    /**
     * @brief Returns the tool for model
     */
    IItemAttributeToolForModel* itemAttributeToolForModel() const override;

    /**
     * @brief Returns a copy of the item attribute
     * @param model : the new model that must be set to the copy
     * @param result : the new result that will contains the copy
     * @return nullptr if it was an error otherwise a copy of the item
     */
    virtual CT_AbstractItemAttribute* copy(const DEF_CT_OutAbstractIAModel* model, const CT_AbstractResult* result) const = 0;

protected:
    /**
     * @brief Create and return a qt style iterator to iterate over childrens (groups or items or etc...) that use the specified out model
     * @param outModel : the out model of childrens
     * @return A new qt style iterator to iterate over childrens (groups or items or etc...) that use the specified out model
     */
    IChildrensIteratorQtStylePtr createQtStyleIteratorForChildrensThatUseOutModel(const CT_OutAbstractModel*) const override;

private:
    class ItemAttributeToolForModel : public IItemAttributeToolForModel {
    public:
        CT_AbstractItemAttribute* m_pointer;

        ICategoryForModel* category() const override { return m_pointer->category(); }
        int valueType() const override { return static_cast<int>(m_pointer->valueType()); }
        QString valueTypeToString() const override { return m_pointer->valueTypeToString(); }
        IItemAttributeForModel* copyItemAttribute() const override { return m_pointer->copy(nullptr, nullptr); }
    };

    /**
     * @brief The tool for model
     */
    ItemAttributeToolForModel   m_iaTool;

    /**
     * @brief The category of this item attribute
     */
    CT_AbstractCategory*        m_category;
};

#endif // CT_ABSTRACTITEMATTRIBUTE_H
