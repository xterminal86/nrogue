#include "target-state.h"

#include "application.h"
#include "printer.h"
#include "map.h"
#include "ai-component.h"
#include "door-component.h"
#include "spells-database.h"
#include "game-objects-factory.h"

void TargetState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;  
}

void TargetState::Prepare()
{  
  _maxThrowingRange = (_playerRef->Attrs.Str.Get() <= 0) ? 4 : _playerRef->Attrs.Str.Get() + 4;

  _drawHint = true;

  _cursorPosition.X = _playerRef->PosX;
  _cursorPosition.Y = _playerRef->PosY;

  FindTargets();

  if (!_targets.empty())
  {
    _lastTargetIndex = 0;

    for (int i = 0; i < _targets.size(); i++)
    {
      if (_targets[i]->PosX == _lastCursorPosition.X
       && _targets[i]->PosY == _lastCursorPosition.Y)
      {
        _cursorPosition = _lastCursorPosition;
        _lastTargetIndex = i;
        break;
      }
    }

    if (_lastTargetIndex == 0)
    {
      int x = _targets[0]->PosX;
      int y = _targets[0]->PosY;

      _cursorPosition.Set(x, y);
    }
  }
}

void TargetState::Cleanup()
{
  _throwingItemInventoryIndex = -1;
  _weaponRef = nullptr;
}

void TargetState::FindTargets()
{
  _targets.clear();

  int r = _playerRef->VisibilityRadius.Get();
  int px = _playerRef->PosX;
  int py = _playerRef->PosY;

  int lx = px - r;
  int ly = py - r;
  int hx = px + r;
  int hy = py + r;

  for (int x = lx; x <= hx; x++)
  {
    for (int y = ly; y <= hy; y++)
    {
      if (x == px && y == py)
      {
        continue;
      }

      float d = Util::LinearDistance(px, py, x, y);

      if (Util::IsInsideMap({ x, y }, Map::Instance().CurrentLevel->MapSize)
       && Map::Instance().CurrentLevel->MapArray[x][y]->Visible
       && (int)d <= r)
      {
        auto actor = Map::Instance().GetActorAtPosition(x, y);
        if (actor != nullptr)
        {
          _targets.push_back(actor);
        }
      }
    }
  }
}

void TargetState::CycleTargets()
{
  if (!_targets.empty())
  {
    _lastTargetIndex++;

    if (_lastTargetIndex >= _targets.size())
    {
      _lastTargetIndex = 0;
    }

    int x = _targets[_lastTargetIndex]->PosX;
    int y = _targets[_lastTargetIndex]->PosY;

    _cursorPosition.Set(x, y);
  }
}

void TargetState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case 'q':
    case NUMPAD_7:
      MoveCursor(-1, -1);
      break;

    case 'w':
    case NUMPAD_8:
      MoveCursor(0, -1);
      break;

    case 'e':
    case NUMPAD_9:
      MoveCursor(1, -1);
      break;

    case 'a':
    case NUMPAD_4:
      MoveCursor(-1, 0);
      break;

    case 'd':
    case NUMPAD_6:
      MoveCursor(1, 0);
      break;

    case 'z':
    case NUMPAD_1:
      MoveCursor(-1, 1);
      break;

    case 'x':
    case NUMPAD_2:
      MoveCursor(0, 1);
      break;

    case 'c':
    case NUMPAD_3:
      MoveCursor(1, 1);
      break;

    case VK_TAB:
      CycleTargets();
      break;

    case VK_ENTER:
    case 'f':
      FireWeapon((_throwingItemInventoryIndex != -1));
      break;

    case VK_CANCEL:
      Printer::Instance().AddMessage("Cancelled");
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }
}

