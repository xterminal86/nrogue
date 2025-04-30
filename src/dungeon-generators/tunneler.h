#ifndef TUNNELER_H
#define TUNNELER_H

#include <stack>

#include "dg-base.h"

class Tunneler : public DGBase
{
  public:
    void Backtracking(const Position& mapSize,
                      const Position& tunnelLengthMinMax,
                      const Position& start,
                      bool additionalTweaks);

    void Normal(const Position& mapSize,
                const Position& tunnelLengthMinMax,
                const Position& start,
                int maxIterations,
                bool additionalTweaks);

  private:
    Position* GetRandomDir(const Position& pos);
    Position* TryToGetPerpendicularDir(const Position& pos,
                                       const Position& lastDir);

    Position GetRandomPerpendicularDir(const Position& dir);

    Position* GetCorridorDir(const Position& pos);

    bool IsDirectionValid(const Position& pos, const Position& dir);

    Position _randomDir;
    Position _corridorDir;
    Position _perpendicularDir;
};

#endif // TUNNELER_H
