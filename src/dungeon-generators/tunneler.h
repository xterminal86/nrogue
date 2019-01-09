#ifndef TUNNELER_H
#define TUNNELER_H

#include <stack>

#include "dg-base.h"

class Tunneler : public DGBase
{
  public:
    void Backtracking(Position mapSize, Position tunnelLengthMinMax, Position start = { -1, -1 }, bool additionalTweaks = false);
    void Normal(Position mapSize, Position tunnelLengthMinMax, Position start, int maxIterations, bool additionalTweaks = false);

  private:
    std::vector<Position> GetRandomDir(Position pos);
    std::vector<Position> TryToGetPerpendicularDir(Position pos, Position lastDir);

    Position GetRandomPerpendicularDir(Position dir);

    std::vector<Position> GetCorridorDir(Position pos);

    bool IsDirectionValid(Position pos, Position dir);
};

#endif // TUNNELER_H
