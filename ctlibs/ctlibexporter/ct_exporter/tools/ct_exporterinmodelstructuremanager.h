#ifndef CT_EXPORTERINMODELSTRUCTUREMANAGER_H
#define CT_EXPORTERINMODELSTRUCTUREMANAGER_H

#include "ctlibexporter_global.h"

#include "ct_model/inModel/manager/ct_inmodelstructuremanager.h"
#include "ct_model/inModel/ct_instdgroupmodel.h"
#include "ct_model/inModel/ct_inresultmodelgrouptocopy.h"

#include "ct_handle/ct_handleinstdzeroormoregroup.h"
#include "ct_handle/ct_handleinstdgroup.h"
#include "ct_handle/ct_handleinsingularitem.h"

#include "tools/sfinae.h"

class CTLIBEXPORTER_EXPORT CT_ExporterInModelStructureManager
{
public:
    CT_ExporterInModelStructureManager(CT_InModelStructureManager& manager,
                                       CT_HandleInStdZeroOrMoreGroup& hInZeroOrMoreGroup) :
        m_manager(manager),
        m_hInZeroOrMoreGroup(hInZeroOrMoreGroup),
        m_hAbstractInGroup(nullptr),
        m_hAbstractInItem(nullptr)
    {
    }

    /**
     * @brief Returns true if this manager has at least one result model
     */
    bool needInputs() const {
        return m_manager.needInputs();
    }

    /**
     * @brief Returns true if at least one model (recursive) use the specified type
     */
    bool doesAtLeastOneInputModelsUseType(const QString& type) const {
        return m_manager.doesAtLeastOneInputModelsUseType(type);
    }

    /**
     * @brief Add a group to the root group model
     * @param parentGroup : the handle of the input group model to use to add the new group
     * @param groupHandle : the handle of the input group model to use to create the new group model and access it later
     * @param displayableName : the displayable that must be set to the new group model
     * @param shortDescription : the short description that must be set to the new group model
     * @param detailledDescription : the detailled description that must be set to the new group model
     */
    template<class HandleInGroup>
    void addGroupToRootGroup(HandleInGroup& groupHandle,
                             const QString& displayableName = QString(),
                             const QString& shortDescription = QString(),
                             const QString& detailledDescription = QString()) {

        m_manager.addGroup(m_hInZeroOrMoreGroup,
                           groupHandle,
                           displayableName,
                           shortDescription,
                           detailledDescription);

        if(m_hAbstractInGroup == nullptr)
            m_hAbstractInGroup = &groupHandle;
    }

    /**
     * @brief Add an item to a group
     * @param itemHandle : the handle of the input item model to use to create the new item model and access it later
     * @param displayableName : the displayable that must be set to the new item model
     * @param shortDescription : the short description that must be set to the new item model
     * @param detailledDescription : the detailled description that must be set to the new item model
     */
    template<class HandleInGroupParent, class HandleInItem>
    void addItemToGroup(const HandleInGroupParent& groupHandle,
                        HandleInItem& itemHandle,
                        const QString& displayableName = QString(),
                        const QString& shortDescription = QString(),
                        const QString& detailledDescription = QString()) {
        m_manager.addItem(groupHandle,
                          itemHandle,
                          displayableName,
                          shortDescription,
                          detailledDescription);

        if(m_hAbstractInItem == nullptr) {
            m_hAbstractInGroup = &const_cast<HandleInGroupParent&>(groupHandle);
            m_hAbstractInItem = &itemHandle;
        }
    }

    /**
     * @brief Add a point attribute to a group
     * @param itemHandle : the handle of the input item model to use to create the new item model and access it later
     * @param displayableName : the displayable that must be set to the new item model
     * @param shortDescription : the short description that must be set to the new item model
     * @param detailledDescription : the detailled description that must be set to the new item model
     */
    template<class HandleInGroupParent, class HandleInPointAttribute>
    void addPointAttributeToGroup(const HandleInGroupParent& groupHandle,
                                  HandleInPointAttribute& itemHandle,
                                  const QString& displayableName = QString(),
                                  const QString& shortDescription = QString(),
                                  const QString& detailledDescription = QString()) {
        m_manager.addPointAttribute(groupHandle,
                                    itemHandle,
                                    displayableName,
                                    shortDescription,
                                    detailledDescription);

        if(m_hAbstractInItem == nullptr) {
            m_hAbstractInGroup = &const_cast<HandleInGroupParent&>(groupHandle);
            m_hAbstractInItem = &itemHandle;
        }
    }

