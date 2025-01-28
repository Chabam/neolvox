#include "lvox_countwithlengththresholdvisitor.h"

LVOX_CountWithLengthThresholdVisitor::LVOX_CountWithLengthThresholdVisitor(CT_Grid3D<int> *grid, double threshold)
{
  _grid = grid;
  _threshold = threshold;
}

void LVOX_CountWithLengthThresholdVisitor::visit(const size_t &index, const CT_Beam *beam)
{

    Eigen::Vector3d bot, top, nearInter, farInter;
    Eigen::Vector3d origin = beam->getOrigin();
    bool ok = _grid->getCellCoordinates(index, bot, top);

    if (ok && beam->intersect(bot, top, nearInter, farInter))
    {
        double distNear = sqrt(pow(nearInter(0) - origin(0), 2) + pow(nearInter(1) - origin(1), 2) + pow(nearInter(2) - origin(2), 2));
        double distFar = sqrt(pow(farInter(0) - origin(0), 2) + pow(farInter(1) - origin(1), 2) + pow(farInter(2) - origin(2), 2));

        if (((distNear + distFar) / 2.0) < _threshold)
        {
            _grid->addValueAtIndex(index, 1);
        }
    }
}
