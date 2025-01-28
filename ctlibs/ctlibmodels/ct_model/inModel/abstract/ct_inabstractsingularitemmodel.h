#ifndef CT_INABSTRACTSINGULARITEMMODEL_H
#define CT_INABSTRACTSINGULARITEMMODEL_H

#include "ct_model/inModel/abstract/ct_inabstractitemmodel.h"

#include <QString>

class CT_InAbstractItemAttributeModel;

/**
 * @brief Represent a model for an INPUT singular item. A singular item has item attributes for children.
 */
class CTLIBMODELS_EXPORT CT_InAbstractSingularItemModel : public CT_InAbstractItemModel
{
    Q_OBJECT
    using SuperClass = CT_InAbstractItemModel;

public:
    using AttributeVisitor = std::function<bool(const CT_InAbstractItemAttributeModel*)>;

    /**
     * @brief Construct a singular item model
     * @param displayableName : a displayable name
     */
    CT_InAbstractSingularItemModel(const QString& displayableName);

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

#endif // CT_INABSTRACTSINGULARITEMMODEL_H
