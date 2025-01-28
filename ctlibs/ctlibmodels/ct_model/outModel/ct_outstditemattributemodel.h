#ifndef CT_OUTSTDITEMATTRIBUTEMODEL_H
#define CT_OUTSTDITEMATTRIBUTEMODEL_H

#include "ct_model/outModel/abstract/def_ct_outabstractitemattributemodel.h"
#include "ct_categories/tools/ct_categorymanager.h"

/**
 * @brief Use this class if you will create a item attribute and add it to an
 *        other output model that represent a singular item
 */
template<class ItemAttribute>
class CT_OutStdItemAttributeModel : public DEF_CT_OutAbstractIAModel
{
    using SuperClass = DEF_CT_OutAbstractIAModel;

public:

    using PublicSuperClass = DEF_CT_OutAbstractIAModel;

    /**
     * @brief Construct an output model that has a unique index (by default at -1 so it was invalid)
     * @param category : the category to set to the prototype that will be created in the constructor.
     * @param displayableName : a displayable name
     * @param shortDescription : a short description
     */
    CT_OutStdItemAttributeModel(const ICategoryForModel* category,
                                const QString& displayableName = "",
                                const QString& shortDescription = "",
                                const QString& detailledDescription = "",
                                ItemAttribute* prototype = nullptr);

    /**
     * @brief Construct an output model that has a unique index (by default at -1 so it was invalid)
     * @param categoryUniqueName : the unique name of the category to find it. The category if founded will be set to the prototype that will be created in the constructor.
     * @param displayableName : a displayable name
     * @param shortDescription : a short description
     */
    CT_OutStdItemAttributeModel(const QString& categoryUniqueName,
                                const QString& displayableName = "",
                                const QString& shortDescription = "",
                                const QString& detailledDescription = "",
                                ItemAttribute* prototype = nullptr);
    /**
     * @brief Copy constructor
     * @warning The parent model will not be copied !
     */
    CT_OutStdItemAttributeModel(const CT_OutStdItemAttributeModel& other) = default;

    /**
     * @brief Returns a copy of this model
     */
    CT_OutAbstractModel* copy() const override;
};

#include "ct_outstditemattributemodel.hpp"

#endif // CT_OUTSTDITEMATTRIBUTEMODEL_H
