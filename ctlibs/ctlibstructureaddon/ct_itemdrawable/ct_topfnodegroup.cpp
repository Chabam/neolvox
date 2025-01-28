#include "ct_topfnodegroup.h"

#include "ct_model/outModel/ct_outopfnodegroupmodel.h"

CT_TYPE_IMPL_INIT_MACRO(CT_TOPFNodeGroup)

CT_TOPFNodeGroup::CT_TOPFNodeGroup() : CT_TNodeGroup()
{
}

void CT_TOPFNodeGroup::setOPFMatrix(const QMatrix4x4 &matrix)
{
    m_opfMatrix = matrix;
}

QMatrix4x4 CT_TOPFNodeGroup::opfMatrix() const
{
    return m_opfMatrix;
}

CT_OutOPFNodeGroupModel<CT_TOPFNodeGroup>* CT_TOPFNodeGroup::opfModel() const
{
    return static_cast<CT_OutOPFNodeGroupModel<CT_TOPFNodeGroup>*>(model());
}

void CT_TOPFNodeGroup::setOPFID(const size_t &id)
{
    setId(static_cast<qint32>(id));
}
