#ifndef POSITION_H
#define POSITION_H

struct Position
{
  Position()
  {
    X = 0;
    Y = 0;
  }

  Position(int x, int y)
  {
    X = x;
    Y = y;
  }

  Position(const Position& from)
  {
    X = from.X;
    Y = from.Y;
  }

  virtual ~Position()
  {
  }

  void Set(int x, int y)
  {
    X = x;
    Y = y;
  }

  void Set(const Position& from)
  {
    X = from.X;
    Y = from.Y;
  }

  // For use inside std::map as a key
  bool operator< (const Position& rhs) const
  {
    return (X < rhs.X || Y < rhs.Y);
  }

  bool operator== (const Position& rhs) const
  {
    return (X == rhs.X && Y == rhs.Y);
  }

  int X;
  int Y;
};

#endif // POSITION_H
