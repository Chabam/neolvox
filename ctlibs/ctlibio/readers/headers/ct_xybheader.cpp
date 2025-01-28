#include "ct_xybheader.h"

CT_DEFAULT_IA_INIT(CT_XYBHeader)

CT_XYBHeader::CT_XYBHeader() : SuperClass(),
    m_scanCenterX(0),
    m_scanCenterY(0),
    m_scanCenterZ(0),
    m_nrows(0),
    m_ncols(0),
    m_offset(0)
{
}

void CT_XYBHeader::setScanCenter(const double& cx, const double& cy, const double& cz)
{
    m_scanCenterX = cx;
    m_scanCenterY = cy;
    m_scanCenterZ = cz;
}

void CT_XYBHeader::setNRows(const int& n)
{
    m_nrows = n;
}

void CT_XYBHeader::setNCols(const int& n)
{
    m_ncols = n;
}

void CT_XYBHeader::setOffset(const int& offset)
{
    m_offset = offset;
}
