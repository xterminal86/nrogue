#ifndef POSITION_H
#define POSITION_H

///
/// Because no project is considered serious unless you reinvent the wheel.
///
struct Position
{
  Position();
  Position(int x, int y);

  void Set(int x, int y);
  void Set(const Position& from);

  //
  // For use inside std::map as a key.
  //
  bool operator< (const Position& rhs) const;
  bool operator== (const Position& rhs) const;
  bool operator!= (const Position& rhs) const;

  void Print();

  int X;
  int Y;
};

#endif // POSITION_H
