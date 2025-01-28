#ifndef CT_INSTDGROUPMODEL_H
#define CT_INSTDGROUPMODEL_H

#include "ct_model/inModel/abstract/def_ct_abstractgroupmodelin.h"

/*!
 *  Use this class to search a group. Add other model if you want to search a group with childrens.
 */
class CTLIBMODELS_EXPORT CT_InStdGroupModel : public DEF_CT_AbstractGroupModelIn
{
    Q_OBJECT
    typedef DEF_CT_AbstractGroupModelIn SuperClass;

public:
    template<class Group>
    static CT_InStdGroupModel* create(const QString& displayableName = "Group",
                                      const QString& shortDescription = "",
                                      const QString& detailledDescription = "",
                                      quint8 minimumNumberOfPossibilityToSelect = 1,
                                      int maximumNumberOfPossibilityThatCanBeSelected = -1) {

        return new CT_InStdGroupModel(Group::staticType(),
                                      Group::nameFromType(Group::staticType()),
                                      displayableName,
                                      shortDescription,
                                      detailledDescription,
                                      minimumNumberOfPossibilityToSelect,
                                      maximumNumberOfPossibilityThatCanBeSelected);
    }

    /**
     * @brief Construct a model that search a group
     * @param groupType : the type of the group (CT_GroupXXX::staticType). The group can be abstract if you want to be generic. MUST NOT BE EMPTY !
     * @param groupNameFromType : the name from type of the group (CT_GroupXXX::nameFromType(CT_GroupXXX::staticType())). MUST NOT BE EMPTY !
     * @param displayableName : a displayable name (name displayed in gui for user information). "Group" by default.
     * @param short description : a short description (displayed in gui for user information). Empty by default.
     * @param detailled description : a detailled description (displayed in gui for user information). Empty by default.
     * @param minimumNumberOfPossibilityToSelect : the minimum number of possibility that must be selected
     * @param maximumNumberOfPossibilityThatCanBeSelected : the maximum number of possibility that can be selected. -1 if you want no limit.
     */
    CT_InStdGroupModel(const QString& groupType,
                       const QString& groupNameFromType,
                       const QString& displayableName = "Group",
                       const QString& shortDescription = "",
                       const QString& detailledDescription = "",
                       quint8 minimumNumberOfPossibilityToSelect = 1,
                       int maximumNumberOfPossibilityThatCanBeSelected = -1);

    /**
     * @brief Copy constructor
     * @param other : the object to copy
     * @param withPossibilities : to true if possibilities of this object and of childrens must be copied
     */
    CT_InStdGroupModel(const CT_InStdGroupModel& other, const bool& withPossibilities);

    /**
     * @brief Copy this model
     * @param withPossibilities : to true if possibilities of this object and of childrens must be copied
     */
    CT_InAbstractModel* copy(bool withPossibilities) const override;
};

#endif // CT_INSTDGROUPMODEL_H
