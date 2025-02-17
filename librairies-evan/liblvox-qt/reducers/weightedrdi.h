#ifndef WEIGHTEDRDI_H
#define WEIGHTEDRDI_H

#pragma once

#include "base.h"

class WeightedRDI : public ReducerBase
{
public:
    WeightedRDI();
    WeightedRDI(VoxelReducerOptions& opts);
    void init(VoxelData& data) override;
    void join(const VoxelData& rhs) override;
    VoxelData& value() override;

protected:
    bool isNotOcclusion(const VoxelData& v);
    // Intermediary rdi value, the actual value is calculated in the value()
    // method
    double _sumRDI;
    // Intermediary weights
    int _sumWeight;
};

class WeightedRDIAlt : public WeightedRDI
{
    using WeightedRDI::WeightedRDI;
    // using VoxelReducerWeightedRDI::init;
    using WeightedRDI::value;
    void init(VoxelData& data) override;
    void join(const VoxelData& rhs) override;
};

#endif
