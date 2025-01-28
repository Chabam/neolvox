#include "ct_outopfnodegroupmodel.h"

template<class OPFGroup>
CT_OutOPFNodeGroupModel<OPFGroup>::CT_OutOPFNodeGroupModel(const QString& displayableName,
                                                           const QString& shortDescription,
                                                           const QString& detailledDescription,
                                                           OPFGroup* prototype) : SuperClass(displayableName,
                                                                                             shortDescription,
                                                                                             detailledDescription,
                                                                                             prototype)
{
    m_level = 0;
}

template<class OPFGroup>
void CT_OutOPFNodeGroupModel<OPFGroup>::setOPFLevel(const quint8 &level)
{
    m_level = level;
}

template<class OPFGroup>
quint8 CT_OutOPFNodeGroupModel<OPFGroup>::opfLevel() const
{
    return m_level;
}

template<class OPFGroup>
CT_OutAbstractModel* CT_OutOPFNodeGroupModel<OPFGroup>::copy() const
{
    return new CT_OutOPFNodeGroupModel(*this);
}
