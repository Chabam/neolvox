#ifndef CT_METRICOUTMODELSTRUCTUREMANAGER_H
#define CT_METRICOUTMODELSTRUCTUREMANAGER_H

#include "ctlibmetrics_global.h"

#include "ct_model/outModel/manager/ct_outmodelstructuremanager.h"
#include "ct_model/inModel/ct_instdsingularitemmodel.h"
#include "ct_model/inModel/ct_inresultmodelgrouptocopy.h"
#include "ct_handle/ct_handleoutsingularitem.h"
#include "ct_handle/ct_handleoutstdgroup.h"
#include "ct_itemdrawable/ct_itemattributelist.h"

#include "tools/sfinae.h"

class CTLIBMETRICS_EXPORT CT_MetricOutModelStructureManager
{
public:
    using ToolToModifyResultModelCopiesType = CT_InResultModelGroupToCopy::ToolToModifyResultModelCopiesType;

    CT_MetricOutModelStructureManager(CT_OutModelStructureManager& manager, CT_HandleOutItem<CT_ItemAttributeList>& hOutItem) :
        m_manager(manager),
        m_hOutItem(&hOutItem),
        m_inParentModel(nullptr),
        m_inTool(nullptr)
    {
    }

    CT_MetricOutModelStructureManager(CT_OutModelStructureManager& manager,
                                      const CT_InStdSingularItemModel* inParentModel,
                                      const ToolToModifyResultModelCopiesType* inTool) :
        m_manager(manager),
        m_inParentModel(const_cast<CT_InStdSingularItemModel*>(inParentModel)),
        m_inTool(const_cast<ToolToModifyResultModelCopiesType*>(inTool))
    {
    }

    template<class HandleItem>
    static CT_MetricOutModelStructureManager createFromHandle(CT_OutModelStructureManager& manager,
                                                              HandleItem& hParentItem)
    {
        return internalCreateFromHandle(manager,
                                        hParentItem,
                                        std::integral_constant<bool, IsAnOutputModel<typename HandleItem::ModelType>::value>());
    }

    template<class HandleInItem>
    static CT_MetricOutModelStructureManager createFromInHandle(CT_OutModelStructureManager& manager, HandleInItem& hInItem)
    {
        using InResultModelCopyType = typename HandleInItem::InResultModelCopyType;

        CT_InStdSingularItemModel* inParentModel = hInItem.model();

        Q_ASSERT(inParentModel != nullptr);
        if (inParentModel == nullptr) {qDebug() << "CT_MetricOutModelStructureManager::createFromInHandle" << ", " <<  "inParentModel == nullptr";}

        InResultModelCopyType* inResultModelCopy = dynamic_cast<InResultModelCopyType*>(inParentModel->rootModel());

        Q_ASSERT(inResultModelCopy != nullptr);
        if (inResultModelCopy == nullptr) {qDebug() << "CT_MetricOutModelStructureManager::createFromInHandle" << ", " <<  "inResultModelCopy == nullptr";}

        ToolToModifyResultModelCopiesType* inTool = inResultModelCopy->toolToModifyResultModelCopies();

        Q_ASSERT(inTool != nullptr);
        if (inTool == nullptr) {qDebug() << "CT_MetricOutModelStructureManager::createFromInHandle" << ", " <<  "inTool == nullptr";}

        return CT_MetricOutModelStructureManager(manager, inParentModel, inTool);
    }

    /**
     * @brief Add an item attribute to the out item model
     * @param itemAttributeHandle : the handle of the output item attribute model to use to create the new item attribute model and access it later
     * @param category : a category, per example CT_AbstractCategory::DATA_VALUE
     * @param displayableName : the displayable that must be set to the new item attribute model
     * @param shortDescription : the short description that must be set to the new item attribute model
     * @param detailledDescription : the detailled description that must be set to the new item attribute model
     */
    template<class HandleOutItemAttribute, typename Category>
    void addItemAttribute(HandleOutItemAttribute& itemAttributeHandle,
                          const Category& category,
                          const QString& displayableName = QString{"Out Item Attribute"},
                          const QString& shortDescription = QString{""},
                          const QString& detailledDescription = QString{""},
                          typename HandleOutItemAttribute::ItemAttributeType* prototype = nullptr) {

        if(m_hOutItem != nullptr) {
            this->addItemAttribute(*m_hOutItem,
                                   itemAttributeHandle,
                                   category,
                                   displayableName,
                                   shortDescription,
                                   detailledDescription,
                                   prototype);
            return;
        }

        m_manager.addItemAttributeWithInputTool(m_inParentModel,
                                                m_inTool,
                                                itemAttributeHandle,
                                                category,
                                                displayableName,
                                                shortDescription,
                                                detailledDescription,
                                                prototype);
    }

private:
    CT_OutModelStructureManager&            m_manager;
    CT_HandleOutItem<CT_ItemAttributeList>* m_hOutItem;
    CT_InStdSingularItemModel*              m_inParentModel;
    ToolToModifyResultModelCopiesType*      m_inTool;

    template<class HandleItem>
    static CT_MetricOutModelStructureManager internalCreateFromHandle(CT_OutModelStructureManager& manager,
                                                                      HandleItem& hParentItem,
                                                                      std::true_type)
    {
        return CT_MetricOutModelStructureManager(manager, hParentItem);
    }

    template<class HandleItem>
    static CT_MetricOutModelStructureManager internalCreateFromHandle(CT_OutModelStructureManager& manager,
                                                                      HandleItem& hParentItem,
                                                                      std::false_type)
    {
        return createFromInHandle(manager, hParentItem);
    }

    /**
     * @brief Add an item attribute to an item model
     * @param parentItem : the handle of an output item model to use to add the new item attribute
     * @param itemAttributeHandle : the handle of the output item attribute model to use to create the new item attribute model and access it later
     * @param category : a category, per example CT_AbstractCategory::DATA_VALUE
     * @param displayableName : the displayable that must be set to the new item attribute model
     * @param shortDescription : the short description that must be set to the new item attribute model
     * @param detailledDescription : the detailled description that must be set to the new item attribute model
     */
    template<class HandleItemParent, class HandleOutItemAttribute, typename Category>
    void addItemAttribute(const HandleItemParent& parentItem,
                          HandleOutItemAttribute& itemAttributeHandle,
                          const Category& category,
                          const QString& displayableName = QString{"Out Item Attribute"},
                          const QString& shortDescription = QString{""},
                          const QString& detailledDescription = QString{""},
                          typename HandleOutItemAttribute::ItemAttributeType* prototype = nullptr) {

        m_manager.addItemAttribute(parentItem,
                                   itemAttributeHandle,
                                   category,
                                   displayableName,
                                   shortDescription,
                                   detailledDescription,
                                   prototype);
    }
};

#endif // CT_METRICOUTMODELSTRUCTUREMANAGER_H
