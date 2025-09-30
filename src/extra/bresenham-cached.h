#ifndef BRESENHAMCACHED_H
#define BRESENHAMCACHED_H

#include "util.h"

class BresenhamCached
{
  public:
    BresenhamCached(size_t rangeX, size_t rangeY);

    const PositionV& GetLineOffsets(int32_t sx, int32_t sy, int32_t ex, int32_t ey);
    const PositionV& GetLineOffsets(const Position& from, const Position& to);

    const PositionV& GetLine(int32_t sx, int32_t sy, int32_t ex, int32_t ey);
    const PositionV& GetLine(const Position& from, const Position& to);

    std::string GetStats();

  private:
    std::unordered_map<Position, std::vector<Position>> _cache;

    PositionV _trueLine;

    Position _zero;
    Position _corrected;

    Position _start;
    Position _end;
};

#endif // BRESENHAMCACHED_H
