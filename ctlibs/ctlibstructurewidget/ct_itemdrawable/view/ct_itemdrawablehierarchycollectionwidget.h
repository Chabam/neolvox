#ifndef CT_ITEMDRAWABLEHIERARCHYCOLLECTIONWIDGET_H
#define CT_ITEMDRAWABLEHIERARCHYCOLLECTIONWIDGET_H

#include <QWidget>
#include <QStandardItemModel>

#include "ct_itemdrawablehierarchycollectionmodel.h"
#include "documentinterface.h"
#include "graphicsviewinterface.h"
#include "ct_model/outModel/abstract/ct_outabstractsingularitemmodel.h"

namespace Ui {
class CT_ItemDrawableHierarchyCollectionWidget;
}

/**
 * @brief A widget that can show attributes that can be selected to use it in export
 */
class CTLIBSTRUCTUREWIDGET_EXPORT CT_ItemDrawableHierarchyCollectionWidget : public QWidget
{
    Q_OBJECT

public:

    enum CloudType {
        POINTS,
        FACES,
        EDGES,
        NONE
    };

    explicit CT_ItemDrawableHierarchyCollectionWidget(CloudType cloudType, QWidget *parent = 0);
    ~CT_ItemDrawableHierarchyCollectionWidget();

    /**
     * @brief Call this method before 'setModel' to enable/disable the use of document's colors
     */
    void setCanSelectColorsOfDocuments(bool enable);

    /**
     * @brief Call this method before 'setModel' to enable/disable the use of document's normals
     */
    void setCanSelectNormalsOfDocuments(bool enable);

    /**
     * @brief Set the document manager (if exist)
     */
    void setDocumentManager(const DocumentManagerInterface *dm);

    /**
     * @brief Set the collection of items and/or attributes that can be selected too
     */
    void setModel(CT_ItemDrawableHierarchyCollectionModel *model);

    /**
     * @brief Returns true if at least one element can be selected (if at least one element is present in the selection model)
     */
    bool hasChoice() const;

    /**
     * @brief Returns true if choice can be selected by default
     */
    bool canChoiceBeSelectedByDefault() const;

    /**
     * @brief Select choice by default
     */
    bool selectChoiceByDefault();

    /**
     * @brief Returns the color cloud selected (nullptr if it was not selected)
     */
    QSharedPointer<CT_StandardColorCloudRegistered> colorCloudOfDocumentSelected() const;

    /**
     * @brief Returns the normal cloud selected (nullptr if it was not selected)
     */
    QSharedPointer<CT_StandardNormalCloudRegistered> normalCloudOfDocumentSelected() const;

    /**
     * @brief Returns itemdrawables selected
     */
    QList<CT_AbstractSingularItemDrawable*> itemDrawableSelected() const;

    /**
     * @brief Returns itemdrawables selected that can be dynamic cast to the type defined
     */
    template<typename T>
    QList<T*> itemDrawableSelectedT() const {
        QList<T*> l;

        const QList<QStandardItem*> sItems = recursiveItemsSelected(m_model.invisibleRootItem());

        if(!sItems.isEmpty()
                && (sItems.first()->data().toInt() >= 0))
        {
            for(const QStandardItem* sItem : sItems) {
                T* item = dynamic_cast<T*>(static_cast<CT_AbstractSingularItemDrawable*>(sItem->data(Qt::UserRole+2).value<void*>()));

                if(item != nullptr)
                    l.append(item);
            }
        }

        return l;
    }

    /**
     * @brief Returns models selected. Use this method if the step was not launched (results is empty)
     */
    QList<CT_OutAbstractSingularItemModel*> itemDrawableModelSelected() const;

    /**
     * @brief Returns models selected that type of itemdrawable can be dynamic cast to the type defined. Use this method if the step was not launched (results is empty)
     */
    template<typename T>
    QList<CT_OutAbstractSingularItemModel*> itemDrawableModelSelectedT() const {
        QList<CT_OutAbstractSingularItemModel*> l;

        const QList<QStandardItem*> sItems = recursiveItemsSelected(m_model.invisibleRootItem());

        if(!sItems.isEmpty()
                && (sItems.first()->data().toInt() >= 0))
        {
            for(const QStandardItem* sItem : sItems) {
                CT_OutAbstractSingularItemModel* model = static_cast<CT_OutAbstractSingularItemModel*>(sItem->data(Qt::UserRole+4).value<void*>());

                if((model != nullptr) && (dynamic_cast<T*>(model->itemDrawable()) != nullptr))
                    l.append(model);
            }
        }

        return l;
    }

private:
    Ui::CT_ItemDrawableHierarchyCollectionWidget            *ui;
    CloudType                                               m_cloudType;
    GraphicsViewInterface::ColorCloudType                   m_colorCloudType;
    GraphicsViewInterface::NormalCloudType                  m_normalCloudType;
    DocumentManagerInterface                                *m_dm;
    CT_ItemDrawableHierarchyCollectionModel                 *m_modelCollection;
    QStandardItemModel                                      m_model;
    int                                                     m_nChoice;
    CT_ItemDrawableHierarchyCollectionSelectionModel        *m_selectionModelForPointCloud;
    CT_ItemDrawableHierarchyCollectionSelectionModel        *m_selectionModelForNormalCloud;
    bool                                                    m_canSelectColorsOfDocuments;
    bool                                                    m_canSelectNormalsOfDocuments;

    void constructHeader();
    QList<QStandardItem*> createItems(const CT_ItemDrawableHierarchyCollectionSelectionModel *sm, const int &index) const;
    QList<QStandardItem*> createItemsForItemModel(const CT_ItemDrawableHierarchyCollectionSelectionModel *sm, const CT_OutAbstractItemModel *modelItemd, const int &index) const;
    QList<QStandardItem*> createItemsForItemDrawable(const CT_ItemDrawableHierarchyCollectionSelectionModel *sm, const CT_AbstractSingularItemDrawable *itemd, const int &index) const;
    QList<QStandardItem*> createItemsForColorCloud(QSharedPointer<CT_StandardColorCloudRegistered> colors) const;
    QList<QStandardItem*> createItemsForNormalCloud(QSharedPointer<CT_StandardNormalCloudRegistered> normals) const;

    void recursiveUncheckAllItemsExcept(QStandardItem *root, const QStandardItem *item, const CT_ItemDrawableHierarchyCollectionSelectionModel *sm);
    QList<QStandardItem*> recursiveItemsSelected(QStandardItem *root) const;

    /**
     * @brief Returns all checkable QStandardItem
     */
    QList<QStandardItem*> checkableItems(QStandardItem *root) const;

private slots:
    void itemChanged(QStandardItem *item);
};

#endif // CT_ITEMDRAWABLEHIERARCHYCOLLECTIONWIDGET_H
