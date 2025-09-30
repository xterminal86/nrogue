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
}

// =============================================================================

const PositionV& BresenhamCached::GetLine(const Position& from,
                                          const Position& to)
{
  _corrected.Set(to.X - from.X, to.Y - from.Y);

  if (_cache.count(_corrected) == 0)
  {
    _cache[_corrected] = Util::BresenhamLine(_zero, _corrected);
  }

  return _cache[_corrected];
}

// =============================================================================

std::string BresenhamCached::GetStats()
{
  static const std::string ruler(80, '=');
  std::stringstream ss;

  ss << ruler.data() << "\n";

  ss << "Size             : " << _cache.size()              << "\n"
     << "Max size         : " << _cache.max_size()          << "\n"
     << "Bucket count     : " << _cache.bucket_count()      << "\n"
     << "Max bucket count : " << _cache.max_bucket_count()  << "\n"
     << "Load factor      : " << _cache.load_factor()       << "\n"
     << "Max load factor  : " << _cache.max_load_factor()   << "\n";

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
