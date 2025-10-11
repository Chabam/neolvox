#ifndef LVOX_COMPUTE_PAD_HPP
#define LVOX_COMPUTE_PAD_HPP

namespace lvox
{

class COOGrid;

namespace algorithms
{

namespace pad_estimators
{
struct BeerLambert;
struct ContactFrequency;
struct UnequalPathLengthBeerLambert;
} // namespace pad_estimators

class ComputePAD
{
  public:
    ComputePAD(COOGrid& grid);
    auto operator()(algorithms::pad_estimators::BeerLambert) -> void;
    auto operator()(algorithms::pad_estimators::ContactFrequency) -> void;
    auto operator()(algorithms::pad_estimators::UnequalPathLengthBeerLambert) -> void;

  private:
    COOGrid& m_grid;
};

} // namespace algorithms

} // namespace lvox

#endif // LVOX_COMPUTE_PAD_HPP
