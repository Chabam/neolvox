#ifndef CT_OUTSTDSINGULARITEMMODEL_H
#define CT_OUTSTDSINGULARITEMMODEL_H

#include "ct_model/outModel/abstract/def_ct_abstractitemdrawablemodelout.h"

/**
 * @brief Use this class if you will inform that you want create a singular item and add it to an
 *        other OUTPUT model that represent a group
 */
template<class SingularItem>
class CT_OutStdSingularItemModel : public DEF_CT_AbstractItemDrawableModelOut
{
    using SuperClass = DEF_CT_AbstractItemDrawableModelOut;

public:

    using PublicSuperClass = DEF_CT_AbstractItemDrawableModelOut;

    /**
     * @brief Construct a model
     * @param displayableName : a displayable name (name displayed in gui for user information). Empty by default.
     * @param shortDescription : a short description (displayed in gui for user information). Empty by default.
     * @param detailledDescription : a detailled description (displayed in gui for user information). Empty by default.
     */
    CT_OutStdSingularItemModel(const QString& displayableName = "",
                               const QString& shortDescription = "",
                               const QString& detailledDescription = "",
                               SingularItem* prototype = nullptr);

    /**
     * @brief Copy constructor
     * @warning The parent model will not be copied !
     */
    CT_OutStdSingularItemModel(const CT_OutStdSingularItemModel& other);

    /**
     * @brief Returns a copy of this model
     */
    CT_OutAbstractModel* copy() const override;

protected:
    /**
     * @brief Redefined to add default attributes of the item in attribute model (childrens) collection of this model
     */
    bool finalize() override;
};

#include "ct_outstdsingularitemmodel.hpp"

#endif // CT_OUTSTDSINGULARITEMMODEL_H
