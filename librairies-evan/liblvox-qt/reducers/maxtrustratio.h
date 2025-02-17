#ifndef MAXTRUSTRATIO_H
#define MAXTRUSTRATIO_H

#pragma once
#include "base.h"

class MaxTrustRatio : public ReducerBase
{
public:
    void join(const VoxelData& rhs);
};

#endif