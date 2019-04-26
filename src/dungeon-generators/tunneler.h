#ifndef TUNNELER_H
#define TUNNELER_H

#include <stack>

#include "dg-base.h"

class Tunneler : public DGBase
{
  public:
    void Backtracking(const Position& mapSize,
                      const Position& tunnelLengthMinMax,
                      const Position& start = { -1, -1 },
                      bool additionalTweaks = false);

    void Normal(const Position& mapSize,
                const Position& tunnelLengthMinMax,
                const Position& start,
                int maxIterations,
                bool additionalTweaks = false);

  private:
    std::vector<Position> GetRandomDir(const Position& pos);
    std::vector<Position> TryToGetPerpendicularDir(const Position& pos, const Position& lastDir);

    Position GetRandomPerpendicularDir(const Position& dir);

    std::vector<Position> GetCorridorDir(const Position& pos);

    bool IsDirectionValid(const Position& pos, const Position& dir);
};

#endif // TUNNELER_H
