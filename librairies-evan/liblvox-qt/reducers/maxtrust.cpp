#include "maxtrust.h"

void MaxTrust::join(const VoxelData& rhs)
{
    if ((rhs.nt - rhs.nb) > (_data.nt - _data.nb))
    {
        _data = rhs;
    }
}