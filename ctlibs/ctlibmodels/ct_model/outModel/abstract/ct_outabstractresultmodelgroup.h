#ifndef CT_OUTABSTRACTRESULTMODELGROUP_H
#define CT_OUTABSTRACTRESULTMODELGROUP_H

#include "ct_outabstractresultmodel.h"
#include "ct_model/outModel/abstract/def_ct_abstractgroupmodelout.h"

/**
 * @brief Base class for result group model
 */
class CTLIBMODELS_EXPORT CT_OutAbstractResultModelGroup : public CT_OutAbstractResultModel
{
    Q_OBJECT
    typedef CT_OutAbstractResultModel SuperClass;

public:
    CT_OutAbstractResultModelGroup(const QString& displayableName);

    /**
     * @brief Copy constructor
     * @warning The parent model and the prototype item will not be copied !
     */
    CT_OutAbstractResultModelGroup(const CT_OutAbstractResultModelGroup& other) = default;

    /**
     * @brief Return the root model that represent a group
     */
    virtual DEF_CT_AbstractGroupModelOut* rootGroup() const = 0;

    /**
     * @brief Set the root model that represent a group (delete the last root group from memory if exist).
     */
    virtual void setRootGroup(DEF_CT_AbstractGroupModelOut* rootGroup) = 0;

    /**
     * @brief Redefined to visit the root group
     */
    bool visitChildrens(const ChildrenVisitor& visitor) const override;

    /**
     * @brief Redefined to returns true if it was a root group
     */
    bool isEmpty() const override;
};

#endif // CT_OUTABSTRACTRESULTMODELGROUP_H
