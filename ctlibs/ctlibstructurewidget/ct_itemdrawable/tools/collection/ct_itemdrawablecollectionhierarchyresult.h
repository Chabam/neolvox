#ifndef CT_ITEMDRAWABLECOLLECTIONHIERARCHYRESULT_H
#define CT_ITEMDRAWABLECOLLECTIONHIERARCHYRESULT_H

#include "ctlibstructurewidget_global.h"
#include <QList>

class CT_AbstractSingularItemDrawable;
class CT_ResultGroup;
class CT_OutAbstractItemModel;
class CT_OutAbstractResultModel;

/**
 * @brief A collection of itemdrawable in a result that can be exported
 */
class CTLIBSTRUCTUREWIDGET_EXPORT CT_ItemDrawableCollectionHierarchyResult
{
public:
    CT_ItemDrawableCollectionHierarchyResult() { result = nullptr; modelResult = nullptr;}

    /**
     * @brief Contains real result if the element is created from step that have results
     */
    CT_ResultGroup                              *result;

    /**
     * @brief The collection contains real itemDrawable if the element is created from step that have results
     */
    QList<CT_AbstractSingularItemDrawable*>     collection;

    /**
     * @brief Contains model of result if the element is created from step that not have results (step has not been launched)
     */
    CT_OutAbstractResultModel                   *modelResult;

    /**
     * @brief The collection contains model of itemDrawable if the element is created from step that not have results (step has not been launched)
     */
    QList<CT_OutAbstractItemModel*>             modelsCollection;
};

#endif // CT_ITEMDRAWABLECOLLECTIONHIERARCHYRESULT_H
