#ifndef CT_INSTDSINGULARITEMMODEL_H
#define CT_INSTDSINGULARITEMMODEL_H

#include "ct_model/inModel/abstract/def_ct_abstractitemdrawablemodelin.h"

/**
 * @brief Use this class to search a singular item (generic or specific)
 */
class CTLIBMODELS_EXPORT CT_InStdSingularItemModel : public DEF_CT_AbstractItemDrawableModelIn
{
    Q_OBJECT
    using SuperClass = DEF_CT_AbstractItemDrawableModelIn;

public:

    template<class SItem>
    static CT_InStdSingularItemModel* create(const QString& displayableName = "",
                                             const QString& shortDescription = "",
                                             const QString& detailledDescription = "",
                                             quint8 minimumNumberOfPossibilityToSelect = 1,
                                             int maximumNumberOfPossibilityThatCanBeSelected = 1) {

        return new CT_InStdSingularItemModel(SItem::staticType(),
                                             SItem::nameFromType(SItem::staticType()),
                                             displayableName,
                                             shortDescription,
                                             detailledDescription,
                                             minimumNumberOfPossibilityToSelect,
                                             maximumNumberOfPossibilityThatCanBeSelected);
    }

    /**
     * @brief Construct a model that search a singular item
     * @param itemType : the type of the item (CT_ItemXXX::staticType()). Item can be abstract if you want to be generic. MUST NOT BE EMPTY !
     * @param itemNameFromType : the name from type of the item (CT_ItemXXX::nameFromType(CT_ItemXXX::staticType())). MUST NOT BE EMPTY !
     * @param displayableName : a displayable name (name displayed in gui for user information). Empty by default.
     * @param shortDescription : a short description (displayed in gui for user information). Empty by default.
     * @param detailledDescription : a detailled description (displayed in gui for user information). Empty by default.
     * @param minimumNumberOfPossibilityToSelect : the minimum number of possibility that must be selected
     * @param maximumNumberOfPossibilityThatCanBeSelected : the maximum number of possibility that can be selected. -1 if you want no limit.
     */
    CT_InStdSingularItemModel(const QString& itemType,
                              const QString& itemNameFromType,
                              const QString& displayableName = "",
                              const QString& shortDescription = "",
                              const QString& detailledDescription = "",
                              quint8 minimumNumberOfPossibilityToSelect = 1,
                              int maximumNumberOfPossibilityThatCanBeSelected = 1);

    /**
     * @brief Copy constructor
     * @param other : the object to copy
     * @param withPossibilities : to true if possibilities of this object and of childrens must be copied
     */
    CT_InStdSingularItemModel(const CT_InStdSingularItemModel& other, const bool& withPossibilities);

    /**
     * @brief Copy this model
     * @param withPossibilities : to true if possibilities of this object and of childrens must be copied
     */
    CT_InAbstractModel* copy(bool withPossibilities) const override;

    /**
     * @brief Redefined to returns true if the output model can be dynamic_cast to a CT_OutAbstractSingularItemModel
     */
    bool canBeComparedWith(const CT_OutAbstractModel& model) const override;
};

#endif // CT_INSTDSINGULARITEMMODEL_H
