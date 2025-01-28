#ifndef CT_ABSTRACTSINGULARITEMDRAWABLE_H
#define CT_ABSTRACTSINGULARITEMDRAWABLE_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawable.h"
#include "ct_itemattributes/tools/ct_defaultitemattributemanager.h"
#include "ct_itemattributes/tools/ct_itemattributecontainer.h"
#include "tools/sfinae.h"

#include <QColor>

class CT_StandardItemGroup;

/**
 * @brief Represent an item that is a leaf of a group in the tree structure. Only singular item can have item attributes.
 */
class CTLIBSTRUCTURE_EXPORT CT_AbstractSingularItemDrawable : public CT_AbstractItemDrawable, public ISingularItemDrawableForModel
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractSingularItemDrawable, CT_AbstractItemDrawable, Item)

    using SuperClass = CT_AbstractItemDrawable;

public:
    CT_AbstractSingularItemDrawable();

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - Unique ID
     *          - Pointer of base and alternative draw manager
     *          - Displayable name
     *          - Center coordinates
     *          - Default Color
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractSingularItemDrawable(const CT_AbstractSingularItemDrawable& other);

    /**
     * @brief Returns the x center coordinate of this item
     */
    double centerX() const override;

    /**
     * @brief Returns the y center coordinate of this item
     */
    double centerY() const override;

    /**
     * @brief Returns the z center coordinate of this item
     */
    double centerZ() const override;

    /**
     * @brief Visit childrens. Redefined to visit item attributes (default and new).
     * @return Returns true if no childrens has been visited otherwise returns the result of the visitor.
     */
    bool visitChildrens(const ChildrensVisitor& visitor) const override;

    /**
     * @brief Returns the number of attributes
     */
    int nChildrens() const override;

    /**
     * @brief Visit childrens of type CT_AbstractItemDrawable. Redefined because this item doesn't contains child of type CT_AbstractItemDrawable.
     * @return Returns true because no childrens can be visited.
     */
    bool visitChildrensOfTypeItem(const ItemVisitor&) const override { return true; }

    /**
     * @brief Add an item attribute to this item.
     * @param outItemAttributeHandle : the handle of the item attribute
     * @param itemAttribute : the item attribute to add (life in memory of this item attribute will be managed by this group)
     * @warning There is no verification made to check that this item attribute can be added or not to this item. Be sure what you do.
     */
    template<typename OutHandleType>
    void addItemAttribute(const OutHandleType& outItemAttributeHandle, CT_AbstractItemAttribute* itemAttribute) {
        Q_ASSERT(model() != nullptr);
        if (model() == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::addItemAttribute" << ", " << "model() == nullptr";}

        // the handle can have multiple models if it was created with a result copy so we must get the model
        // that his parent match with the model of this item
        const CT_OutAbstractItemAttributeModel* outModelToUse = outItemAttributeHandle.findAbstractModelWithParent(model());

        // now we can add the item with the right model
        addItemAttributeWithOutModel(outModelToUse, itemAttribute);
    }

    /**
     * @brief Add an item attribute to this item.
     * @param model : the model to set to the item attribute and to use to add it in the collection of attributes
     * @param itemAttribute : the item attribute to add (life in memory of this item attribute will be managed by this group)
     * @warning There is no verification made to check that this item attribute can be added or not to this item. Be sure what you do.
     */
    void addItemAttributeWithOutModel(const CT_OutAbstractItemAttributeModel* outModel, CT_AbstractItemAttribute* itemAttribute);

    /**
     * @brief Returns the item attribute that use the specified output model
     * @param outModel : the model of the item attribute to find. The model of the item attribute will be used to find it in the collection.
     * @return nullptr if no item attribute that use this model has been found
     */
    CT_AbstractItemAttribute* itemAttributeWithOutModel(const CT_OutAbstractItemAttributeModel* outModel) const;

    /**
     * @brief Visit all item attributes
     * @param visitor : the visitor to use
     * @return Returns true if no item attributes has been visited otherwise returns the result of the visitor.
     */
    bool visitItemAttributes(const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const;

    /**
     * @brief Visit all default item attributes
     * @param visitor : the visitor to use
     * @return Returns true if no item attributes has been visited otherwise returns the result of the visitor.
     */
    bool visitDefaultItemAttributes(const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const;

    /**
     * @brief Visit all not default item attributes
     * @param visitor : the visitor to use
     * @return Returns true if no item attributes has been visited otherwise returns the result of the visitor.
     */
    bool visitItemAttributesAdded(const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const;

    /**
     * @brief Visit all item attributes that will not be removed and that use output model of selected possibilities of the specified input model
     * @param inModel : the input model to use to visit selected possibilities to get output models
     * @param visitor : the visitor to use
     * @return Returns true if no attributes has been visited otherwise returns the result of the visitor.
     */
    bool visitItemAttributesInSelectedPossibilitiesOfInModel(const CT_InAbstractItemAttributeModel* inModel, const CT_ItemAttributeContainer::ItemAttributesVisitor& visitor) const;

    /**
     * @brief Return the number of default and added item attributes
     */
    int nItemAttributes() const;

    /**
     * @brief Return the number of default item attributes
     */
    int nDefaultItemAttributes() const;

    /**
     * @brief Return the number of added item attributes
     */
    int nItemAttributesAdded() const;

    /**
     * @brief Returns a list of item attributes corresponding to the possibility selected of the INPUT model passed in parameter
     */
    QList<CT_AbstractItemAttribute*> itemAttributes(const CT_InAbstractItemAttributeModel* inModel) const;

    /**
     * @brief Returns a list of item attributes corresponding to the list of out models passed in parameter
     */
    QList<CT_AbstractItemAttribute*> itemAttributes(const QList<CT_OutAbstractItemAttributeModel *>& outModelList) const;

    /**
     * @brief Return all item attributes of this item drawable
     */
    QList<CT_AbstractItemAttribute*> itemAttributes() const;

    /**
     * @brief Returns the first item attribute that use the model in the specified handle.
     * @param itemAttHandle : the handle of the item attribute (input or output)
     */
    template<typename HandleType>
    const typename HandleType::ItemAttributeType* itemAttribute(const HandleType& itemAttHandle) const {
        Q_ASSERT(model() != nullptr);
        if (model() == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::itemAttribute" << ", " << "model() == nullptr";}

        return internalItemAttribute(itemAttHandle, std::integral_constant<bool, IsAnOutputModel<typename HandleType::ModelType>::value>());
    }

    /**
     * @brief Returns an object to iterate over item attributes that use the specified handle.
     * @param itemAttHandle : the handle of the item attribute (input or output)
     */
    template<typename HandleType>
    ChildrensCollectionT<const typename HandleType::ItemAttributeType> itemAttributesByHandle(const HandleType& itemAttHandle) const {
        Q_ASSERT(model() != nullptr);
        if (model() == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::itemAttributesByHandle" << ", " << "model() == nullptr";}

        return internalItemAttributes(itemAttHandle, std::integral_constant<bool, IsAnOutputModel<typename HandleType::ModelType>::value>());
    }

    /**
     * @brief Returns the first item attribute that match with a possibility of the IN model.
     *
     *        An IN model can have a number of possibilities > 1 if you set Choose_MultipleIfMultiple. A
     *        group can contains items that match with one or two or etc... possibility of the model so he must
     *        return a list of ItemDrawable. This method test the first possibility, if a ItemDrawable is found,
     *        it will be returned otherwise the method continue to search with the next possibility, etc...
     *
     * @return nullptr if no item will be found
     */
    CT_AbstractItemAttribute* firstItemAttribute(const CT_InAbstractItemAttributeModel *inModel) const;

    /**
     * @brief Return all default item attributes of this item drawable
     */
    QList<CT_AbstractItemAttribute*> defaultItemAttributes() const;

    /**
     * @brief Return all item attributes (added by user) of this item drawable
     */
    QList<CT_AbstractItemAttribute*> itemAttributesAdded() const;

    /**
     * @brief Set a default color to this item. When it will added for the first time in a view it will have this color
     */
    void setDefaultColor(const QColor &color);

    /**
     * @brief Returns the default color of this item
     */
    QColor defaultColor() const;

    /**
     * @brief Called from result or parent item to inform that this item will be deleted from the result passed in parameter
     */
    void willBeRemovedFromResult(const CT_AbstractResult* res) override;

    /**
     * @brief Set the parent group
     * @warning Not intended for direct use by plugin developper
     */
    void setParentGroup(const CT_StandardItemGroup* pGroup);

    /**
     * @brief Returns the parentGroup of this group
     * @return nullptr if the group is a root group (the parent group is the result)
     */
    CT_StandardItemGroup* parentGroup() const;

protected:
    /**
     * @brief Create and return a qt style iterator to iterate over childrens (groups or items or etc...) that use the specified out model
     * @param outModel : the out model of childrens
     * @return A new qt style iterator to iterate over childrens (groups or items or etc...) that use the specified out model
     */
    IChildrensIteratorQtStylePtr createQtStyleIteratorForChildrensThatUseOutModel(const CT_OutAbstractModel* outModel) const override;

private:
    class ItemAttributeIterator : public IChildrensIteratorQtStyle {
    public:
        ItemAttributeIterator(const CT_AbstractItemAttribute* item) : m_current(const_cast<CT_AbstractItemAttribute*>(item)) {}

        bool hasNext() const override { return m_current != nullptr; }

        CT_AbstractItem* next() override {
            CT_AbstractItem* c = m_current;
            m_current = nullptr;
            return c;
        }

        IChildrensIteratorQtStyle* copy() const override { return new ItemAttributeIterator(m_current); }

    private:
        CT_AbstractItemAttribute* m_current;
    };

    class ItemAttributesIterator : public IChildrensIteratorQtStyle {
    public:
        ItemAttributesIterator(const QList<const CT_AbstractItemAttribute*>& itemAtts) : mCollection(itemAtts), mIt(mCollection) {}

        bool hasNext() const override { return mIt.hasNext(); }

        CT_AbstractItem* next() override {
            return const_cast<CT_AbstractItemAttribute*>(mIt.next());
        }

        IChildrensIteratorQtStyle* copy() const override { return new ItemAttributesIterator(mCollection); }

    private:
        QList<const CT_AbstractItemAttribute*> mCollection;
        QListIterator<const CT_AbstractItemAttribute*> mIt;
    };

    /**
     * @brief The container of attributes added to this item (not default)
     */
    CT_ItemAttributeContainer   m_itemAttributes;

    /**
     * @brief The default color to use when it was added to a document
     */
    QColor                      m_defaultColor;

    /**
     * @brief Writted to create a default Item Attribute
     */
    quint64 pId() const {return id();}

    /**
     * @brief Writted to create a default Item Attribute
     */
    QString pDisplayableName() const { return displayableName(); }

    /**
     * @brief Returns the first item attribute that use the model in the specified handle.
     * @param outItemHandle : the handle of the item (output)
     */
    template<typename OutHandleType>
    const typename OutHandleType::ItemAttributeType* internalItemAttribute(const OutHandleType& outItemHandle,
                                                                 std::true_type) const {
        // the handle can have multiple models if it was created with a result copy so we must get the model
        // that his parent match with the model of this group
        const DEF_CT_OutAbstractIAModel* outModelToUse = outItemHandle.findAbstractModelWithParent(model());

        Q_ASSERT(outModelToUse != nullptr);
        if (outModelToUse == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::internalItemAttribute" << ", " << "outModelToUse == nullptr";}

        return static_cast<typename OutHandleType::ItemAttributeType*>(itemAttributeWithOutModel(outModelToUse));
    }

    /**
     * @brief Returns the first item attribute that use the model in the specified handle.
     * @param inItemHandle : the handle of the item (input)
     */
    template<typename InHandleType>
    const typename InHandleType::ItemAttributeType* internalItemAttribute(const InHandleType& inHandle,
                                                                std::false_type) const {
        const typename InHandleType::ItemAttributeType* found = nullptr;

        const CT_OutAbstractModel::UniqueIndexType myModelUI = model()->uniqueIndex();

        visitInModelWithPossibilitiesFromInHandle(inHandle, [&found, &myModelUI, this](CT_InAbstractModel* inModelWithPossibilities) -> bool {

            const CT_InStdModelPossibilitySelectionGroup* selectionGroup = inModelWithPossibilities->possibilitiesGroup();

            for(const CT_InStdModelPossibility* possibility : selectionGroup->selectedPossibilities()) {
                const CT_OutAbstractModel* outModel = possibility->outModel();

                if(static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() == myModelUI) {
                    const DEF_CT_OutAbstractIAModel* outModelToUse = dynamic_cast<const DEF_CT_OutAbstractIAModel*>(outModel);

                    if(outModelToUse != nullptr) {
                        found = static_cast<typename InHandleType::ItemAttributeType*>(itemAttributeWithOutModel(outModelToUse));
                        return false;
                    }
                }
            }

            return true;
        });

        return found;
    }

    /**
     * @brief Returns an iterator to iterate over the item attribute that use the model in the specified handle.
     * @param outItemHandle : the handle of the item (output)
     */
    template<typename OutHandleType>
    ChildrensCollectionT<const typename OutHandleType::ItemAttributeType> internalItemAttributes(const OutHandleType& outItemHandle,
                                                                 std::true_type) const {
        // the handle can have multiple models if it was created with a result copy so we must get the model
        // that his parent match with the model of this group
        const DEF_CT_OutAbstractIAModel* outModelToUse = outItemHandle.findAbstractModelWithParent(model());

        Q_ASSERT(outModelToUse != nullptr);
        if (outModelToUse == nullptr) {qDebug() << "CT_AbstractSingularItemDrawable::internalItemAttributes" << ", " << "outModelToUse == nullptr";}

        QList<const typename OutHandleType::ItemAttributeType*> founds;
        founds.append(static_cast<typename OutHandleType::ItemAttributeType*>(itemAttributeWithOutModel(outModelToUse)));

        return ChildrensCollectionT<const typename OutHandleType::ItemAttributeType>(new ItemAttributesIterator(founds));
    }

    /**
     * @brief Returns an iterator to iterate over all item attributes that use the model in the specified handle.
     * @param inItemHandle : the handle of the item (input)
     */
    template<typename InHandleType>
    ChildrensCollectionT<const typename InHandleType::ItemAttributeType> internalItemAttributes(const InHandleType& inHandle,
                                                                std::false_type) const {
        QList<const typename InHandleType::ItemAttributeType*> founds;

        const CT_OutAbstractModel::UniqueIndexType myModelUI = model()->uniqueIndex();

        visitInModelWithPossibilitiesFromInHandle(inHandle, [&founds, &myModelUI, this](CT_InAbstractModel* inModelWithPossibilities) -> bool {

            const CT_InStdModelPossibilitySelectionGroup* selectionGroup = inModelWithPossibilities->possibilitiesGroup();

            for(const CT_InStdModelPossibility* possibility : selectionGroup->selectedPossibilities()) {
                const CT_OutAbstractModel* outModel = possibility->outModel();

                if(static_cast<CT_OutAbstractModel*>(outModel->parentModel())->uniqueIndex() == myModelUI) {
                    const DEF_CT_OutAbstractIAModel* outModelToUse = dynamic_cast<const DEF_CT_OutAbstractIAModel*>(outModel);

                    if(outModelToUse != nullptr)
                        founds.append(static_cast<typename InHandleType::ItemAttributeType*>(itemAttributeWithOutModel(outModelToUse)));
                }
            }

            return true;
        });

        return ChildrensCollectionT<const typename InHandleType::ItemAttributeType>(new ItemAttributesIterator(founds));
    }

    // declare that we will add default item attributes in this class
    CT_DEFAULT_IA_BEGIN(CT_AbstractSingularItemDrawable)
    CT_DEFAULT_IA_V2(CT_AbstractSingularItemDrawable, CT_AbstractCategory::staticInitDataId(), &CT_AbstractSingularItemDrawable::pId, QObject::tr("ID"))
    CT_DEFAULT_IA_V2(CT_AbstractSingularItemDrawable, CT_AbstractCategory::staticInitDataDisplayableName(), &CT_AbstractSingularItemDrawable::pDisplayableName, QObject::tr("Name"))
    CT_DEFAULT_IA_END(CT_AbstractSingularItemDrawable)
};

#endif // CT_ABSTRACTSINGULARITEMDRAWABLE_H
