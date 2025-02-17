#include "base.h"

void VoxelData::load(Grid3DSet& g, size_t idx)
{
    nt = g.Nt->getValueAtIndex(idx);
    nb = g.Nb->getValueAtIndex(idx);
    ni = g.Ni->getValueAtIndex(idx);
    rd = g.Density->getValueAtIndex(idx);
}

void VoxelData::commit(Grid3DSet& g, size_t idx)
{
    g.Nt->setValueAtIndex(idx, nt);
    g.Nb->setValueAtIndex(idx, nb);
    g.Ni->setValueAtIndex(idx, ni);
    g.Density->setValueAtIndex(idx, rd);
}

ReducerBase::ReducerBase(VoxelReducerOptions& opts) : _opts(opts)
{
}

void ReducerBase::init(VoxelData& data)
{
    _data = data;
}

VoxelData& ReducerBase::value()
{
    return _data;
}
