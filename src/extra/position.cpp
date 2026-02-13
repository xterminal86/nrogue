#include "position.h"

#include <cstdio>

#include "util.h"

Position::Position()
  : X(0), Y(0) {}

// =============================================================================

Position::Position(int32_t x, int32_t y)
  : X(x), Y(y) {}

// =============================================================================

void Position::Set(int32_t x, int32_t y)
{
  X = x;
  Y = y;
}

// =============================================================================

void Position::Set(const Position &from)
{
  X = from.X;
  Y = from.Y;
}

// =============================================================================

uint64_t Position::GetHashCode() const
{
  std::bitset<64> one{(uint32_t)X};
  std::bitset<64> two{(uint32_t)Y};

  one <<= 32;

  std::bitset<64> hash = one | two;

  return hash.to_ullong();
}

// =============================================================================

bool Position::operator< (const Position& rhs) const
{
  //
  // Expression:
  //
  // (X < rhs.X || Y < rhs.Y)
  //
  // actually does not conform to strict weak ordering,
  // which should be enforced in order to be used in certain containers like
  // std::map and std::set in particular, meaning if a < b is true then b < a
  // must be false. But consider this:
  //
  // a (0 ; 39), b (1, 0)
  //
  // a < b ? (a.0 < b.1 || a.39 < b.0) - true
  //          ^^^^^^^^^
  // b < a ? (b.1 < a.0 || b.0 < a.39) - also true
  //                       ^^^^^^^^^^
  //
  // Both a < b and b < a can be false only if a == b, but they can't be both
  // true, like in the above case. Fix was taken from here:
  //
  // https://stackoverflow.com/questions/67049689/how-do-i-fix-an-invalid-comparator-error
  //
  // Interestingly, it seems that it does affect MSVC Debug build: roads in town
  // look a little different in certain spot than in Release build.
  //
  return ( X < rhs.X || (X == rhs.X && Y < rhs.Y) );
}

// =============================================================================

bool Position::operator== (const Position& rhs) const
{
  return (X == rhs.X && Y == rhs.Y);
}

// =============================================================================

bool Position::operator!= (const Position& rhs) const
{
  return (X != rhs.X || Y != rhs.Y);
}

// =============================================================================

std::string Position::ToString() const
{
  static std::string intl;
  intl = Util::StringFormat("<%d %d>", X, Y);
  return intl;
}

// =============================================================================

const Position& Position::Zero()
{
  static Position zero(0, 0);
  return zero;
}
