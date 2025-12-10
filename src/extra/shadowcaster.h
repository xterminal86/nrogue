#ifndef SHADOWCASTER_H
#define SHADOWCASTER_H

//
// Ported implementation from here:
//
// https://www.albertford.com/shadowcasting/
//

#include <vector>

#include "position.h"

using PositionV = std::vector<Position>;

class Shadowcaster
{
  public:
    Shadowcaster();
    Shadowcaster(const Position& origin);

    const std::string& ToString();

  private:
    std::string _stringRepr;

    std::vector<PositionV> _quadrants;

    Position _origin;
};

#endif // SHADOWCASTER_H
