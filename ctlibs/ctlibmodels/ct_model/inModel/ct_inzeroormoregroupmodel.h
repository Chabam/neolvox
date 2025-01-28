#ifndef CT_INZEROORMOREGROUPMODEL_H
#define CT_INZEROORMOREGROUPMODEL_H

#include "ct_model/inModel/ct_instdgroupmodel.h"

/*!
 * Use this class to search children anywhere in the tree.
 *
 * Typically you will use this class if you want to search a group with childrens anywhere in a tree :
 *
 * INPUT example :
 *
 * IR (CT_InResultModelGroup)
 * |--- IG (CT_InZeroOrMoreGroupModel)
 *      |--- IG (CT_InStdGroupModel / Obligatory)
 *           |--- II (CT_InStdSingularItem => CT_Circle::staticType() / min = 1 / max = 1)
 *
 * OUTPUT example 1 :
 *
 * OR (Result = 0)
 * |--- OG (Group = 1)
 *      |--- OI (CT_Circle = 2)
 *      |--- OI (CT_PointCluster = 3)
 *
 * OUTPUT example 2:
 *
 * OR (Result = 0)
 * |--- OG (Group = 1)
 *      |--- OG (Group = 2)
 *           |--- OI (CT_Circle = 3)
 *
 * OUTPUT example 3:
 *
 * OR (Result = 0)
 * |--- OG (Group = 1)
 *      |--- OG (Group = 2)
 *      |    |--- OI (CT_Circle = 3)
 *      |--- OI (CT_Circle = 4)
 *
 * For the first output example the CT_InStdGroupModel will find 1
 * For the second output example the CT_InStdGroupModel will find 2
 * For the second output example the CT_InStdGroupModel will find 1 and 2 but the user can choose only one possibility because the circle is set with max == 1
 */
class CTLIBMODELS_EXPORT CT_InZeroOrMoreGroupModel : public CT_InStdGroupModel
{
    Q_OBJECT
    typedef CT_InStdGroupModel SuperClass;

public:
    template<class Group>
    static CT_InZeroOrMoreGroupModel* create() {
        return new CT_InZeroOrMoreGroupModel(Group::staticType(), Group::nameFromType(Group::staticType()));
    }

    /**
     * @brief Construct a model that search a group anywhere in the hierarchy
     */
    CT_InZeroOrMoreGroupModel(const QString& groupType,
                              const QString& groupNameFromType);

    /**
     * @brief Copy constructor
     * @param other : the object to copy
     * @param withPossibilities : to true if possibilities of this object and of childrens must be copied
     */
    CT_InZeroOrMoreGroupModel(const CT_InZeroOrMoreGroupModel& other, const bool& withPossibilities);

    /**
     * @brief Copy this model
     * @param withPossibilities : to true if possibilities of this object and of childrens must be copied
     */
    CT_InAbstractModel* copy(bool withPossibilities) const override;

protected:
    /**
     * @brief Redefined to returns true because this model must be compared with children of the current output model
     */
    bool mustCompareThisWithChildrenOfOutModel() const override;

    /**
     * @brief Redefined to returns true because the current output model must be compared with children of this input model
     */
    bool mustCompareOutModelWithChildrenOfThisModel() const override;

    /**
     * @brief Redefined to returns false because all childrens in this group can be in separate parents
     */
    bool mustAllObligatoryChildrensFoundAtLeastOnePossibilityInTheSameParent() const override;

    /**
     * @brief Redefined to change the minimum and maximum number of possibility to select
     */
    void groupAdded(const DEF_CT_AbstractGroupModelIn *group) override;

    /**
     * @brief Redefined to change the minimum and maximum number of possibility to select
     */
    void itemAdded(const CT_InAbstractSingularItemModel *item) override;

    /**
     * @brief Redefined to change the minimum and maximum number of possibility to select
     */
    void groupRemoved(const DEF_CT_AbstractGroupModelIn *group) override;

    /**
     * @brief Redefined to change the minimum and maximum number of possibility to select
     */
    void itemRemoved(const CT_InAbstractSingularItemModel *item) override;
};

#endif // CT_INZEROORMOREGROUPMODEL_H
