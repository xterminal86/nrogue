#include "bresenham-cached.h"

BresenhamCached::BresenhamCached(size_t rangeX, size_t rangeY)
{
  int32_t ax = rangeX;
  int32_t ay = rangeY;

  Position dst;

  for (int32_t x = -ax; x <= ax; x++)
  {
    for (int32_t y = -ay; y <= ay; y++)
    {
      dst.Set(x, y);
      _cache[dst] = Util::BresenhamLine(_zero, dst);
    }
  }

  size_t capacity =
      std::sqrt(std::pow(rangeX * 2, 2) + std::pow(rangeY * 2, 2)) * 2;

  _trueLine.reserve(capacity);
}

// =============================================================================

const PositionV& BresenhamCached::GetOffsets(const Position& from,
                                              const Position& to)
{
  _corrected.Set(to.X - from.X, to.Y - from.Y);

  if (_cache.count(_corrected) == 0)
  {
    _cacheMisses++;
    _cache[_corrected] = Util::BresenhamLine(_zero, _corrected);
  }
  else
  {
    _cacheHits++;
  }

  return _cache[_corrected];
}

// =============================================================================

const PositionV& BresenhamCached::GetOffsets(int32_t sx,
                                              int32_t sy,
                                              int32_t ex,
                                              int32_t ey)
{
  _start.Set(sx, sy);
  _end.Set(ex, ey);

  return GetOffsets(_start, _end);
}

// =============================================================================

const PositionV& BresenhamCached::GetPoints(const Position &from,
                                             const Position &to)
{
  _trueLine.clear();

  const PositionV& offsets = GetOffsets(from, to);

  Position p;
  for (const Position& offset : offsets)
  {
    p.Set(from.X + offset.X, from.Y + offset.Y);
    _trueLine.push_back(p);
  }

  return _trueLine;
}

// =============================================================================

const PositionV& BresenhamCached::GetPoints(int32_t sx,
                                             int32_t sy,
                                             int32_t ex,
                                             int32_t ey)
{
  _start.Set(sx, sy);
  _end.Set(ex, ey);

  return GetPoints(_start, _end);
}

// =============================================================================

std::string BresenhamCached::GetStats()
{
  static const std::string ruler(80, '=');
  std::stringstream ss;

  ss << ruler.data() << "\n";

  ss << "Size               : " << _cache.size()             << "\n"
     << "Max size           : " << _cache.max_size()         << "\n"
     << "Bucket count       : " << _cache.bucket_count()     << "\n"
     << "Max bucket count   : " << _cache.max_bucket_count() << "\n"
     << "Load factor        : " << _cache.load_factor()      << "\n"
     << "Max load factor    : " << _cache.max_load_factor()  << "\n"
     << "True line capacity : " << _trueLine.capacity()      << "\n"
     << "Cache hits         : " << _cacheHits                 << "\n"
     << "Cache misses       : " << _cacheMisses               << "\n";

  size_t footprint = 0;

  size_t buckets = _cache.bucket_count();
  for (size_t i = 0; i < buckets; i++)
  {
    size_t bs = _cache.bucket_size(i);
    footprint += (sizeof(void*) * bs);
  }

  for (auto& kvp : _cache)
  {
    // Keys (hash is converted to some kind of index that is most likely of
    // current CPU's word size)
    footprint += (sizeof(void*));
    // Values
    footprint += (kvp.second.size() * sizeof(Position));
  }

  ss << "Footprint (est.) : " << footprint << " bytes\n";

  ss << ruler.data() << "\n";

  return ss.str();
}
