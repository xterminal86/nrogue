#include "player.h"

#include "map.h"
#include "printer.h"
#include "util.h"
#include "door-component.h"
#include "game-objects-factory.h"

void Player::Init()
{
  PosX = Map::Instance().PlayerStartX;
  PosY = Map::Instance().PlayerStartY;
  Avatar = '@';
  Color = "#00FFFF";
  ActionMeter = 100;

  SetAttributes();

  _currentCell = &Map::Instance().MapArray[PosX][PosY];
  _currentCell->Occupied = true;

  auto go = GameObjectsFactory::Instance().CreateMoney();
  Inventory.AddObject(go);
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

  int radius = (map[PosX][PosY].ObjectName == "Tree") ? VisibilityRadius / 4 : VisibilityRadius;

  auto mapCells = Util::GetRectAroundPoint(PosX, PosY, tw / 2, th / 2);
  for (auto& cell : mapCells)
  {
    map[cell.X][cell.Y].Visible = false;
  }

  // Update visibility around player

  for (auto& cell : mapCells)
  {
    float d = Util::LinearDistance(PosX, PosY, cell.X, cell.Y);

    if (d > (float)radius)
    {
      continue;
    }

    // Bresenham FoV

    auto line = Util::BresenhamLine(PosX, PosY, cell.X, cell.Y);
    for (auto& point : line)
    {
      if (point.X == PosX && point.Y == PosY)
      {
        continue;
      }

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
      SetSoldierAttrs();
      break;

    case PlayerClass::THIEF:
      SetThiefAttrs();
      break;

    case PlayerClass::ARCANIST:
      SetArcanistAttrs();
      break;

    default:
      // TODO: custom class defaults to knight for now
      SetSoldierAttrs();
      break;
  }
}

void Player::SetSoldierAttrs()
{
  Actor.Attrs.Str.IsTalent = true;
  Actor.Attrs.Def.IsTalent = true;
  Actor.Attrs.HP.IsTalent = true;

  Actor.Attrs.Str.Set(2);
  Actor.Attrs.Def.Set(2);
  Actor.Attrs.Spd.Set(80);

  Actor.Attrs.HP.Set(40);

  Actor.Attrs.HungerRate.Set(50);
  Actor.Attrs.HungerSpeed.Set(2);
}

void Player::SetThiefAttrs()
{
  Actor.Attrs.Spd.IsTalent = true;
  Actor.Attrs.Skl.IsTalent = true;

  Actor.Attrs.Def.Set(1);
  Actor.Attrs.Skl.Set(2);
  Actor.Attrs.Spd.Set(120);

  Actor.Attrs.HP.Set(25);
  Actor.Attrs.MP.Set(10);

  Actor.Attrs.HungerRate.Set(75);
  Actor.Attrs.HungerSpeed.Set(2);
}

void Player::SetArcanistAttrs()
{
  Actor.Attrs.Mag.IsTalent = true;
  Actor.Attrs.Res.IsTalent = true;
  Actor.Attrs.MP.IsTalent = true;

  Actor.Attrs.Mag.Set(2);
  Actor.Attrs.Res.Set(2);
  Actor.Attrs.Spd.Set(100);

  Actor.Attrs.HP.Set(10);
  Actor.Attrs.MP.Set(50);

  Actor.Attrs.HungerRate.Set(100);
  Actor.Attrs.HungerSpeed.Set(1);
}
