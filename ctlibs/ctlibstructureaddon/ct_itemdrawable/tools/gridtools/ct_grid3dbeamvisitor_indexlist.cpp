#include "ct_grid3dbeamvisitor_indexlist.h"

CT_Grid3DBeamVisitor_IndexList::CT_Grid3DBeamVisitor_IndexList() : CT_AbstractGrid3DBeamVisitor()
{
}

void CT_Grid3DBeamVisitor_IndexList::visit(const size_t &index, const CT_Beam *beam)
{
    Q_UNUSED(beam);
    _indices.append(index);
}
