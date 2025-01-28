#include "ct_opfmeshmodel.h"

CT_TYPE_IMPL_INIT_MACRO(CT_OPFMeshModel)

CT_OPFMeshModel::CT_OPFMeshModel() : SuperClass(),
    m_dUp(1.0),
    m_dDown(1.0)
{
}

CT_OPFMeshModel::CT_OPFMeshModel(CT_Mesh* mesh) : SuperClass(mesh),
    m_dUp(1.0),
    m_dDown(1.0)
{
}

void CT_OPFMeshModel::setDUp(double dUp)
{
    m_dUp = dUp;
}

void CT_OPFMeshModel::setDDown(double dDown)
{
    m_dDown = dDown;
}

double CT_OPFMeshModel::dUp() const
{
    return m_dUp;
}

double CT_OPFMeshModel::dDown() const
{
    return m_dDown;
}