GameObject* TargetState::LaunchProjectile(char image, const std::string& color)
{
  GameObject* stoppedAt = nullptr;

  Position startPoint = { _playerRef->PosX, _playerRef->PosY };
  Position endPoint = _cursorPosition;

  auto line = Util::BresenhamLine(startPoint, endPoint);

  int distanceCovered = 0;

  // Don't include player's position
  for (int i = 1; i < line.size(); i++)
  {    
    int mx = line[i].X;
    int my = line[i].Y;

    Update(true);

    int drawingPosX = mx + Map::Instance().CurrentLevel->MapOffsetX;
    int drawingPosY = my + Map::Instance().CurrentLevel->MapOffsetY;

    Printer::Instance().PrintFB(drawingPosX, drawingPosY, image, color);
    Printer::Instance().Render();

    // FIXME: debug
    // Util::Sleep(100);

    //#ifndef USE_SDL
    Util::Sleep(20);
    //#endif

    stoppedAt = CheckHit({ mx, my }, { line[i - 1].X, line[i - 1].Y });

    distanceCovered++;

    bool isThrowing = (_throwingItemInventoryIndex != -1);

    // Hit object or max shooting distance reached
    if (stoppedAt != nullptr)
    {
      break;
    }
    else if ((!isThrowing && distanceCovered >= _weaponRef->Data.Range)
           || (isThrowing && distanceCovered >= _maxThrowingRange))
    {
      endPoint.Set(mx, my);
      break;
    }
  }

  // Projectile reached end point without hitting anyone
  if (stoppedAt == nullptr)
  {
    auto cell = Map::Instance().CurrentLevel->MapArray[endPoint.X][endPoint.Y].get();
    stoppedAt = cell;
  }

  return stoppedAt;
}

GameObject* TargetState::CheckHit(const Position& at, const Position& prev)
{
  auto actor = Map::Instance().GetActorAtPosition(at.X, at.Y);
  if (actor != nullptr)
  {
    return actor;
  }

  auto cell = Map::Instance().CurrentLevel->StaticMapObjects[at.X][at.Y].get();
  if (cell != nullptr)
  {    
    // If door is open, ignore it
    DoorComponent* dc = cell->GetComponent<DoorComponent>();
    if (dc != nullptr && dc->IsOpen)
    {
      return nullptr;
    }

    if (_throwingItemInventoryIndex != -1)
    {
      if (cell->Blocking)
      {
        auto prevCell = Map::Instance().CurrentLevel->MapArray[prev.X][prev.Y].get();
        return prevCell;
      }
      else
      {
        return cell;
      }
    }
    else
    {
      if (cell->Blocking)
      {
        if (_weaponRef->Data.ItemType_ == ItemType::RANGED_WEAPON)
        {
          return cell;
        }
        else if (_weaponRef->Data.ItemType_ == ItemType::WAND)
        {
          if (_weaponRef->Data.SpellHeld == SpellType::FIREBALL)
          {
            auto prevCell = Map::Instance().CurrentLevel->MapArray[prev.X][prev.Y].get();
            return prevCell;
          }
          else
          {
            return cell;
          }
        }
      }
      else
      {
        return cell;
      }
    }
  }

  return nullptr;
}

bool TargetState::SafetyCheck()
{
  bool posCheck = (_cursorPosition.X == _playerRef->PosX
                && _cursorPosition.Y == _playerRef->PosY);

  return posCheck;
}

void TargetState::CheckCursorPositionBounds()
{
  Position lastPositionInsideMap;
  bool isOutsideMap = false;
  auto line = Util::BresenhamLine({ _playerRef->PosX, _playerRef->PosY }, _cursorPosition);
  for (auto& p : line)
  {
    if (!Util::IsInsideMap(p, Map::Instance().CurrentLevel->MapSize, false))
    {
      isOutsideMap = true;
      break;
    }

    lastPositionInsideMap = p;
  }

  if (isOutsideMap)
  {
    _cursorPosition = lastPositionInsideMap;
  }
}

