#include "player.h"

#include "map.h"
#include "printer.h"
#include "util.h"
#include "door-component.h"
#include "game-objects-factory.h"
#include "ai-monster-basic.h"
#include "application.h"

void Player::Init()
{
  PosX = Map::Instance().PlayerStartX;
  PosY = Map::Instance().PlayerStartY;
  Avatar = '@';
  Color = GlobalConstants::PlayerColor;
  ActionMeter = 100;

  SetAttributes();

  _currentCell = &Map::Instance().MapArray[PosX][PosY];
  _currentCell->Occupied = true;  
}

bool Player::TryToAttack(int dx, int dy)
{
  auto go = Map::Instance().GetActorAtPosition(PosX + dx, PosY + dy);
  if (go != nullptr)
  {
    // FIXME: redesign
    auto c = go->GetComponent<AIMonsterBasic>();
    if (c != nullptr)
    {
      Attack(go);
      return true;
    }
  }

  return false;
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
    // Search for components only if there are any

    if (cell.ComponentsSize() != 0)
    {
      auto c = cell.GetComponent<DoorComponent>();

      // Automatically interact with door if it's closed
      if (c != nullptr && cell.Blocking && cell.Interact())
      {
        SubtractActionMeter();
      }
    }
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

      if (map[point.X][point.Y].BlocksSight)
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
      // TODO: custom class defaults to soldier for now
      SetSoldierAttrs();
      break;
  }
}

void Player::SetSoldierAttrs()
{
  Attrs.Str.IsTalent = true;
  Attrs.Def.IsTalent = true;
  Attrs.HP.IsTalent = true;

  Attrs.Str.Set(2);
  Attrs.Def.Set(2);
  Attrs.Spd.Set(80);

  Attrs.HP.Set(40);

  Attrs.HungerRate.Set(50);
  Attrs.HungerSpeed.Set(2);

  auto go = GameObjectsFactory::Instance().CreateHealingPotion();
  auto ic = go->GetComponent<ItemComponent>();
  ((ItemComponent*)ic)->Amount = 3;

  Inventory.AddToInventory(go);
}

void Player::SetThiefAttrs()
{
  Attrs.Spd.IsTalent = true;
  Attrs.Skl.IsTalent = true;
  Attrs.Exp.IsTalent = true;

  Attrs.Def.Set(1);
  Attrs.Skl.Set(2);
  Attrs.Spd.Set(120);

  Attrs.HP.Set(25);

  Attrs.HungerRate.Set(75);
  Attrs.HungerSpeed.Set(2);  

  auto go = GameObjectsFactory::Instance().CreateHealingPotion();
  auto ic = go->GetComponent<ItemComponent>();
  Inventory.AddToInventory(go);

  go = GameObjectsFactory::Instance().CreateManaPotion();
  ic = go->GetComponent<ItemComponent>();
  Inventory.AddToInventory(go);

  go = GameObjectsFactory::Instance().CreateMoney(100);
  ic = go->GetComponent<ItemComponent>();
  Inventory.AddToInventory(go);
}

void Player::SetArcanistAttrs()
{
  Attrs.Mag.IsTalent = true;
  Attrs.Res.IsTalent = true;
  Attrs.MP.IsTalent = true;

  Attrs.Mag.Set(2);
  Attrs.Res.Set(2);
  Attrs.Spd.Set(100);

  Attrs.HP.Set(10);
  Attrs.MP.Set(50);

  Attrs.HungerRate.Set(100);
  Attrs.HungerSpeed.Set(1);

  auto go = GameObjectsFactory::Instance().CreateManaPotion();
  auto ic = go->GetComponent<ItemComponent>();
  ((ItemComponent*)ic)->Amount = 3;

  Inventory.AddToInventory(go);
}

void Player::Attack(GameObject* go)
{
  if (go->Attrs.Indestructible)
  {
    auto str = Util::StringFormat("No effect on %s!", go->ObjectName.data());
    Printer::Instance().AddMessage(str);
  }
  else
  {
    // FIXME: temporary damage
    int dmg = Attrs.Str.CurrentValue - go->Attrs.Def.CurrentValue;
    if (dmg <= 0)
    {
      dmg = 1;
    }

    auto str = Util::StringFormat("You hit %s for %i damage", go->ObjectName.data(), dmg);
    Printer::Instance().AddMessage(str);
    go->ReceiveDamage(dmg);
  }

  SubtractActionMeter();
}

void Player::ReceiveDamage(GameObject* from, int amount)
{
  auto str = Util::StringFormat("%s hits you for %i damage", from->ObjectName.data(), amount);
  Printer::Instance().AddMessage(str);

  Attrs.HP.CurrentValue -= amount;

  if (Attrs.HP.CurrentValue <= 0)
  {
    Avatar = '%';
    Color = "#FF0000";

    Printer::Instance().AddMessage("You are dead. Not big surprise.");
    Application::Instance().ChangeState(Application::GameStates::ENDGAME_STATE);
  }
}
