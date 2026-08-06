#include "globals.h"
#include "enumerations.h"

#include "util.h"

void ConsoleTest()
{
  StringV map =
  {
      "###########"
    , "#.........#"
    , "#.........#"
    , "#.........#"
    , "#.........#"
    , "#.#.#.#.#.#"
    , "#.........#"
    , "#....x....#"
    , "#.........#"
    , "#.........#"
    , "###########"
  };

  const StringV expected =
  {
      "   #####   "
    , "#   ...   #"
    , "#.  ...  .#"
    , "#.. ... ..#"
    , "  .  .  .  "
    , "#.#.#.#.#.#"
    , "#.........#"
    , "#....x....#"
    , "#.........#"
    , "#.........#"
    , "###########"
  };

  const std::string decor(80, '=');

  using PairI = std::pair<int, int>;

  int playerPosX = 0;
  int playerPosY = 0;

  for (size_t x = 0; x < map[0].length(); x++)
  {
    for (size_t y = 0; y < map.size(); y++)
    {
      if (map[x][y] == 'x')
      {
        map[x][y] = '.';
        playerPosX = x;
        playerPosY = y;
        goto exitFor;
      }
    }
  }

exitFor:

  printf("Player pos: <%d,%d>\n\n", playerPosX, playerPosY);

  //
  // Ported from :
  // https://journal.stuffwithstuff.com/2015/09/07/what-the-hero-sees/
  //
  auto TransformOctant =
  [](const int row, const int col, const uint8_t octant) -> PairI
  {
    switch (octant)
    {
      case 0:  return {  col, -row };
      case 1:  return {  row, -col };
      case 2:  return {  row,  col };
      case 3:  return {  col,  row };
      case 4:  return { -col,  row };
      case 5:  return { -row,  col };
      case 6:  return { -row, -col };
      case 7:  return { -col, -row };
      default: return {    0,    0 };
    }
  };

  for (uint8_t octant = 0; octant < 8; octant++)
  {
    printf("Octant %u:\n\n", octant);

    for (int row = 1; row < 10; row++)
    {
      for (int col = 0; col <= row; col++)
      {
        PairI t = TransformOctant(row, col, octant);

        int x = playerPosX + t.first;
        int y = playerPosY + t.second;

        printf("<%d,%d>", x, y);
      }

      printf("\n");
    }

    printf("%s\n", decor.data());
  }

  struct Shadow
  {
    double Start = 0.0;
    double End   = 0.0;

    bool Contains(const Shadow& other)
    {
      return (Start <= other.Start && End >= other.End);
    }
  };

  struct ShadowLine
  {
    private:
      std::vector<Shadow> _shadows;

    public:

      // =======================================================================

      bool IsInShadow(const Shadow& projection)
      {
        for (Shadow& s : _shadows)
        {
          if (s.Contains(projection))
          {
            return true;
          }
        }

        return false;
      };

      // =======================================================================

      void Add(const Shadow& shadow)
      {
        // Figure out where to slot the new shadow in the list.
        size_t index = 0;

        for (index = 0; index < _shadows.size(); index++)
        {
          // Stop when we hit the insertion point.
          if (_shadows[index].Start >= shadow.Start)
          {
            break;
          }
        }

        // The new shadow is going here. See if it overlaps the
        // previous or next.
        Shadow* overlappingPrevious = nullptr;
        if (index > 0 && _shadows[index - 1].End > shadow.Start)
        {
          overlappingPrevious = &_shadows[index - 1];
        }

        Shadow* overlappingNext = nullptr;
        if (index < _shadows.size() && _shadows[index].Start < shadow.End)
        {
          overlappingNext = &_shadows[index];
        }

        // Insert and unify with overlapping shadows.
        if (overlappingNext != nullptr)
        {
          if (overlappingPrevious != nullptr)
          {
            // Overlaps both, so unify one and delete the other.
            overlappingPrevious->End = overlappingNext->End;
            _shadows.erase(_shadows.begin() + index);
          }
          else
          {
            // Overlaps the next one, so unify it with that.
            overlappingNext->Start = shadow.Start;
          }
        }
        else
        {
          if (overlappingPrevious != nullptr)
          {
            // Overlaps the previous one, so unify it with that.
            overlappingPrevious->End = shadow.End;
          }
          else
          {
            // Does not overlap anything, so insert.
            _shadows.insert(_shadows.begin() + index, shadow);
          }
        }
      }

      // =======================================================================

      bool IsInFullShadow()
      {
        return (!_shadows.empty() &&
                Util::CloseEnoughTo(_shadows[0].Start, 0.0) &&
                Util::CloseEnoughTo(_shadows[0].End,   1.0));
      }
  };

  //
  // Check comments in shadowcaster.cpp before this method.
  //
  auto ProjectTile = [](const int row, const int col)
  {
    double topLeft     = (double)col / (double)(row + 2);
    double bottomRight = (double)(col + 1) / (double)(row + 1);

    printf("  tl = %d / %d, br = %d / %d\n", col, row + 2, col + 1, row + 1);
    printf("  row = %d col = %d -> topLeft = %.4f, bottomRight = %.4f\n",
           row, col, topLeft, bottomRight);

    return Shadow { topLeft, bottomRight };
  };

  auto RefreshOctant = [&TransformOctant, &ProjectTile, playerPosX, playerPosY, &map]
  (const Position& hero, const uint8_t octant)
  {
    ShadowLine line;
    bool fullShadow = false;

    for (int row = 1; ; row++)
    {
      PairI transformed = TransformOctant(row, 0, octant);

      int x = playerPosX + transformed.first;
      int y = playerPosY + transformed.second;

      // Stop once we go out of bounds.
      if (x < 0 || x > (int)map[0].length() - 1
       || y < 0 || y > (int)map.size() - 1)
      {
        break;
      }

      for (int col = 0; col <= row; col++)
      {
        PairI transformed = TransformOctant(row, col, octant);

        int x = playerPosX + transformed.first;
        int y = playerPosY + transformed.second;

        // Stop once we go out of bounds.
        if (x < 0 || x > (int)map[0].length() - 1
         || y < 0 || y > (int)map.size() - 1)
        {
          break;
        }

        if (fullShadow)
        {
          map[x][y] = ' ';
        }
        else
        {
          Shadow projection = ProjectTile(row, col);

          printf("  (%d, %d) projection: %.4f -> %.4f\n",
                 x, y, projection.Start, projection.End);

          bool visible = !line.IsInShadow(projection);
          map[x][y] = visible ? map[x][y] : ' ';

          if (visible && map[x][y] == '#')
          {
            line.Add(projection);
            fullShadow = line.IsInFullShadow();
          }
        }
      }
    }
  };

  auto RefreshVisibility = [RefreshOctant](const Position playerPos)
  {
    for (uint8_t octant = 0; octant < 8; octant++)
    {
      printf("Octant: %u\n", octant);
      RefreshOctant(playerPos, octant);
    }
  };

  Position p = { playerPosX, playerPosY };

  RefreshVisibility(p);

  map[playerPosX][playerPosY] = 'x';

  StringV res;

  size_t index = 0;
  for (auto& line : map)
  {
    printf("%4u: %s\n", index, line.data());
    res.push_back(line);
    index++;
  }

  if (map.size() != expected.size())
  {
    printf("actual (%lu) and expected (%lu) map sizes don't match!\n",
           map.size(), expected.size());
  }
  else
  {
    bool ok = true;
    for (size_t i = 0; i < map.size(); i++)
    {
      if (map[i] != expected[i])
      {
        printf("! line mismatch at index %lu !\n", i);
        printf("actual   = '%s'\n", map[i].data());
        printf("expected = '%s'\n", expected[i].data());
        ok = false;
        break;
      }
    }

    if (ok)
    {
      printf("Looks fine.\n");
    }
  }
}

// =============================================================================

int main(int argc, char* argv[])
{
  ConsoleTest();

  return 0;
}
