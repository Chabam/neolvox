#include "ct_model/outModel/ct_outresultmodelgroup.h"

template<class ResultGroup>
CT_OutResultModelGroup<ResultGroup>::CT_OutResultModelGroup(DEF_CT_AbstractGroupModelOut* rootGroupModel,
                                                            const QString& resultName,
                                                            const QString& displayableName,
                                                            const QString& shortDescription) : SuperClass(displayableName),
    m_rootGroup(rootGroupModel)
{
    setResultName(resultName);
    setShortDescription(shortDescription);
    setPrototype(new ResultGroup());

    if(m_rootGroup != nullptr)
        m_rootGroup->setParentModel(this);
}

template<class ResultGroup>
CT_OutResultModelGroup<ResultGroup>::~CT_OutResultModelGroup()
{
    delete m_rootGroup;
}

template<class ResultGroup>
CT_OutResultModelGroup<ResultGroup>::CT_OutResultModelGroup(const CT_OutResultModelGroup& other) : SuperClass(other),
    m_rootGroup(static_cast<DEF_CT_AbstractGroupModelOut*>(other.m_rootGroup->copy()))
{
    m_rootGroup->setParentModel(this);
}

template<class ResultGroup>
DEF_CT_AbstractGroupModelOut* CT_OutResultModelGroup<ResultGroup>::rootGroup() const
{
    return m_rootGroup;
}

template<class ResultGroup>
void CT_OutResultModelGroup<ResultGroup>::setRootGroup(DEF_CT_AbstractGroupModelOut* rootGroup)
{
    delete m_rootGroup;

    m_rootGroup = rootGroup;

    if(m_rootGroup != nullptr)
        m_rootGroup->setParentModel(this);
}

template<class ResultGroup>
template<class StdGroup>
CT_OutStdGroupModel<StdGroup>* CT_OutResultModelGroup<ResultGroup>::setRootGroup(IGroupForModel* prototype,
                                                                                 const QString& displayableName)
{
    CT_OutStdGroupModel<StdGroup>* model = new CT_OutStdGroupModel<StdGroup>(prototype,
                                                                             displayableName);
    setRootGroup(model);

    return model;
}

template<class ResultGroup>
IResultForModel* CT_OutResultModelGroup<ResultGroup>::createResult()
{
    ResultGroup* res = new ResultGroup();
    res->setModel(this);
    res->resultToolForModel()->setResultName(resultName());

    this->setResult(res);

    return res;
}

template<class ResultGroup>
CT_OutAbstractModel* CT_OutResultModelGroup<ResultGroup>::copy() const
{
    return new CT_OutResultModelGroup(*this);
}
