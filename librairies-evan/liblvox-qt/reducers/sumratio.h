#ifndef SUMRATIO_H
#define SUMRATIO_H

#pragma once
#include "base.h"

class SumRatio : public ReducerBase
{
public:
    void join(const VoxelData& rhs);
    VoxelData& value();
};

#endif