    /**
     * @brief Add a point attribute to a group
     * @param itemHandle : the handle of the input item model to use to create the new item model and access it later
     * @param displayableName : the displayable that must be set to the new item model
     * @param shortDescription : the short description that must be set to the new item model
     * @param detailledDescription : the detailled description that must be set to the new item model
     */
    template<class HandleInGroupParent, class HandleInEdgeAttribute>
    void addEdgeAttributeToGroup(const HandleInGroupParent& groupHandle,
                                  HandleInEdgeAttribute& itemHandle,
                                  const QString& displayableName = QString(),
                                  const QString& shortDescription = QString(),
                                  const QString& detailledDescription = QString()) {
        m_manager.addEdgeAttribute(groupHandle,
                                   itemHandle,
                                   displayableName,
                                   shortDescription,
                                   detailledDescription);

        if(m_hAbstractInItem == nullptr) {
            m_hAbstractInGroup = &const_cast<HandleInGroupParent&>(groupHandle);
            m_hAbstractInItem = &itemHandle;
        }
    }

    /**
     * @brief Add a point attribute to a group
     * @param itemHandle : the handle of the input item model to use to create the new item model and access it later
     * @param displayableName : the displayable that must be set to the new item model
     * @param shortDescription : the short description that must be set to the new item model
     * @param detailledDescription : the detailled description that must be set to the new item model
     */
    template<class HandleInGroupParent, class HandleInFaceAttribute>
    void addFaceAttributeToGroup(const HandleInGroupParent& groupHandle,
                                 HandleInFaceAttribute& itemHandle,
                                 const QString& displayableName = QString(),
                                 const QString& shortDescription = QString(),
                                 const QString& detailledDescription = QString()) {
        m_manager.addFaceAttribute(groupHandle,
                                   itemHandle,
                                   displayableName,
                                   shortDescription,
                                   detailledDescription);

        if(m_hAbstractInItem == nullptr) {
            m_hAbstractInGroup = &const_cast<HandleInGroupParent&>(groupHandle);
            m_hAbstractInItem = &itemHandle;
        }
    }

    /**
     * @brief Add an item attribute to an item
     * @param parentItem : the handle of the input item model to use to add the new item attribute
     * @param itemAttributeHandle : the handle of the input item attribute model to use to create the new item attribute model and access it later
     * @param categories : one category or a list of category. Per example : CT_AbstractCategory::DATA_VALUE or QStringList{CT_AbstractCategory::DATA_SIZE, CT_AbstractCategory::DATA_RADIUS}
     * @param displayableName : the displayable that must be set to the new item attribute model
     * @param shortDescription : the short description that must be set to the new item attribute model
     * @param detailledDescription : the detailled description that must be set to the new item attribute model
     */
    template<class HandleInItemParent, class HandleInItemAttribute, typename Categories>
    void addItemAttribute(const HandleInItemParent& parentItem,
                          HandleInItemAttribute& itemAttributeHandle,
                          const Categories& categories,
                          const QString& displayableName = QString{"In Item Attribute"},
                          const QString& shortDescription = QString(),
                          const QString& detailledDescription = QString()) {

        m_manager.addItemAttribute(parentItem,
                                   itemAttributeHandle,
                                   categories,
                                   displayableName,
                                   shortDescription,
                                   detailledDescription);
    }

    /**
     * @brief Visit all input result model added to this manager
     * @param visitor : a lambda expression per example that return a bool (true if must continue the visit) and receive a const pointer to a CT_InAbstractResultModel
     * @return True if no results has been visited, otherwise the returned value of the visitor
     */
    template<typename Visitor>
    bool visitResults(const Visitor& visitor) const {
        return m_manager.visitResults(visitor);
    }

    /**
     * @brief Returns the number of results
     */
    int nResults() const {
        return m_manager.nResults();
    }

private:
    CT_InModelStructureManager&             m_manager;
    CT_HandleInStdZeroOrMoreGroup&          m_hInZeroOrMoreGroup;

    friend class CT_AbstractExporter;

    CT_AbstractHandleInGroup*               m_hAbstractInGroup;
    CT_AbstractHandleInItem*                m_hAbstractInItem;
};

#endif // CT_EXPORTERINMODELSTRUCTUREMANAGER_H
