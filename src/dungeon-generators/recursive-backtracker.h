#ifndef RECURSIVEBACKTRACKER_H
#define RECURSIVEBACKTRACKER_H

#include "dg-base.h"

class RecursiveBacktracker : public DGBase
{
  public:
    void Generate(const Position& mapSize, const Position& startingPoint);

  private:
    void CutProblemCorners();
    void CheckIfProblemCorner(const Position& p);

    std::vector<Position> GetRandomCell(const Position& p);
};

#endif // RECURSIVEBACKTRACKER_H
