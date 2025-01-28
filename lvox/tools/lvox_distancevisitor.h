#ifndef LVOX_DISTANCEVISITOR_H
#define LVOX_DISTANCEVISITOR_H

#include "ct_itemdrawable/tools/gridtools/ct_abstractgrid3dbeamvisitor.h"
#include "ct_itemdrawable/ct_grid3d.h"

class LVOX_DistanceVisitor : public CT_AbstractGrid3DBeamVisitor
{
public:

    LVOX_DistanceVisitor(CT_Grid3D<float> *grid);

    virtual void visit(const size_t &index, const CT_Beam *beam);

private:
    CT_Grid3D<float>*  _grid;
};

#endif // LVOX_DISTANCEVISITOR_H
