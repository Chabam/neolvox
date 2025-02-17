#ifndef VOXELREDUCERMAXRDI_H
#define VOXELREDUCERMAXRDI_H

#pragma once

#include "base.h"

class MaxRDI : public ReducerBase
{
public:
    void join(const VoxelData& rhs);
};

#endif