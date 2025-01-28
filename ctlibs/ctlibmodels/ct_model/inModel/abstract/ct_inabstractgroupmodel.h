#ifndef CT_INABSTRACTGROUPMODEL_H
#define CT_INABSTRACTGROUPMODEL_H

#include "ct_model/inModel/abstract/ct_inabstractitemmodel.h"
/**
 * @brief Represent a model for an input group. With a input group model you can :
 *
 *      - add/remove/clear input groups and singular items
 *      - find/clear possibilities (a list of input models that match with model researched) in output item model
 *      - save/restore this model
 */
class CTLIBMODELS_EXPORT CT_InAbstractGroupModel : public CT_InAbstractItemModel
{
    Q_OBJECT
    typedef CT_InAbstractItemModel SuperClass;

public:
    /**
     * @brief Construct a group model
     * @param displayableName : a displayable name
     */
    CT_InAbstractGroupModel(const QString& displayableName);

    /**
     * @brief Copy constructor
     * @param other : the object to copy
     */
    CT_InAbstractGroupModel(const CT_InAbstractGroupModel& other);

    /**
     * @brief Redefined to returns true if the output model can be dynamic_cast to a CT_OutAbstractGroupModel
     */
    bool canBeComparedWith(const CT_OutAbstractModel &model) const override;
};

#endif // CT_INABSTRACTGROUPMODEL_H
