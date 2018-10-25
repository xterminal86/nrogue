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
  Image = '@';
  FgColor = GlobalConstants::PlayerColor;
  Attrs.ActionMeter = 100;

  SetAttributes();
  SetDefaultEquipment();

  _currentCell = &Map::Instance().MapArray[PosX][PosY];
  _currentCell->Occupied = true;  

  // FIXME: remove afterwards
  for (int i = 0; i < 20; i++)
  {
    auto go = new GameObject(2, 2 + i, 'O', "#FFFFFF");
    go->ObjectName = "Rock";

    auto ic = go->AddComponent<ItemComponent>();
    ((ItemComponent*)ic)->Description = { "This is a placeholder test object" };

    Map::Instance().InsertGameObject(go);

    //auto msg = Util::StringFormat("Created 0x%X", go);
    //Logger::Instance().Print(msg);
  }

  for (int i = 0; i < 3; i++)
  {
    auto go = new GameObject(3 + i, 1, 'o', "#FFFFFF");

    auto name = Util::StringFormat("Ring %i", i);
    go->ObjectName = name;

    auto ic = go->AddComponent<ItemComponent>();
    ((ItemComponent*)ic)->EquipmentType = EquipmentCategory::RING;
    ((ItemComponent*)ic)->Description = { "This is a placeholder equippable object" };

    Map::Instance().InsertGameObject(go);
  }

  auto go = new GameObject(10, 1, 'O', "#FFFFFF");
  go->ObjectName = "Medallion";

  auto ic = go->AddComponent<ItemComponent>();
  ((ItemComponent*)ic)->EquipmentType = EquipmentCategory::NECK;
  ((ItemComponent*)ic)->Description = { "This is a placeholder equippable object" };

  Map::Instance().InsertGameObject(go);
}

void Player::Draw()
{
  bool cond = (BgColor.length() == 0 || BgColor == "#000000");
  GameObject::Draw(FgColor, cond ? Map::Instance().MapArray[PosX][PosY].BgColor : BgColor);
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

  // FIXME: think
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
  Attrs.Str.Talents = 3;
  Attrs.Def.Talents = 2;
  Attrs.HP.Talents = 2;

  Attrs.Str.Set(2);
  Attrs.Def.Set(2);
  Attrs.Spd.Set(60);

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
  Attrs.Spd.Talents = 2;
  Attrs.Skl.Talents = 2;
  Attrs.Def.Talents = 1;

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
  Attrs.Mag.Talents = 3;
  Attrs.Res.Talents = 3;
  Attrs.MP.Talents = 3;

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

void Player::SetDefaultEquipment()
{
  // Initialize map

  EquipmentByCategory[EquipmentCategory::HEAD] = { nullptr };
  EquipmentByCategory[EquipmentCategory::NECK] = { nullptr };
  EquipmentByCategory[EquipmentCategory::TORSO] = { nullptr };
  EquipmentByCategory[EquipmentCategory::LEGS] = { nullptr };
  EquipmentByCategory[EquipmentCategory::BOOTS] = { nullptr };
  EquipmentByCategory[EquipmentCategory::WEAPON] = { nullptr };
  EquipmentByCategory[EquipmentCategory::RING] = { nullptr, nullptr };

  // TODO: assign default equipment according to selected class
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
    int defaultHitChance = 50;
    int hitChance = defaultHitChance;

    int d = Attrs.Skl.CurrentValue - go->Attrs.Skl.CurrentValue;
    if (d > 0)
    {
      hitChance += (d * 5);
    }
    else
    {
      hitChance -= (d * 5);
    }

    hitChance = Util::Clamp(hitChance, GlobalConstants::MinHitChance, GlobalConstants::MaxHitChance);

    auto logMsg = Util::StringFormat("Player (SKL %i, LVL %i) attacks %s (SKL: %i, LVL %i): chance = %i",
                                     Attrs.Skl.CurrentValue,
                                     Attrs.Lvl.CurrentValue,
                                     go->ObjectName.data(),
                                     go->Attrs.Skl.CurrentValue,
                                     go->Attrs.Lvl.CurrentValue,
                                     hitChance);
    Logger::Instance().Print(logMsg);

    if (Util::RollDice(hitChance))
    {
      int dmg = Attrs.Str.CurrentValue - go->Attrs.Def.CurrentValue;
      if (dmg <= 0)
      {
        dmg = 1;
      }

      go->ReceiveDamage(this, dmg);
    }
    else
    {
      Printer::Instance().AddMessage("You missed");
    }
  }

  SubtractActionMeter();
}

void Player::ReceiveDamage(GameObject* from, int amount)
{  
  auto str = Util::StringFormat("You were hit for %i damage", from->ObjectName.data(), amount);
  Printer::Instance().AddMessage(str);

  Attrs.HP.CurrentValue -= amount;

  if (Attrs.HP.CurrentValue <= 0)
  {
    Image = '%';
    FgColor = "#FF0000";

    Printer::Instance().AddMessage("You are dead. Not big surprise.");

    if (from != nullptr)
    {
      auto str = Util::StringFormat("%s was killed by a %s", Name.data(), from->ObjectName.data());
      Printer::Instance().AddMessage(str);
    }

    Application::Instance().ChangeState(Application::GameStates::ENDGAME_STATE);
  }
}

void Player::AwardExperience(int amount)
{
  int amnt = amount * (Attrs.Exp.Talents + 1);

  Attrs.Exp.CurrentValue += amnt;

  if (Attrs.Exp.CurrentValue >= 100)
  {
    Attrs.Exp.Set(0);
    LevelUp();
  }
}

void Player::LevelUp()
{
  for (auto& a : MainAttributes)
  {
    if (CanRaiseAttribute(*a))
    {
      a->OriginalValue++;
      a->CurrentValue = a->OriginalValue;
    }
  }

  int minRndHp = (Attrs.HP.Talents + 1);
  int maxRndHp = 2 * (Attrs.HP.Talents + 1);
  int hpToAdd = RNG::Instance().RandomRange(minRndHp, maxRndHp);
  Attrs.HP.OriginalValue += hpToAdd;

  int minRndMp = Attrs.Mag.OriginalValue;
  int maxRndMp = Attrs.Mag.OriginalValue * (Attrs.MP.Talents + 1);

  int mpToAdd = RNG::Instance().RandomRange(minRndMp, maxRndMp);
  Attrs.MP.OriginalValue += mpToAdd;

  /*
  auto class_ = _classesMap[SelectedClass];

  switch (class_)
  {
    case PlayerClass::SOLDIER:
      break;

    case PlayerClass::THIEF:
      break;

    case PlayerClass::ARCANIST:
      break;

    case PlayerClass::CUSTOM:
      break;
  }
  */

  Attrs.Lvl.OriginalValue++;
  Attrs.Lvl.CurrentValue = Attrs.Lvl.OriginalValue;
}

bool Player::CanRaiseAttribute(Attribute& attr)
{
  bool success = false;

  int chance = GlobalConstants::AttributeMinimumRaiseChance;

  int iterations = attr.Talents;
  for (int i = 0; i < iterations; i++)
  {
    int res = Util::RollDice(chance);
    if (res != 0)
    {
      return true;
    }

    chance += GlobalConstants::AttributeMinimumRaiseChance;
  }

  int res = Util::RollDice(GlobalConstants::AttributeMinimumRaiseChance);
  return (res != 0);
}
