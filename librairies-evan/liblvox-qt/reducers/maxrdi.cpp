#include "maxrdi.h"

void MaxRDI::join(const VoxelData& rhs)
{
    if (rhs.rd > _data.rd)
    {
        _data = rhs;
    }
}