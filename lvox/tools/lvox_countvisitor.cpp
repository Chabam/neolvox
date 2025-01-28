#include "lvox_countvisitor.h"

LVOX_CountVisitor::LVOX_CountVisitor(CT_Grid3D<int> *grid)
{
  _grid = grid;
}

void LVOX_CountVisitor::visit(const size_t &index, const CT_Beam *beam)
{
    _grid->addValueAtIndex(index, 1);
}
