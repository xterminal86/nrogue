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
  return (X < rhs.X || Y < rhs.Y);
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

const std::string& Position::ToString() const
{
  static std::string intl;
  intl = Util::StringFormat("<%d %d>", X, Y);
  return intl;
}
