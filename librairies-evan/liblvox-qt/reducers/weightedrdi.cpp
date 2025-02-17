#include "weightedrdi.h"

#include "base.h"

WeightedRDI::WeightedRDI() : _sumRDI(0), _sumWeight(0.0)
{
}

WeightedRDI::WeightedRDI(VoxelReducerOptions& opts) : ReducerBase(opts), _sumRDI(0), _sumWeight(0)
{
}

void WeightedRDI::init(VoxelData& data)
{
    ReducerBase::init(data);
    _sumRDI = 0.0;
    _sumWeight = 0;

    if (isNotOcclusion(_data))
    {
        _sumRDI += (_data.nt - _data.nb) * _data.rd;
        _sumWeight += (_data.nt - _data.nb);
    }
}

// Voxelreducer that weighted sum of voxels with higher than raythreshhold,
// normally 10
void WeightedRDI::join(const VoxelData& rhs)
{
    _data.nt += rhs.nt;
    _data.ni += rhs.ni;
    _data.nb += rhs.nb;

    if (isNotOcclusion(rhs) && isNotOcclusion(_data))
    {
        _sumRDI += ((rhs.nt - rhs.nb) * rhs.rd);
        _sumWeight += (rhs.nt - rhs.nb);
    }
    else if (isNotOcclusion(rhs))
    { // if
        _data.rd = rhs.rd;
        _sumRDI += ((rhs.nt - rhs.nb) * rhs.rd);
        _sumWeight += (rhs.nt - rhs.nb);
    }
}

VoxelData& WeightedRDI::value()
{
    // Only modify of voxel value if we did sommations
    if (isNotOcclusion(_data) && _sumWeight > 0)
    {
        _data.rd = _sumRDI / _sumWeight;
    }
    return _data;
}

bool WeightedRDI::isNotOcclusion(const VoxelData& v)
{
    // Only modify of voxel value if we did sommations

    // if the computation of PAD is correct, there is no need to check rd >= 0!!!
    if (_opts.isRelativeThreshold)
    {
        return ((v.nt - v.nb) * 100.0 / v.nt > _opts.effectiveRaysThreshold) && (v.rd >= 0);
    }
    else
    {
        return (v.nt - v.nb > _opts.effectiveRaysThreshold) && (v.rd >= 0);
    }
}

void WeightedRDIAlt::init(VoxelData& data)
{
    ReducerBase::init(data);
    _sumRDI = 0.0;
    _sumWeight = 0;
    if (isNotOcclusion(_data))
    {
        double weight = (_data.nt - _data.nb) / _data.nt;
        _sumRDI += weight * _data.rd;
        _sumWeight += weight;
    }
}

// Voxelreducer that weighted sum of voxels with higher than raythreshhold,
// normally 10
void WeightedRDIAlt::join(const VoxelData& rhs)
{
    _data.nt += rhs.nt;
    _data.ni += rhs.ni;
    _data.nb += rhs.nb;

    if (isNotOcclusion(rhs) && isNotOcclusion(_data))
    {
        double weight = (rhs.nt - rhs.nb) / rhs.nt;
        _sumRDI += weight * rhs.rd; // Intermediary rdi value, the actual value is
                                    // calculated in the value() method
        _sumWeight += weight;       // Private variable to VoxelReducerWeightedRDI for
                                    // storage purposes to the value() method
    }
    else if (isNotOcclusion(rhs))
    { // if
        _data.rd = rhs.rd;
    }

    // do nothing if the value of rhs is not good
    if (rhs.nt - rhs.nb > _opts.effectiveRaysThreshold / 100.0 && rhs.rd > 0)
    {
        // do a summation if m_data does not contain error value
        /**only do the sum if both voxels has a significal rays and the density
         *value (or PAD) is greater than 0 some voxels may have a PAD = 0 or -1
         *which can diminue the value of merged voxel
         **/
        if (_data.nt - _data.nb > _opts.effectiveRaysThreshold && _data.rd > 0)
        {
            // Merging Ni,Nt,Nb
            // Calculation of RDI
            double weight = (rhs.nt - rhs.nb) / rhs.nt;
            _sumRDI += weight * rhs.rd; // Intermediary rdi value, the actual value is
                                        // calculated in the value() method
            _sumWeight += weight;       // Private variable to VoxelReducerWeightedRDI for
                                        // storage purposes to the value() method
        }
        else
        {
            // replace value of curent voxel by that of rhs
            _data.rd = rhs.rd;
            double weight = (rhs.nt - rhs.nb) / rhs.nt;
            _sumRDI += weight * _data.rd;
            _sumWeight += weight;
        }
    }
}
