#ifndef RECURSIVEBACKTRACKER_H
#define RECURSIVEBACKTRACKER_H

#include "dg-base.h"

class RecursiveBacktracker : public DGBase
{
  public:
    void Generate(const Position& mapSize,
                  const Position& startingPoint,
                  const RemovalParams& removalParams = RemovalParams());

  private:
    std::vector<Position> GetRandomCell(const Position& p);
};

#endif // RECURSIVEBACKTRACKER_H