/// It is assumed that we have valid weapon and ammunition
/// in corresponding equipment slots
/// (necessary checks were performed in MainState)
void TargetState::FireWeapon(bool throwingFromInventory)
{
  // Do not target self.
  if (SafetyCheck())
  {
    return;
  }

  // If cursor is outside map,
  // pick last valid point as target end point.
  CheckCursorPositionBounds();

  _lastCursorPosition = _cursorPosition;

  _drawHint = false;

  std::string weaponName = _weaponRef->Data.IsIdentified ? _weaponRef->Data.IdentifiedName : _weaponRef->Data.UnidentifiedName;

  std::string str = throwingFromInventory ? "You throw " : "You fire ";
  str += weaponName;

  Printer::Instance().AddMessage(str);

  int chance = CalculateHitChance();
  if (!Util::Rolld100(chance))
  {    
    auto rect = Util::GetEightPointsAround(_cursorPosition, Map::Instance().CurrentLevel->MapSize);

    bool outOfRange = false;

    // If we shoot from point blank in a corridor,
    // we shouldn't accidentaly target ourselves
    // due to lack of skill.
    for (int i = 0; i < rect.size(); i++)
    {
      // Do not include points above weapon's maximum range as well.
      int d = Util::LinearDistance({ _playerRef->PosX, _playerRef->PosY }, rect[i]);

      if (d > _weaponRef->Data.Range)
      {
        outOfRange = true;
      }

      if ((rect[i].X == _playerRef->PosX
        && rect[i].Y == _playerRef->PosY)
        || outOfRange)
      {
        rect.erase(rect.begin() + i);
        break;
      }
    }

    int index = RNG::Instance().RandomRange(0, rect.size());
    _cursorPosition = rect[index];

    /*
    if (!outOfRange)
    {
      Printer::Instance().AddMessage("The shot goes astray");
    }
    */
  }

  GameObject* stoppedAt = nullptr;

  std::string projColor = "#FFFFFF";

  bool isWand = (_weaponRef->Data.ItemType_ == ItemType::WAND);
  bool isWeapon = (_weaponRef->Data.ItemType_ == ItemType::RANGED_WEAPON);

  char projectile = ' ';
  if (throwingFromInventory)
  {
    projectile = _weaponRef->OwnerGameObject->Image;
  }
  else
  {
    if (isWand)
    {
      projectile = '*';

      SpellType spell = _weaponRef->Data.SpellHeld;
      SpellInfo* si = SpellsDatabase::Instance().GetInfo(spell);
      if (!si->SpellProjectileColor.empty())
      {
        projColor = si->SpellProjectileColor;
      }
    }
    else if (isWeapon)
    {
      projectile = '+';
    }
  }

  stoppedAt = LaunchProjectile(projectile, projColor);
  ProcessHit(stoppedAt);

  _playerRef->FinishTurn();

  DirtyHack();
}

void TargetState::DirtyHack()
{
  // In case player gained a level after firing a ranged weapon,
  // message box state will be overridden by one of the
  // ChangeState() calls below.
  // Should either introduce a hack and display message box
  // after those calls below, or refactor message box state.
  //
  // Check if player accidentally killed himself
  // (e.g. after firing fireball close to the wall)
  if (!_playerRef->IsAlive())
  {
    Application::Instance().ChangeState(GameStates::ENDGAME_STATE);
  }
  else
  {
    // NOTE: OK, so dirty hack it is, because I don't have time
    // to refactor all this bullshit.
    //
    // If we are in message box state, it means player got level up
    // in ProcessHit() and changed state to MessageBoxState
    // to show level up window.
    // To fix state override, manually set
    // previous state variable to MainState
    // for message box, so that after closing we kinda
    // "change state" to MainState.
    // Looks safe (haha) since we don't have
    // any Cleanup() or Prepare() to do in this case.
    if (Application::Instance().CurrentStateIs(GameStates::MESSAGE_BOX_STATE))
    {
      Application::Instance()._previousState = Application::Instance().GetGameStateRefByName(GameStates::MAIN_STATE);
    }
    else
    {
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
    }
  }
}

