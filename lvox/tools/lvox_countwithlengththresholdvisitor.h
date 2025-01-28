#ifndef LVOX_COUNTWITHLENGTHTHRESHOLDVISITOR_H
#define LVOX_COUNTWITHLENGTHTHRESHOLDVISITOR_H

#include "ct_itemdrawable/tools/gridtools/ct_abstractgrid3dbeamvisitor.h"
#include "ct_itemdrawable/ct_grid3d.h"

class LVOX_CountWithLengthThresholdVisitor : public CT_AbstractGrid3DBeamVisitor
{
public:

    LVOX_CountWithLengthThresholdVisitor(CT_Grid3D<int> *grid, double threshold);

    virtual void visit(const size_t &index, const CT_Beam *beam);

private:
    CT_Grid3D<int>*     _grid;
    double              _threshold;
};

#endif // LVOX_COUNTWITHLENGTHTHRESHOLDVISITOR_H
