#include <sstream>

#include "shadowcaster.h"

#include "application.h"
#include "map.h"
#include "util.h"

Shadowcaster::Shadowcaster() {}

// =============================================================================

Shadowcaster::Shadowcaster(const Position& playerPos)
{
  _playerPos.X = playerPos.X;
  _playerPos.Y = playerPos.Y;
}

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

const Shadow& Shadowcaster::ProjectTile(const int row, const int col)
{
  // NOTE: cache is not needed since it will work slower (checked via in-game
  // profiler).

  // TODO: figure this out
  double topLeft     = (double)col / (double)(row + 2);
  double bottomRight = (double)(col + 1) / (double)(row + 1);

  _shadow.Start = topLeft;
  _shadow.End   = bottomRight;

  return _shadow;
}

// =============================================================================

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

    // Stop once we go out of bounds.
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

      // Stop once we go out of bounds.
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

bool Shadow::Contains(const Shadow& other)
{
  return (Start <= other.Start && End >= other.End);
}

// =============================================================================

bool ShadowLine::IsInShadow(const Shadow& projection)
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

void ShadowLine::Add(const Shadow& shadow)
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

bool ShadowLine::IsInFullShadow()
{
  return (!_shadows.empty() && _shadows[0].Start == 0 && _shadows[0].End == 1);
}

