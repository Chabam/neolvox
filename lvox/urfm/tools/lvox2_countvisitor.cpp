#include "lvox2_countvisitor.h"

LVOX2_CountVisitor::LVOX2_CountVisitor(CT_Grid3D<int> *grid)
{
  _grid = grid;
}

void LVOX2_CountVisitor::visit(const size_t &index, const CT_Beam *beam)
{
    _grid->addValueAtIndex(index, 1);
}
