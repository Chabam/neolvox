#ifndef LVOX2_COUNTVISITOR_H
#define LVOX2_COUNTVISITOR_H

#include "ct_itemdrawable/tools/gridtools/ct_abstractgrid3dbeamvisitor.h"
#include "ct_itemdrawable/ct_grid3d.h"

class LVOX2_CountVisitor : public CT_AbstractGrid3DBeamVisitor
{
public:

    LVOX2_CountVisitor(CT_Grid3D<int> *grid);

    virtual void visit(const size_t &index, const CT_Beam *beam);

private:
    CT_Grid3D<int>*     _grid;
};

#endif // LVOX2_COUNTVISITOR_H
