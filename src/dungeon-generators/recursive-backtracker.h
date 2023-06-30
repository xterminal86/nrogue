#ifndef RECURSIVEBACKTRACKER_H
#define RECURSIVEBACKTRACKER_H

#include "dg-base.h"

class RecursiveBacktracker : public DGBase
{
  public:
    void Generate(const Position& mapSize,
                  const Position& startingPoint,
                  const RemovalParams& removalParams);

  private:
    Position* GetRandomCell(const Position& p);

    Position _randomCell;
};

#endif // RECURSIVEBACKTRACKER_H
