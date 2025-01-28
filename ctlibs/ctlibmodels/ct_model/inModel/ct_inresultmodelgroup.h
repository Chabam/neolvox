#ifndef CT_INRESULTMODELGROUP_H
#define CT_INRESULTMODELGROUP_H

#include "ct_model/inModel/abstract/ct_inabstractresultmodel.h"
#include "ct_model/inModel/abstract/def_ct_abstractgroupmodelin.h"
#include "ct_model/inModel/abstract/def_ct_abstractitemdrawablemodelin.h"

class CT_InZeroOrMoreGroupModel;
class CT_InStdGroupModel;
class CT_InStdSingularItemModel;
class CT_InStdItemAttributeModel;

/**
 * @brief A model that can search possibility of CT_ResultGroup.
 */
class CTLIBMODELS_EXPORT CT_InResultModelGroup : public CT_InAbstractResultModel
{
    Q_OBJECT
    typedef CT_InAbstractResultModel SuperClass;

public:
    /**
     * @brief Construct a result group model
     * @param displayableName : a displayable name for this model. If not set use "Result" by default.
     * @param shortDescription : a short description of the result you search. If empty by default use the displayable name.
     * @param recursive : true if you want that this model research recusively results in all step in the tree (go up in hierarchy) or false
     *                    if you want just research in results of the step passed in parameter of the method "searchResultsInStep...". False by default.
     * @param minimumNumberOfPossibilityToSelect : the minimum number of possibility that must be selected
     * @param maximumNumberOfPossibilityThatCanBeSelected : the maximum number of possibility that can be selected. -1 if you want no limit.
     */
    CT_InResultModelGroup(const QString& displayableName = "Result",
                          const QString& shortDescription = "",
                          bool recursive = false,
                          quint8 minimumNumberOfPossibilityToSelect = 1,
                          int maximumNumberOfPossibilityThatCanBeSelected = -1);

    /**
     * @brief Copy constructor
     * @param other : the object to copy
     * @param withPossibilities : to true if possibilities of this object and of childrens must be copied
     */
    CT_InResultModelGroup(const CT_InResultModelGroup& other, const bool& withPossibilities);

    ~CT_InResultModelGroup() override;

    /**
     * @brief Returns the root group model
     */
    DEF_CT_AbstractGroupModelIn* rootGroup() const;

    /**
     * @brief Define a custom root group model (delete the last root group model from memory if exist).
     */
    void setRootGroup(DEF_CT_AbstractGroupModelIn* rootGroup);

    /**
     * @brief Define a CT_InStdGroupModel to be the root group model (delete the last root group model from memory if exist).
     * @param groupType : The type of the group returned by the static method "staticType" of the class. MUST NOT BE EMPTY !
     * @param groupNameFromType : the name from type of the group (CT_GroupXXX::nameFromType(CT_GroupXXX::staticType())).
     * @param displayableName : displayable name that will be show in gui
     * @param shortDescription : short description of the group you search
     * @param detailledDescription : detailled description of the group you search
     * @param minPossibility : the minimum number of possibility that you want that the user select (0 if optionnal)
     * @param maxPossibility : the maximum number of possibility that the user can select (-1 if unlimited)
     * @return Returns the group model created. nullptr if it was a problem.
     */
    CT_InStdGroupModel* setStdRootGroup(const QString& groupType,
                                        const QString& groupNameFromType,
                                        const QString& displayableName = "Groupe",
                                        const QString& shortDescription = "",
                                        const QString& detailledDescription = "",
                                        const quint8& minPossibility = 1,
                                        const int& maxPossibility = -1);

    /**
     * @brief Define a CT_InZeroOrMoreGroupModel to be the root group model (delete the last root group model from memory if exist).
     * @param groupType : The type of the group returned by the static method "staticType" of the class. MUST NOT BE EMPTY !
     * @param groupNameFromType : the name from type of the group (CT_GroupXXX::nameFromType(CT_GroupXXX::staticType())).
     * @return Returns the group model created. nullptr if it was a problem.
     */
    CT_InZeroOrMoreGroupModel* setZeroOrMoreRootGroup(const QString& groupType,
                                                      const QString& groupNameFromType);

