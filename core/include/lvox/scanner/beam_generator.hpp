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

    virtual const std::vector<Beam>& get_beams() const = 0;
};

} // namespace lvox

#endif // LVOX_BEAM_GENERATOR_HPP
