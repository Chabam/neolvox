#ifndef LVOX_BEAM_GENERATOR_HPP
#define LVOX_BEAM_GENERATOR_HPP

#include <vector>

namespace lvox
{

class Beam;

class BeamGenerator
{
  public:
    BeamGenerator()          = default;
    virtual ~BeamGenerator() = default;

    virtual auto get_beams() const -> const std::vector<Beam>& = 0;
};

} // namespace lvox

#endif // LVOX_BEAM_GENERATOR_HPP
