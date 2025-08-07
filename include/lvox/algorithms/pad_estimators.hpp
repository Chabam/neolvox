#ifndef LVOX_PAD_ESTIMATORS
#define LVOX_PAD_ESTIMATORS

#include <lvox/types.hpp>

namespace lvox::algorithms
{

namespace pad_estimators
{

struct BeerLambert {};
struct ContactFrequency {};
struct UnequalPathLengthBeerLambert {};

using PADEstimator = std::variant<BeerLambert, ContactFrequency, UnequalPathLengthBeerLambert>;

} // namespace pad_estimator

} // namespace lvox::algorithms

#endif // LVOX_PAD_ESTIMATORS
