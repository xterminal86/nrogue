#include <sstream>

#include "shadowcaster.h"

#include "util.h"

Shadowcaster::Shadowcaster()
{
}

// =============================================================================

Shadowcaster::Shadowcaster(const Position& origin)
{
  _origin = origin;
}

// =============================================================================

const std::string& Shadowcaster::ToString()
{
  std::stringstream ss;

  for (auto& row : _quadrants)
  {
    for (auto& i : row)
    {
      ss << Util::StringFormat("%s", i.ToString().data());
    }

    ss << "\n";
  }

  _stringRepr = ss.str();

  return _stringRepr;
}
