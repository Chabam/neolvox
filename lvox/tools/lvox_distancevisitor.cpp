#include "lvox_distancevisitor.h"


LVOX_DistanceVisitor::LVOX_DistanceVisitor(CT_Grid3D<float> *grid)
{
  _grid = grid;
}

void LVOX_DistanceVisitor::visit(const size_t &index, const CT_Beam *beam)
{
    Eigen::Vector3d bot, top, nearInter, farInter;
    bool ok = _grid->getCellCoordinates(index, bot, top);

    if (ok && beam->intersect(bot, top, nearInter, farInter))
    {
        _grid->addValueAtIndex(index, sqrt(pow(nearInter(0) - farInter(0), 2) + pow(nearInter(1) - farInter(1), 2) + pow(nearInter(2) - farInter(2), 2)));
    }
}
