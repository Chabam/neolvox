#ifndef LVOX_COMPUTE_PAD_VISITOR
#define LVOX_COMPUTE_PAD_VISITOR

#include <lvox/types.hpp>

namespace lvox::algorithms
{

namespace pad_estimators
{
struct BeerLambert;
struct ContactFrequency;
struct UnequalPathLengthBeerLambert;
}

struct ComputeData;

namespace pe = lvox::algorithms::pad_estimators;

struct ComputePADVisitor
{
    ComputePADVisitor(const ComputeData& data, Index index);

    auto operator()(pe::BeerLambert) const -> double;
    auto operator()(pe::ContactFrequency) const -> double;
    auto operator()(pe::UnequalPathLengthBeerLambert) const -> double;

    const ComputeData& m_data;
    Index m_index;
};

}

#endif // LVOX_COMPUTE_PAD_VISITOR
