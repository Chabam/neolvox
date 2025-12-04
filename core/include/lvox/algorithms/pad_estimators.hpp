#ifndef LVOX_PAD_ESTIMATORS_HPP
#define LVOX_PAD_ESTIMATORS_HPP

#include <variant>

#include <lvox/types.hpp>

namespace lvox::algorithms
{

struct PadComputeData;

namespace pad_estimators
{

// clang-format off

struct BeerLambert {};
struct ContactFrequency {};
struct UnequalPathLengthBeerLambert {};
struct BiasCorrectedMaximumLikelyhoodEstimator {};

template <typename T>
struct is_bl : std::is_same<T, BeerLambert> {};

template <typename T>
struct is_cf : std::is_same<T, ContactFrequency> {};

template <typename T>
struct is_uplbl : std::is_same<T, UnequalPathLengthBeerLambert> {};

template <typename T>
struct is_bcmle : std::is_same<T, BiasCorrectedMaximumLikelyhoodEstimator> {};

template <typename T>
struct estimator_uses_effective_lengths : std::disjunction<is_uplbl<T>, is_bcmle<T>> {};

// clang-format on
using PADEstimator = std::variant<
    BeerLambert,
    ContactFrequency,
    UnequalPathLengthBeerLambert,
    BiasCorrectedMaximumLikelyhoodEstimator>;

} // namespace pad_estimators

} // namespace lvox::algorithms

#endif // LVOX_PAD_ESTIMATORS_HPP
