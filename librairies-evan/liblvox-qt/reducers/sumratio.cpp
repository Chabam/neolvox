#include "sumratio.h"

void SumRatio::join(const VoxelData& rhs)
{
    if ((_data.nt - _data.nb) > _opts.effectiveRaysThreshold)
    {
        _data.nt += rhs.nt;
        _data.ni += rhs.ni;
        _data.nb += rhs.nb;
    }
    else
    {
        _data.rd = rhs.rd;
    }
}

VoxelData& SumRatio::value()
{
    if ((_data.nt - _data.nb) > _opts.effectiveRaysThreshold)
    {
        _data.rd = (float)_data.ni / (_data.nt - _data.nb);
    }
    return _data;
}