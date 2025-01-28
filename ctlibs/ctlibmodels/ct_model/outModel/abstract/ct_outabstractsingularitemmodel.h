#ifndef CT_OUTABSTRACTSINGULARITEMMODEL_H
#define CT_OUTABSTRACTSINGULARITEMMODEL_H

#include "ct_model/outModel/abstract/ct_outabstractitemmodel.h"

class CT_OutAbstractItemAttributeModel;

/**
 * @brief Represent an output model for a singular item.
 */
class CTLIBMODELS_EXPORT CT_OutAbstractSingularItemModel : public CT_OutAbstractItemModel
{
    Q_OBJECT
    typedef CT_OutAbstractItemModel SuperClass;

public:
    typedef std::function<bool(const CT_OutAbstractItemAttributeModel*)> AttributeVisitor;

    /**
     * @brief Construct an output model for a singular item
     * @param displayableName : a displayable name
     */
    CT_OutAbstractSingularItemModel(const QString& displayableName);

    /**
     * @brief Copy constructor
     * @warning The parent model will not be copied !
     */
    CT_OutAbstractSingularItemModel(const CT_OutAbstractSingularItemModel& other) = default;

    /**
     * @brief Call it to visit the attributes of this model.
     *        Because nature of childrens can be different this class doesn't contains attributes
     *        so the visit is deleguate to subclass.
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  CT_InAbstractItemAttributeModel and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    virtual bool visitAttributes(const AttributeVisitor& visitor) const = 0;
};

#endif // CT_OUTABSTRACTSINGULARITEMMODEL_H
