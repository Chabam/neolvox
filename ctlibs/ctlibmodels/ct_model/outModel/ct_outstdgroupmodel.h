#ifndef CT_OUTSTDGROUPMODEL_H
#define CT_OUTSTDGROUPMODEL_H

#include "ct_model/outModel/abstract/def_ct_abstractgroupmodelout.h"

/**
 * @brief Use this class if you will inform that you want create a group and add it to an
 *        other output model that represent a group or a result
 */
template<class StdGroup>
class CT_OutStdGroupModel : public DEF_CT_AbstractGroupModelOut
{
    using SuperClass = DEF_CT_AbstractGroupModelOut;

public:
    using PublicSuperClass = DEF_CT_AbstractGroupModelOut;

    /**
     * @brief Construct a model
     * @param displayableName : a displayable name (name displayed in gui for user information). Empty by default.
     * @param shortDescription : a short description (displayed in gui for user information). Empty by default.
     * @param detailledDescription : a detailled description (displayed in gui for user information). Empty by default.
     */
    CT_OutStdGroupModel(const QString& displayableName = "Group",
                        const QString& shortDescription = "",
                        const QString& detailledDescription = "",
                        StdGroup* prototype = nullptr);

    /**
     * @brief Copy constructor
     * @warning The parent model will not be copied !
     */
    CT_OutStdGroupModel(const CT_OutStdGroupModel& other);

    /**
     * @brief Find a group by it's unique index
     * @param uniqueIndex : the unique index of the group to search
     * @return Returns nullptr if group was not founded, otherwise returns the group with the unique index specified
     */
    SuperClass::GroupType* findGroup(const int& uniqueIndex) const;

    /**
     * @brief Find an item by it's unique index
     * @param uniqueIndex : the unique index of the item to search
     * @return Returns nullptr if item was not founded, otherwise returns the item with the unique index specified
     */
    SuperClass::ItemType* findItem(const int& uniqueIndex) const;

    /**
     * @brief Returns a copy of this model
     */
    CT_OutAbstractModel* copy() const override;
};

#include "ct_outstdgroupmodel.hpp"

#endif // CT_OUTSTDGROUPMODEL_H
