#ifndef CT_OUTRESULTMODELGROUP_H
#define CT_OUTRESULTMODELGROUP_H

#include "ct_model/outModel/abstract/ct_outabstractresultmodelgroup.h"
#include "ct_model/outModel/ct_outstdgroupmodel.h"

/**
 * @brief Use this class if you want to create a NEW result in your step.
 */
template<class ResultGroup>
class CT_OutResultModelGroup : public CT_OutAbstractResultModelGroup
{
    typedef CT_OutAbstractResultModelGroup SuperClass;

public:
    /**
     * @brief Construct an output model that represent a result group
     * @param rootGroupModel : the root model that represent a group.
     * @param resultName: the name that will be set to the result. Empty by default.
     * @param displayableName : a displayable name (used in gui for user information). Empty by default.
     * @param shortDescription : a short description of the result you search. Empty by default.
     */
    CT_OutResultModelGroup(DEF_CT_AbstractGroupModelOut* rootGroupModel,
                           const QString& resultName = "",
                           const QString& displayableName = "",
                           const QString& shortDescription = "");
    ~CT_OutResultModelGroup();

    /**
     * @brief Copy constructor
     * @warning The parent model will not be copied !
     */
    CT_OutResultModelGroup(const CT_OutResultModelGroup& other);

    /**
     * @brief Return the root model that represent a group
     */
    DEF_CT_AbstractGroupModelOut* rootGroup() const override;

    /**
     * @brief Set the root model that represent a group (delete the last root group from memory if exist).
     */
    void setRootGroup(DEF_CT_AbstractGroupModelOut* rootGroup) override;

    /**
     * @brief Set the root group (delete the last root group from memory if exist).
     * @param prototype : the prototype
     * @param displayableName : a displayable name (used in gui for user information).
     * @return Returns the created model
     */
    template<class StdGroup>
    CT_OutStdGroupModel<StdGroup>* setRootGroup(IGroupForModel* prototype = new StdGroup(),
                                                const QString& displayableName = QString());

    // INHERITED FROM CT_OutAbstractResultModel
    IResultForModel* createResult() override;

    /**
     * @brief Returns a copy of this model
     */
    CT_OutAbstractModel* copy() const override;

private:
    /**
     * @brief The root group
     */
    DEF_CT_AbstractGroupModelOut*   m_rootGroup;
};

#include "ct_model/outModel/ct_outresultmodelgroup.hpp"

#endif // CT_OUTRESULTMODELGROUP_H
