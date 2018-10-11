#include "player.h"

#include "map.h"
#include "printer.h"
#include "util.h"
#include "door-component.h"

void Player::Init()
{
  PosX = Map::Instance().PlayerStartX;
  PosY = Map::Instance().PlayerStartY;
  Avatar = '@';
  Color = "#00FFFF";

  SetAttributes();

  _currentCell = &Map::Instance().MapArray[PosX][PosY];
  _currentCell->Occupied = true;
}

bool Player::Move(int dx, int dy)
{
  auto& cell = Map::Instance().MapArray[PosX + dx][PosY + dy];

  if (!cell.Occupied && !cell.Blocking)
  {
    _previousCell = &Map::Instance().MapArray[PosX][PosY];
    _previousCell->Occupied = false;

    PosX += dx;
    PosY += dy;

    _currentCell = &Map::Instance().MapArray[PosX][PosY];
    _currentCell->Occupied = true;

    return true;
  }
  else
  {
    cell.Interact();
  }

  return false;
}

void Player::CheckVisibility()
{
  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  // FIXME: think
  //
  // Should we compensate for different terminal sizes,
  // so we get more circle-like visible area around player?
  /*

  int vrx = VisibilityRadius;
  int vry = VisibilityRadius;

  if (tw > th)
  {
    vry /= 2;
  }
  else if (tw < th)
  {
    vrx /= 2;
  }
  */

  // Update map around player

  auto& map = Map::Instance().MapArray;

  auto mapCells = Util::GetRectAroundPoint(PosX, PosY, tw / 2, th / 2);
  for (auto& cell : mapCells)
  {
    map[cell.X][cell.Y].Visible = false;
  }

  // Update visibility around player

  for (auto& cell : mapCells)
  {
    float d = Util::LinearDistance(PosX, PosY, cell.X, cell.Y);

    if (d > (float)VisibilityRadius)
    {
      continue;
    }

    // Bresenham FoV

    auto line = Util::BresenhamLine(PosX, PosY, cell.X, cell.Y);
    for (auto& point : line)
    {
      if (map[point.X][point.Y].Blocking || map[point.X][point.Y].BlockSight)
      {
        DiscoverCell(point.X, point.Y);
        break;
      }

      DiscoverCell(point.X, point.Y);
    }
  }    
}

void Player::Draw()
{
  Printer::Instance().PrintFB(PosX + Map::Instance().MapOffsetX,
                              PosY + Map::Instance().MapOffsetY,
                              Avatar,
                              Color,
                              Map::Instance().MapArray[PosX][PosY].BgColor);
}

void Player::DiscoverCell(int x, int y)
{
  auto& map = Map::Instance().MapArray;

  map[x][y].Visible = true;

  if (!map[x][y].Revealed)
  {
    map[x][y].Revealed = true;
  }
}

void Player::SetAttributes()
{
  switch (_classesMap[SelectedClass])
  {
    case PlayerClass::SOLDIER:
      SetKnightAttrs();
      break;

    case PlayerClass::THIEF:
      SetRogueAttrs();
      break;

    case PlayerClass::ARCANIST:
      SetArcanistAttrs();
      break;

    default:
      // TODO: custom class defaults to knight for now
      SetKnightAttrs();
      break;
  }
}

void Player::SetKnightAttrs()
{
  _actorComponent.Attrs.Str.IsTalent = true;
  _actorComponent.Attrs.Def.IsTalent = true;
  _actorComponent.Attrs.HP.IsTalent = true;

  _actorComponent.Attrs.HungerRate.OriginalValue = 50;
  _actorComponent.Attrs.HungerSpeed.OriginalValue = 2;
}

void Player::SetRogueAttrs()
{
  _actorComponent.Attrs.Spd.IsTalent = true;
  _actorComponent.Attrs.Skl.IsTalent = true;

  _actorComponent.Attrs.HungerRate.OriginalValue = 75;
  _actorComponent.Attrs.HungerSpeed.OriginalValue = 2;
}

void Player::SetArcanistAttrs()
{
  _actorComponent.Attrs.Mag.IsTalent = true;
  _actorComponent.Attrs.Res.IsTalent = true;
  _actorComponent.Attrs.MP.IsTalent = true;

  _actorComponent.Attrs.HungerRate.OriginalValue = 100;
  _actorComponent.Attrs.HungerSpeed.OriginalValue = 1;
}