    /**
     * @brief Add a CT_InStdGroupModel to another group model.
     *
     * @param parentGroup : the parent group to use.
     * @param groupType : The type of the group returned by the static method "staticType" of the class. MUST NOT BE EMPTY !
     * @param groupNameFromType : the name from type of the group (CT_GroupXXX::nameFromType(CT_GroupXXX::staticType())).
     * @param displayableName : displayable name that will be show in gui
     * @param shortDescription : short description of the group you search
     * @param detailledDescription : detailled description of the group you search
     * @param minPossibility : the minimum number of possibility that you want that the user select (0 if optionnal)
     * @param maxPossibility : the maximum number of possibility that the user can select (-1 if unlimited)
     * @return Returns the group model created. nullptr if it was a problem.
     */
    CT_InStdGroupModel* addStdGroupModel(DEF_CT_AbstractGroupModelIn* parentGroup,
                                         const QString& groupType,
                                         const QString& groupNameFromType,
                                         const QString& displayableName = "Groupe",
                                         const QString& shortDescription = "",
                                         const QString& detailledDescription = "",
                                         const quint8& minPossibility = 1,
                                         const int& maxPossibility = -1);

    /**
     * @brief Add a CT_InStdSingularItemModel to another model (that represent a group) in the result.
     *
     * @param parentGroup : the parent group to use.
     * @param itemType : The type of the item returned by the static method "staticType" of the class. MUST NOT BE EMPTY !
     * @param itemNameFromType : the name from type of the item (CT_ItemXXX::nameFromType(CT_ItemXXX::staticType())). MUST NOT BE EMPTY !
     * @param displayableName : a displayable name (name displayed in gui for user information). Empty by default.
     * @param shortDescription : a short description (displayed in gui for user information). Empty by default.
     * @param detailledDescription : a detailled description (displayed in gui for user information). Empty by default.
     * @param minPossibility : the minimum number of possibility that must be selected
     * @param maxPossibility : the maximum number of possibility that can be selected. -1 if you want no limit.
     * @return Returns the item model created. nullptr if it was a problem.
     */
    CT_InStdSingularItemModel* addStdItemModel(DEF_CT_AbstractGroupModelIn* parentGroup,
                                               const QString& itemType,
                                               const QString& itemNameFromType,
                                               const QString& displayableName,
                                               const QString& shortDescription = "",
                                               const QString& detailledDescription = "",
                                               const quint8& minPossibility = 1,
                                               const int& maxPossibility = -1);

    /**
     * @brief Add a CT_InStdItemAttributeModel to another model (that represent a item) in the result.
     *
     * @param parentItemUniqueName : the unique name of the model of the parent item (typically DEF_....). Don't be empty !
     * @param uniqueName : unique name of the new item attribute model (typically DEF_....)
     * @param categoriesType : a list of categories type (per example QList<...>() << CT_AbstractCategory::DATA_X_DIMENSION << CT_AbstractCategory::DATA_X_RESOLUTION). The
     *                         list will be used with the logical operator 'OR'. Item attribute with a category type in the list will be accepted if value type is ok.
     * @param valueType : pass the value type that you want (per example CT_AbstractCategory::NUMBER if you want
     *                    to be generic and accept int, double, float, etc... or CT_AbstractCategory::UINT64 for a specific type)
     * @param displayableName: displayable name that will be show in gui
     * @param description : description of the group model
     * @param choiceMode : user selection mode in the input result configuration dialog. By default the user can choose only one itemdrawable type if multiple was founded.
     * @param finderMode : if item is obligatory or not. By default it must.
     *
     * @return true if the item was added to the parent group. false if the unique name already exist in the hierarchy or if the parent group was not found.
     */
    CT_InStdItemAttributeModel* addStdItemAttributeModel(DEF_CT_AbstractItemDrawableModelIn* parentItem,
                                                         const int valueType,
                                                         const QList<QString> &categoriesType,
                                                         const QString& displayableName,
                                                         const QString& shortDescription = "",
                                                         const QString& detailledDescription = "",
                                                         const quint8& minPossibility = 1,
                                                         const int& maxPossibility = -1);


    /**
     * @brief Redefined to visit the root group
     */
    bool visitChildrens(const ChildrenVisitor &visitor) const override;

    /**
     * @brief Redefined to returns true if the root group was set
     */
    bool isEmpty() const override;

    /**
     * @brief Save all necessary elements to be able to restore it with the method "restoreSettings"
     */
    void saveSettings(SettingsWriterInterface& writer) const override;

    /**
     * @brief Restore the state
     */
    bool restoreSettings(SettingsReaderInterface& reader) override;

    /**
     * @brief Copy this model with possibilities if the parameter is set to true
     */
    CT_InAbstractModel* copy(bool withPossibilities) const override;

    /**
     * @brief Redefined to returns true if the output model can be dynamic_cast to a CT_OutAbstractResultModelGroup
     */
    bool canBeComparedWith(const CT_OutAbstractModel& model) const override;

private:
    /**
     * @brief The root group
     */
    DEF_CT_AbstractGroupModelIn* m_rootGroup;
};

#endif // CT_INRESULTMODELGROUP_H
