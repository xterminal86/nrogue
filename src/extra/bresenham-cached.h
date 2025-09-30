#ifndef BRESENHAMCACHED_H
#define BRESENHAMCACHED_H

#include "util.h"

class BresenhamCached
{
  public:
    BresenhamCached(size_t rangeX, size_t rangeY);

    const PositionV& GetOffsets(int32_t sx, int32_t sy, int32_t ex, int32_t ey);
    const PositionV& GetOffsets(const Position& from, const Position& to);

    const PositionV& GetPoints(int32_t sx, int32_t sy, int32_t ex, int32_t ey);
    const PositionV& GetPoints(const Position& from, const Position& to);

    std::string GetStats();

  private:
    std::unordered_map<Position, std::vector<Position>> _cache;

    PositionV _trueLine;

    Position _zero;
    Position _corrected;

    Position _start;
    Position _end;

    size_t _cacheHits   = 0;
    size_t _cacheMisses = 0;
};

#endif // BRESENHAMCACHED_H
