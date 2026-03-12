#include <sstream>

#include "shadowcaster.h"

#include "application.h"
#include "map.h"
#include "util.h"

Shadowcaster::Shadowcaster() {}

// =============================================================================

Shadowcaster::Shadowcaster(const Position& playerPos)
  : _playerPos(playerPos) {}

// =============================================================================

void Shadowcaster::Init(const Position& playerPos)
{
  _playerPos.X = playerPos.X;
  _playerPos.Y = playerPos.Y;
}

// =============================================================================

void Shadowcaster::Init(const int posX, const int posY)
{
  _playerPos.X = posX;
  _playerPos.Y = posY;
}

// =============================================================================

//
// Contrary to what's described in the article, octants here actually go like
// this:
//
// +---------+---------+
// |\        |        /|
// | \       |       / |
// |  \   7  |  6   /  |
// |   \     |     /   |
// |    \    |    /    |
// |     \   |   /     |
// | 8    \  |  /   5  |
// |       \ | /       |
// |        \|/        |
// +---------+---------+
// |        /|\        |
// |       / | \       |
// | 1    /  |  \   4  |
// |     /   |   \     |
// |    /    |    \    |
// |   /     |     \   |
// |  /   2  |  3   \  |
// | /       |       \ |
// |/        |        \|
// +---------+---------+
//
// This doesn't change anything fundamentally, but just to note.
//
// OK, so the general idea is this: we scan octants and save "distance" (called
// "shadow" here) between bottom right and upper left corners of a scanned tile.
// I deliberately used "distance" in quotes, because actually it's slopes of
// lines that go through upper left and bottom right corner of a given tile,
// starting from scan point. This is what's called "projection" in the article.
// And the idea is to scan the octant and gather all projections (and merge them
// if needed) into shadow line(s), and then determine if given tile falls into
// this shadow line, thus making it invisible. So the algorithm looks like this:
//
// 1. Start with no shadow line.
// 2. For each tile check its projection against shadow line. If it falls inside
//    it, it's not visible.
// 3. Otherwise mark it as visible and, if it's an opaque tile, add its
//    projection to shadow line (merge if needed).
// 4. Rinse and repeat until out of bounds.
//
// Let's work in octant 5 for this example (it will work automatically in
// others because of symmetry). Let's zoom in and see what this means
// (unfortunately ASCII graphics is not good for drawing such things, so it's
// not to scale and looks like shit, obviously):
//
//          +-+
//          |5|
//        +-+-+
//      / |2|4|
//    /   +-+-+
//  /   / |1|3|
//     /--+-+-+----
//
// For example, lines that go from scan origin and through upper left and
// bottom right points of tile 1 will have slopes 0.5 and 0 respectively.
// So projections will be:
//
//      BR       UL
//
// 1 - (0,       0.5)
// 2 - (0.3333,  1.0)
// 3 - (0,    0.3333)
// 4 - (0.25, 0.6667)
// 5 - (0.5,     1.0)
//
// I tried to visualize it in Desmos, but it doesn't quite work as one would
// think. For some reason graph's grid doesn't quite match what's described in
// algorithm. It kinda works if you zoom in to 0.5 resolution and assume tile 1
// is at (2.5, 0.5), 2 - at (2.5, 1.5) and so on.
//
// Or maybe I'm stupid or something.
//
// Anyway, you can check graph.png in tests/shadowcaster.
//
// Also, I still don't get why it's +2 and (+1, +1) in method below.
//
const Shadowcaster::Shadow& Shadowcaster::ProjectTile(const int row,
                                                      const int col)
{
  //
  // NOTE: cache is not needed since it will work even slower (checked via
  // in-game profiler).
  //
  double topLeft     = (double)col / (double)(row + 2);
  double bottomRight = (double)(col + 1) / (double)(row + 1);

  _shadow.Start = topLeft;
  _shadow.End   = bottomRight;

  return _shadow;
}

// =============================================================================

