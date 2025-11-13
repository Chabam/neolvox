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
struct BiasCorrectedMaximumLikelyhoodEstimator;
} // namespace pad_estimators

class ComputePAD
{
  public:
    ComputePAD(COOGrid& grid);
    void operator()(algorithms::pad_estimators::BeerLambert);
    void operator()(algorithms::pad_estimators::ContactFrequency);
    void operator()(algorithms::pad_estimators::UnequalPathLengthBeerLambert);
    void operator()(algorithms::pad_estimators::BiasCorrectedMaximumLikelyhoodEstimator);

  private:
    COOGrid& m_grid;
};

} // namespace algorithms

} // namespace lvox

#endif // LVOX_COMPUTE_PAD_HPP
