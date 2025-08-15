#ifndef LVOX_PAD_ESTIMATORS
#define LVOX_PAD_ESTIMATORS

#include <variant>

#include <lvox/types.hpp>

namespace lvox::algorithms
{

struct ComputeData;

namespace pad_estimators
{

struct BeerLambert {};
struct ContactFrequency {};
struct UnequalPathLengthBeerLambert {};

template <typename T>
struct is_bl : std::is_same<T, BeerLambert> {};

template <typename T>
struct is_cf : std::is_same<T, ContactFrequency> {};

template <typename T>
struct is_uplbl : std::is_same<T, UnequalPathLengthBeerLambert> {};

template <typename T>
struct estimator_uses_effective_lengths : std::disjunction<is_bl<T>, is_cf<T>> {};

using PADEstimator = std::variant<BeerLambert, ContactFrequency, UnequalPathLengthBeerLambert>;

auto beer_lambert(const ComputeData& data, Index index) -> double;
auto contact_frequency(const ComputeData& data, Index index) -> double;
auto unequal_path_length_beer_lambert(const ComputeData& data, Index index) -> double;

} // namespace pad_estimators

namespace pe = pad_estimators;

} // namespace lvox::algorithms

#endif // LVOX_PAD_ESTIMATORS
