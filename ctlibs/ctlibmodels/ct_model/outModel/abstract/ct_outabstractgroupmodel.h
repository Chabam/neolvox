#ifndef CT_OUTABSTRACTGROUPMODEL_H
#define CT_OUTABSTRACTGROUPMODEL_H

#include "ct_model/outModel/abstract/ct_outabstractitemmodel.h"

/**
 * @brief Represent an output model for a group. With a output group model you can :
 *
 *      - add/remove/clear output groups and singular items
 */
class CTLIBMODELS_EXPORT CT_OutAbstractGroupModel : public CT_OutAbstractItemModel
{
    Q_OBJECT
    typedef CT_OutAbstractItemModel SuperClass;

public:
    /**
     * @brief Construct an output model that has a unique index (by default at -1 so it was invalid) and a displayable name
     * @param displayableName : a displayable name
     */
    CT_OutAbstractGroupModel(const QString& displayableName);

    /**
     * @brief Copy constructor
     * @warning The parent model will not be copied !
     */
    CT_OutAbstractGroupModel(const CT_OutAbstractGroupModel& other);
};

#endif // CT_OUTABSTRACTGROUPMODEL_H
