#ifndef CT_INSTDITEMATTRIBUTEMODEL_H
#define CT_INSTDITEMATTRIBUTEMODEL_H

#include "ct_model/inModel/abstract/def_ct_inabstractitemattributemodel.h"

/**
 * @brief Use this class if you want to search a item attribute in a OUTPUT model
 */
class CTLIBMODELS_EXPORT CT_InStdItemAttributeModel : public DEF_CT_InAbstractIAModel
{
    Q_OBJECT
    typedef DEF_CT_InAbstractIAModel SuperClass;

public:

    template<typename CategoryType>
    static CT_InStdItemAttributeModel* create(int valueType,
                                              const CategoryType& categoryType,
                                              const QString& displayableName = "",
                                              const QString& shortDescription = "",
                                              const QString& detailledDescription = "",
                                              quint8 minimumNumberOfPossibilityToSelect = 1,
                                              int maximumNumberOfPossibilityThatCanBeSelected = 1) {

        return new CT_InStdItemAttributeModel(valueType,
                                              categoryType,
                                              displayableName,
                                              shortDescription,
                                              detailledDescription,
                                              minimumNumberOfPossibilityToSelect,
                                              maximumNumberOfPossibilityThatCanBeSelected);
    }

    /**
     * @brief Construct a model that represent a item attribute
     * @param categoryType : the category type (per example CT_AbstractCategory::DATA_ANGLE)
     * @param valueType : pass the value type that you want (per example CT_AbstractCategory::NUMBER if you want
     *                    to be generic and accept int, double, float, etc... or CT_AbstractCategory::UINT64 for a specific type)
     * @param displayableName : a displayable name (name displayed in gui for user information). Empty by default.
     * @param shortDescription : a short description (displayed in gui for user information). Empty by default.
     * @param detailledDescription : a detailled description (displayed in gui for user information). Empty by default.
     * @param minimumNumberOfPossibilityToSelect : the minimum number of possibility that must be selected
     * @param maximumNumberOfPossibilityThatCanBeSelected : the maximum number of possibility that can be selected. -1 if you want no limit.
     */
    CT_InStdItemAttributeModel(int valueType,
                               const QString& categoryType,
                               const QString& displayableName = "",
                               const QString& shortDescription = "",
                               const QString& detailledDescription = "",
                               quint8 minimumNumberOfPossibilityToSelect = 1,
                               int maximumNumberOfPossibilityThatCanBeSelected = 1);

    /**
     * @brief Construct a model that represent a item attribute
     * @param categoriesType : a list of categories type (per example QList<...>() << CT_AbstractCategory::DATA_X_DIMENSION << CT_AbstractCategory::DATA_X_RESOLUTION). The
     *                         list will be used with the logical operator 'OR'. Item attribute with a category type in the list will be accepted if value type is ok.
     * @param valueType : pass the value type that you want (per example CT_AbstractCategory::NUMBER if you want
     *                    to be generic and accept int, double, float, etc... or CT_AbstractCategory::UINT64 for a specific type)
     * @param displayableName : a displayable name (name displayed in gui for user information). Empty by default.
     * @param shortDescription : a short description (displayed in gui for user information). Empty by default.
     * @param detailledDescription : a detailled description (displayed in gui for user information). Empty by default.
     * @param minimumNumberOfPossibilityToSelect : the minimum number of possibility that must be selected
     * @param maximumNumberOfPossibilityThatCanBeSelected : the maximum number of possibility that can be selected. -1 if you want no limit.
     */
    CT_InStdItemAttributeModel(int valueType,
                               const QList<QString>& categoriesType,
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
    CT_InStdItemAttributeModel(const CT_InStdItemAttributeModel& other, const bool& withPossibilities);

    /**
     * @brief Copy this model
     * @param withPossibilities : to true if possibilities of this object and of childrens must be copied
     */
    CT_InAbstractModel* copy(bool withPossibilities) const override;
};

#endif // CT_INSTDITEMATTRIBUTEMODEL_H
