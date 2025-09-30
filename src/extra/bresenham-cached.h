#ifndef BRESENHAMCACHED_H
#define BRESENHAMCACHED_H

#include "util.h"

class BresenhamCached
{
  public:
    BresenhamCached(size_t rangeX, size_t rangeY);

    const PositionV& GetLine(const Position& from, const Position& to);

    std::string GetStats();

  private:
    std::unordered_map<Position, std::vector<Position>> _cache;

    Position _zero;
    Position _corrected;
};

#endif // BRESENHAMCACHED_H
