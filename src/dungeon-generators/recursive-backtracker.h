#ifndef RECURSIVEBACKTRACKER_H
#define RECURSIVEBACKTRACKER_H

#include "dg-base.h"

class RecursiveBacktracker : public DGBase
{
  public:
    void Generate(Position mapSize, Position startingPoint);

  private:
    void CutProblemCorners();
    void CheckIfProblemCorner(Position p);

    std::vector<Position> GetRandomCell(Position p);
};

#endif // RECURSIVEBACKTRACKER_H
