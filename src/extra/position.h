#ifndef POSITION_H
#define POSITION_H

#include <functional>
#include <cstdint>
#include <bitset>

///
/// Because no project is considered serious unless you reinvent the wheel.
///
struct Position
{
  Position();
  Position(int32_t x, int32_t y);

  void Set(int32_t x, int32_t y);
  void Set(const Position& from);

  uint64_t GetHashCode() const;

  //
  // For use inside std::map as a key.
  //
  bool operator< (const Position& rhs) const;
  bool operator== (const Position& rhs) const;
  bool operator!= (const Position& rhs) const;

  const std::string& ToString() const;

  static const Position& Zero();

  int32_t X = 0;
  int32_t Y = 0;
};

namespace std
{
  template<>
  struct hash<Position>
  {
    size_t operator()(const Position& p) const
    {
      return p.GetHashCode();
    }
  };
}

#endif // POSITION_H