void TargetState::ProcessHit(GameObject* hitPoint)
{
  bool isWand = (_weaponRef->Data.ItemType_ == ItemType::WAND);
  bool isRangedWeapon = (_weaponRef->Data.ItemType_ == ItemType::RANGED_WEAPON);

  if (_throwingItemInventoryIndex != -1)
  {
    // _weaponRef is item to be thrown

    auto& mapRef = Map::Instance().CurrentLevel->MapArray;

    int x = hitPoint->PosX;
    int y = hitPoint->PosY;

    bool isStackable = (!isWand && !isRangedWeapon && _weaponRef->Data.IsStackable);

    bool tileOk = (mapRef[x][y]->Type != GameObjectType::DEEP_WATER
                && mapRef[x][y]->Type != GameObjectType::LAVA);

    // TODO: potions break on impact, items damage monsters (not?)
    // throwing one item at a time from the stack of items.

    if (isStackable)
    {
      _weaponRef->Data.Amount--;

      if (tileOk)
      {
        ItemComponent* copy = GameObjectsFactory::Instance().CloneItem(_weaponRef);
        copy->Data.Amount = 1;

        copy->OwnerGameObject->PosX = x;
        copy->OwnerGameObject->PosY = y;

        Map::Instance().CurrentLevel->InsertGameObject(copy->OwnerGameObject);
      }
      else
      {
        PrintThrowResult(mapRef[x][y].get());
      }

      if (_weaponRef->Data.Amount == 0)
      {
        auto it = _playerRef->Inventory.Contents.begin();
        _playerRef->Inventory.Contents.erase(it + _throwingItemInventoryIndex);
      }
    }
    else
    {
      if (tileOk)
      {
        GameObject* item = _playerRef->Inventory.Contents[_throwingItemInventoryIndex].release();

        // See comments in InventoryState::DropItem()
        item->SetLevelOwner(Map::Instance().CurrentLevel);

        item->PosX = x;
        item->PosY = y;

        Map::Instance().CurrentLevel->InsertGameObject(item);
      }
      else
      {
        PrintThrowResult(mapRef[x][y].get());
      }

      auto it = _playerRef->Inventory.Contents.begin();
      _playerRef->Inventory.Contents.erase(it + _throwingItemInventoryIndex);
    }
  }
  else
  {
    if (isWand)
    {
      _playerRef->MagicAttack(hitPoint, _weaponRef);
    }
    else if (isRangedWeapon)
    {
      _playerRef->RangedAttack(hitPoint, _weaponRef);
    }
  }
}

void TargetState::PrintThrowResult(GameObject* tileRef)
{
  GameObjectType tile = tileRef->Type;

  std::string objName = _weaponRef->OwnerGameObject->ObjectName;
  std::string verb;
  std::string tileName = tileRef->ObjectName;

  if (tile == GameObjectType::DEEP_WATER)
  {
    verb = "drowns";
  }
  else if (tile == GameObjectType::LAVA)
  {
    verb = "burns";
  }

  if (!verb.empty())
  {
    auto str = Util::StringFormat("%s %s in %s!", objName.data(), verb.data(), tileName.data());
    Printer::Instance().AddMessage(str);
  }
}

void TargetState::MoveCursor(int dx, int dy)
{
  int nx = _cursorPosition.X + dx;
  int ny = _cursorPosition.Y + dy;

  int hw = Printer::Instance().TerminalWidth / 2;
  int hh = Printer::Instance().TerminalHeight / 2;

  nx = Util::Clamp(nx, _playerRef->PosX - hw + 1,
                       _playerRef->PosX + hw - 2);

  ny = Util::Clamp(ny, _playerRef->PosY - hh + 1,
                       _playerRef->PosY + hh - 2);

  _cursorPosition.X = nx;
  _cursorPosition.Y = ny;  
}

void TargetState::DrawHint()
{
  Position startPoint = { _playerRef->PosX, _playerRef->PosY };

  int mox = Map::Instance().CurrentLevel->MapOffsetX;
  int moy = Map::Instance().CurrentLevel->MapOffsetY;

  Position mapSize = Map::Instance().CurrentLevel->MapSize;
  auto& mapRef = Map::Instance().CurrentLevel->MapArray;
  auto& staticObjRef = Map::Instance().CurrentLevel->StaticMapObjects;

  std::vector<Position> cellsToHighlight;

  auto line = Util::BresenhamLine(startPoint, _cursorPosition);
  for (auto& p : line)
  {
    if (p == startPoint)
    {
      continue;
    }

    if (Util::IsInsideMap(p, mapSize))
    {
      auto actor = Map::Instance().GetActorAtPosition(p.X, p.Y);

      bool actorPresent = (actor != nullptr);

      int d = Util::LinearDistance(startPoint, p);

      bool isCellBlocking = Map::Instance().CurrentLevel->IsCellBlocking(p);
      bool isThrowing = (_throwingItemInventoryIndex != -1);      
      bool isLaser = (_weaponRef->Data.SpellHeld == SpellType::LASER);
      bool isThrowingOk = ((!isThrowing && d > _weaponRef->Data.Range)
                         || (isThrowing && d > _maxThrowingRange));

      if (!isLaser && (actorPresent || isCellBlocking || isThrowingOk))
      {
        break;
      }
      else
      {
        cellsToHighlight.push_back(p);
      }
    }
  }

  for (auto& p : cellsToHighlight)
  {
    Printer::Instance().PrintFB(p.X + mox, p.Y + moy, '.', "#FF0000");
  }
}

