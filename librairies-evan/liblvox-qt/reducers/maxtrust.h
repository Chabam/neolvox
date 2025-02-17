#ifndef VOXELREDUCERMAXTRUST_H
#define VOXELREDUCERMAXTRUST_H

#pragma once

#include "base.h"

class MaxTrust : public ReducerBase
{
public:
    void join(const VoxelData& rhs);
};

#endif