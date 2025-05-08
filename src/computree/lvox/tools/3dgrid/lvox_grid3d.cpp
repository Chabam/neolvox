#include <map>
#include <stdexcept>

namespace LVOX_Grid3DPrivate
{
  float safeGetValueFromMap(const std::map<Index, float>& map, Index index)
  {
    try
    {
      return  map.at(index);
    } catch (std::out_of_range _)
    {
      return 0.f;
    }
  }
}
