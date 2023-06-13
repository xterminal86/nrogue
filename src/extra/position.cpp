#include "position.h"

#include <cstdio>

#include "util.h"

Position::Position()
{
  X = 0;
  Y = 0;
}

// =============================================================================

Position::Position(int x, int y)
{
  X = x;
  Y = y;
}

// =============================================================================

void Position::Set(int x, int y)
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

void Position::Print()
{
  DebugLog("%s: [%i ; %i]\n", __PRETTY_FUNCTION__, X, Y);
}