//
// Some symmetry shuffle, because (2, 3) in one octant is (-2, 3) in another and
// so on.
//
const PairI& Shadowcaster::TransformOctant(int row, int col, uint8_t octant)
{
  switch (octant)
  {
    case 0:
    {
      _transformedCoords.first  =  col;
      _transformedCoords.second = -row;
    }
    break;

    case 1:
    {
      _transformedCoords.first  =  row;
      _transformedCoords.second = -col;
    }
    break;

    case 2:
    {
      _transformedCoords.first  = row;
      _transformedCoords.second = col;
    }
    break;

    case 3:
    {
      _transformedCoords.first  = col;
      _transformedCoords.second = row;
    }
    break;

    case 4:
    {
      _transformedCoords.first  = -col;
      _transformedCoords.second =  row;
    }
    break;

    case 5:
    {
      _transformedCoords.first  = -row;
      _transformedCoords.second =  col;
    }
    break;


    case 6:
    {
      _transformedCoords.first  = -row;
      _transformedCoords.second = -col;
    }
    break;

    case 7:
    {
      _transformedCoords.first  = -col;
      _transformedCoords.second = -row;
    }
    break;

    default:
    {
      _transformedCoords.first  = 0;
      _transformedCoords.second = 0;
    }
    break;
  }

  return _transformedCoords;
}

// =============================================================================

void Shadowcaster::RefreshOctant(uint8_t octant)
{
  auto& player     = Game::gApp.PlayerInstance;
  auto& mapArray   = Game::gMap.CurrentLevel->MapArray;
  auto& staticObjs = Game::gMap.CurrentLevel->StaticMapObjects;

  ShadowLine line;
  bool fullShadow = false;

  Position p;

  //
  // We can always see the cell we're standing at. This gets rid of FoW colored
  // background for player character sprite.
  //
  Game::gApp.PlayerInstance.DiscoverCell(_playerPos.X, _playerPos.Y);

  for (int row = 1; ; row++)
  {
    PairI transformed = TransformOctant(row, 0, octant);

    p = { _playerPos.X + transformed.first,
          _playerPos.Y + transformed.second };

    //
    // Create circle-like visibility radius.
    //
    if (Util::LinearDistance(_playerPos, p) >= player.VisibilityRadius.Get())
    {
      break;
    }

    //
    // Stop once we go out of bounds rowwise.
    //
    if (!Util::IsInsideMap(p, Game::gMap.CurrentLevel->MapSize, false))
    {
      break;
    }

    for (int col = 0; col <= row; col++)
    {
      PairI transformed = TransformOctant(row, col, octant);

      p = { _playerPos.X + transformed.first,
            _playerPos.Y + transformed.second };

      //
      // Create circle-like visibility radius.
      //
      if (Util::LinearDistance(_playerPos, p) >= player.VisibilityRadius.Get())
      {
        break;
      }

      //
      // Stop once we go out of bounds columnwise.
      //
      if (!Util::IsInsideMap(p, Game::gMap.CurrentLevel->MapSize, false))
      {
        break;
      }

      if (fullShadow)
      {
        break;
      }
      else
      {
        Shadow projection = ProjectTile(row, col);

        bool visible = !line.IsInShadow(projection);

        bool blocker = (
          mapArray[p.X][p.Y]->BlocksSight ||
          (staticObjs[p.X][p.Y] != nullptr && staticObjs[p.X][p.Y]->BlocksSight)
        );

        if (visible)
        {
          Game::gApp.PlayerInstance.DiscoverCell(p.X, p.Y);

          if (blocker)
          {
            line.Add(projection);
            fullShadow = line.IsInFullShadow();
          }
        }
      }
    }
  }
}

// =============================================================================

void Shadowcaster::RefreshVisibility()
{
  for (uint8_t octant = 0; octant < 8; octant++)
  {
    RefreshOctant(octant);
  }
}

// =============================================================================
//
// Extra structs follow.
//
// =============================================================================

bool Shadowcaster::Shadow::Contains(const Shadow& other)
{
  return (Start <= other.Start && End >= other.End);
}

// =============================================================================

bool Shadowcaster::ShadowLine::IsInShadow(const Shadow& projection)
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

// =============================================================================

void Shadowcaster::ShadowLine::Add(const Shadow& shadow)
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

// =============================================================================

bool Shadowcaster::ShadowLine::IsInFullShadow()
{
  return (!_shadows.empty() && _shadows[0].Start == 0 && _shadows[0].End == 1);
}

