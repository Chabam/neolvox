#include "maxni.h"

void MaxNi::join(const VoxelData& rhs)
{
    if (rhs.ni > _data.ni)
    {
        _data = rhs;
    }
}