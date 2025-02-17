#include "maxtrustratio.h"

void MaxTrustRatio::join(const VoxelData& rhs)
{
    if (_opts.ignoreVoxelZeroDensity && rhs.rd <= 0)
    {
        return;
    }
    float ratioSelf = 0;
    float ratioOther = 0;
    if (rhs.nt > 0)
    {
        ratioOther = (static_cast<float>(rhs.nt - rhs.nb)) / rhs.nt;
    }
    if (_data.nt > 0)
    {
        ratioSelf = (static_cast<float>(_data.nt - _data.nb)) / _data.nt;
    }
    if (ratioOther > ratioSelf)
    {
        _data = rhs;
    }
}