#ifndef CT_INRESULTMODELNOTNEEDINPUTRESULT_H
#define CT_INRESULTMODELNOTNEEDINPUTRESULT_H

#include "ct_model/inModel/abstract/ct_inabstractresultmodel.h"

/**
 * @brief This class represent an input result model that don't need a matching output result model. Use it if you
 *        don't want to process input results.
 */
class CTLIBMODELS_EXPORT CT_InResultModelNotNeedInputResult : public CT_InAbstractResultModel
{
    Q_OBJECT
    typedef CT_InAbstractResultModel SuperClass;

public:
    /**
     * @brief Construct a model. The unique name, the description, etc... cannot be set because it was the same for
     *        all model of the same type. So you cannot add this model twice in the same input result model manager.
     */
    CT_InResultModelNotNeedInputResult();

    /**
      * @brief Default copy constructor
      */
    CT_InResultModelNotNeedInputResult(const CT_InResultModelNotNeedInputResult& other) = default;

    /**
     * @brief Redefined because this result has no childrens
     */
    bool visitChildrens(const ChildrenVisitor &visitor) const override;

    /**
     * @brief Redefined to returns true because this result has no childrens
     */
    bool isEmpty() const override;

    /**
     * @brief This model cannot be compared with an output model
     */
    bool canBeComparedWith(const CT_OutAbstractModel &model) const override { Q_UNUSED(model); return false; }

    // INHERITED FROM CT_InAbstractModel //
    CT_InAbstractModel* copy(bool withPossibilities) const override;
};

#endif // CT_INRESULTMODELNOTNEEDINPUTRESULT_H
