#ifndef SHADOWCASTER_H
#define SHADOWCASTER_H

//
// Ported from:
// https://journal.stuffwithstuff.com/2015/09/07/what-the-hero-sees/
//

#include <vector>
#include <stack>
#include <cstdint>
#include <cmath>

#include "position.h"
#include "util.h"

using PairI = std::pair<int, int>;

class Shadowcaster
{
  public:
    Shadowcaster();
    Shadowcaster(const Position& playerPos);

    void Init(const int posX, const int posY);
    void Init(const Position& playerPos);

    void RefreshVisibility();

  private:
    struct Shadow
    {
      double Start = 0.0;
      double End   = 0.0;

      bool Contains(const Shadow& other);
    };

    class ShadowLine
    {
      public:
        bool IsInShadow(const Shadow& projection);
        void Add(const Shadow& shadow);
        bool IsInFullShadow();

      private:
        std::vector<Shadow> _shadows;
    };

    void RefreshOctant(uint8_t octant);

    const PairI& TransformOctant(int row, int col, uint8_t octant);
    const Shadow& ProjectTile(const int row, const int col);

    Position _playerPos;

    Shadow _shadow;

    PairI _transformedCoords;
};

#endif // SHADOWCASTER_H
