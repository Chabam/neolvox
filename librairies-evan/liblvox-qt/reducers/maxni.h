#ifndef MAXNI_H
#define MAXNI_H

#pragma once

#include "base.h"

class MaxNi : public ReducerBase
{
public:
    void join(const VoxelData& rhs);
};

#endif