void TargetState::DrawCursor()
{
  int mox = Map::Instance().CurrentLevel->MapOffsetX;
  int moy = Map::Instance().CurrentLevel->MapOffsetY;

  Printer::Instance().PrintFB(_cursorPosition.X + mox + 1,
                              _cursorPosition.Y + moy,
                              ']', "#FFFFFF");

  Printer::Instance().PrintFB(_cursorPosition.X + mox - 1,
                              _cursorPosition.Y + moy,
                              '[', "#FFFFFF");
}

void TargetState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Map::Instance().Draw();

    _playerRef->Draw();

    if (_drawHint)
    {
      DrawHint();
      DrawCursor();
    }

    int tw = Printer::Instance().TerminalWidth;

    Printer::Instance().PrintFB(tw / 2, 0,
                                "Select target then press 'f' or 'Enter' to fire",
                                Printer::kAlignCenter,
                                "#FFFFFF");

    Printer::Instance().PrintFB(tw / 2, 1,
                                "(TAB to cycle through visible ones)",
                                Printer::kAlignCenter,
                                "#FFFFFF");

    Printer::Instance().Render();
  }
}

void TargetState::Setup(ItemComponent* weapon)
{
  _weaponRef = weapon;
}

void TargetState::SetupForThrowing(ItemComponent* itemToThrow, int throwingItemInventoryIndex)
{
  _throwingItemInventoryIndex = throwingItemInventoryIndex;
  _weaponRef = itemToThrow;
}

int TargetState::CalculateHitChance()
{
  int baseChance = 50;
  int chance = 0;

  Position startPoint = { _playerRef->PosX, _playerRef->PosY };
  Position endPoint = _cursorPosition;

  if (_weaponRef->Data.ItemType_ == ItemType::WAND
   || _throwingItemInventoryIndex != -1)
  {
    //chance = CalculateChance(startPoint, endPoint, baseChance);

    // TODO: too OP for wands?
    chance = 100;
  }
  else
  {
    bool isXBow = (_weaponRef->Data.RangedWeaponType_ == RangedWeaponType::LIGHT_XBOW
                || _weaponRef->Data.RangedWeaponType_ == RangedWeaponType::XBOW
                || _weaponRef->Data.RangedWeaponType_ == RangedWeaponType::HEAVY_XBOW);

    baseChance = isXBow ? (baseChance + 15) : baseChance;

    chance = CalculateChance(startPoint, endPoint, baseChance);

    ItemPrefix ammoPrefix = _playerRef->EquipmentByCategory[EquipmentCategory::SHIELD][0]->Data.Prefix;
    switch (ammoPrefix)
    {
      case ItemPrefix::BLESSED:
        chance *= 2;
        break;

      case ItemPrefix::CURSED:
        chance /= 2;
        break;
    }
  }

  auto str = Util::StringFormat("Total unclamped hit chance: %i", chance);
  Logger::Instance().Print(str);

  chance = Util::Clamp(chance, GlobalConstants::MinHitChance, GlobalConstants::MaxHitChance);

  return chance;
}

int TargetState::CalculateChance(const Position& startPoint, const Position& endPoint, int baseChance)
{
  int attackChanceScale = 5;

  int chance = baseChance;

  int skl = _playerRef->Attrs.Skl.Get();
  chance += (attackChanceScale * skl);

  int distanceChanceDrop = 2;

  int d = (int)Util::LinearDistance(startPoint, endPoint);
  chance -= (distanceChanceDrop * d);

  auto str = Util::StringFormat("Calculated hit chance: %i (SKL: %i, SKL bonus: %i, distance: -%i)",
                                chance,
                                skl,
                                (attackChanceScale * skl),
                                (distanceChanceDrop * d));
  Logger::Instance().Print(str);

  return chance;
}
