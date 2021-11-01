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

    for (size_t i = 0; i < _targets.size(); i++)
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
    case ALT_K7:
    case NUMPAD_7:
      MoveCursor(-1, -1);
      break;

    case ALT_K8:
    case NUMPAD_8:
      MoveCursor(0, -1);
      break;

    case ALT_K9:
    case NUMPAD_9:
      MoveCursor(1, -1);
      break;

    case ALT_K4:
    case NUMPAD_4:
      MoveCursor(-1, 0);
      break;

    case ALT_K6:
    case NUMPAD_6:
      MoveCursor(1, 0);
      break;

    case ALT_K1:
    case NUMPAD_1:
      MoveCursor(-1, 1);
      break;

    case ALT_K2:
    case NUMPAD_2:
      MoveCursor(0, 1);
      break;

    case ALT_K3:
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
      Printer::Instance().AddMessage(Strings::MsgCancelled);
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

  // Start from 1 to exclude player's position
  for (size_t i = 1; i < line.size(); i++)
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
    //Util::Sleep(20);
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
          if (_weaponRef->Data.SpellHeld.SpellType_ == SpellType::FIREBALL)
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

Position TargetState::GetRandomPointAroundCursor()
{
  Position pos = { -1, -1 };

  auto rect = Util::GetEightPointsAround(_cursorPosition, Map::Instance().CurrentLevel->MapSize);

  bool outOfRange = false;

  // If we shoot from point blank in a corridor,
  // we shouldn't accidentaly target ourselves
  // due to lack of skill.
  for (size_t i = 0; i < rect.size(); i++)
  {
    // Do not include points above weapon's maximum range as well.
    int d = Util::LinearDistance({ _playerRef->PosX, _playerRef->PosY }, rect[i]);
    if (d > _weaponRef->Data.Range)
    {
      outOfRange = true;
    }

    // If random point from lack of skill is the one
    // we're standing on, ignore it.
    bool targetSelf = (rect[i].X == _playerRef->PosX
                    && rect[i].Y == _playerRef->PosY);

    if (targetSelf || outOfRange)
    {
      rect.erase(rect.begin() + i);
      break;
    }
  }

  // Just in case
  if (!rect.empty())
  {
    int index = RNG::Instance().RandomRange(0, rect.size());
    pos = rect[index];
  }

  /*
  if (!outOfRange)
  {
    Printer::Instance().AddMessage("The shot goes astray");
  }
  */

  return pos;
}

std::pair<char, std::string> TargetState::GetProjectileImageAndColor(bool throwingFromInventory)
{
  std::pair<char, std::string> res;

  char projectile = ' ';

  std::string projColor = Colors::WhiteColor;

  bool isWand   = (_weaponRef->Data.ItemType_ == ItemType::WAND);
  bool isWeapon = (_weaponRef->Data.ItemType_ == ItemType::RANGED_WEAPON);

  if (throwingFromInventory)
  {
    projectile = _weaponRef->OwnerGameObject->Image;
  }
  else
  {
    if (isWand)
    {
      projectile = '*';

      SpellType spell = _weaponRef->Data.SpellHeld.SpellType_;
      SpellInfo* si = SpellsDatabase::Instance().GetSpellInfoFromDatabase(spell);
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

  res = { projectile, projColor };

  return res;
}

void TargetState::ProcessLaser()
{
  Position startPoint = { _playerRef->PosX, _playerRef->PosY };

  // Laser stops on cursor point regarless (is it OK?)
  Position endPoint = _cursorPosition;

  auto line = Util::BresenhamLine(startPoint, endPoint);

  int distanceCovered = 0;

  int power = _weaponRef->Data.SpellHeld.SpellBaseDamage.first + _weaponRef->Data.SpellHeld.SpellBaseDamage.second;

  int playerMagic = _playerRef->Attrs.Mag.Get();

  power += playerMagic;

  bool shouldStop = false;

  // Start from 1 to exclude player's position
  for (size_t i = 1; i < line.size(); i++)
  {
    if (power <= 0)
    {
      break;
    }

    int mx = line[i].X;
    int my = line[i].Y;

    for (auto& obj : _objectsOnTheLine)
    {
      if (obj->PosX == mx && obj->PosY == my)
      {
        // Laser stops when it hits indestructible object
        // or its power has dissipated.
        if (obj->Attrs.Indestructible || power <= 0)
        {
          shouldStop = true;
          break;
        }

        int def = obj->Attrs.Def.Get();
        int dmgDone = power - def;

        DoorComponent* dc = obj->GetComponent<DoorComponent>();
        if (dc != nullptr)
        {
          switch (dc->Material)
          {
            case DoorMaterials::WOOD:
              dmgDone = ((float)power * 0.7f);
              break;

            case DoorMaterials::STONE:
              dmgDone = ((float)power * 0.5f);
              break;

            case DoorMaterials::IRON:
              dmgDone = ((float)power * 0.3f);
              break;
          }
        }

        obj->ReceiveDamage(_weaponRef->OwnerGameObject, dmgDone, false);

        power -= (def == 0) ? 1 : dmgDone;
      }
    }

    if (shouldStop)
    {
      break;
    }

    int drawingPosX = mx + Map::Instance().CurrentLevel->MapOffsetX;
    int drawingPosY = my + Map::Instance().CurrentLevel->MapOffsetY;

    Printer::Instance().PrintFB(drawingPosX,
                                drawingPosY,
                                '*',
                                Colors::YellowColor,
                                Colors::RedColor);

    distanceCovered++;
    power--;
  }

  Printer::Instance().Render();
  Util::Sleep(100);
  Update(true);

  _weaponRef->Data.Amount--;
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

  int chance = CalculateHitChance();
  if (Util::Rolld100(chance) == false)
  {
    // If skill is low and we didn't roll a hit,
    // get random point around cursor position.
    // Doesn't affect wands.
    _cursorPosition = GetRandomPointAroundCursor();

    // We couldn't get a valid spot for targeting.
    // Near impossible case, unless we put player into a cell, surrounded
    // by walls and he tries to fire AoE wand.
    if (_cursorPosition.X == -1)
    {
      return;
    }
  }

  _lastCursorPosition = _cursorPosition;

  _drawHint = false;

  std::string weaponName = _weaponRef->Data.IsIdentified
                         ? _weaponRef->OwnerGameObject->ObjectName
                         : _weaponRef->Data.UnidentifiedName;

  std::string str = throwingFromInventory ? "You throw " : "You fire ";
  str += weaponName;

  Printer::Instance().AddMessage(str);

  GameObject* stoppedAt = nullptr;

  auto res = GetProjectileImageAndColor(throwingFromInventory);

  bool isWand = (_weaponRef->Data.ItemType_ == ItemType::WAND);

  if (isWand && _weaponRef->Data.SpellHeld.SpellType_ == SpellType::LASER)
  {
    ProcessLaser();
  }
  else
  {
    stoppedAt = LaunchProjectile(res.first, res.second);
    ProcessHit(stoppedAt);
  }

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

void TargetState::ProcessHitInventoryThrownItem(GameObject* hitPoint)
{
  // _weaponRef is an item to be thrown

  auto& mapRef = Map::Instance().CurrentLevel->MapArray;

  int x = hitPoint->PosX;
  int y = hitPoint->PosY;

  bool isRangedWeapon = (_weaponRef->Data.ItemType_ == ItemType::RANGED_WEAPON);
  bool isWand         = (_weaponRef->Data.ItemType_ == ItemType::WAND);

  bool isStackable = (!isWand && !isRangedWeapon && _weaponRef->Data.IsStackable);

  bool tileOk = (mapRef[x][y]->Type != GameObjectType::DEEP_WATER
              && mapRef[x][y]->Type != GameObjectType::LAVA
              && mapRef[x][y]->Type != GameObjectType::CHASM);

  // TODO: potions break on impact, items damage monsters (not?)
  //
  // Throwing one item at a time from the stack of items.

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

void TargetState::ProcessHit(GameObject* hitPoint)
{
  if (_throwingItemInventoryIndex != -1)
  {
    ProcessHitInventoryThrownItem(hitPoint);
  }
  else
  {
    bool isWand         = (_weaponRef->Data.ItemType_ == ItemType::WAND);
    bool isRangedWeapon = (_weaponRef->Data.ItemType_ == ItemType::RANGED_WEAPON);

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

  std::string objName = _weaponRef->Data.IsIdentified
                      ? _weaponRef->OwnerGameObject->ObjectName
                      : _weaponRef->Data.UnidentifiedName;
  std::string verb;
  std::string tileName = tileRef->ObjectName;

  if (tile == GameObjectType::DEEP_WATER)
  {
    verb = "drowns";
  }
  else if (tile == GameObjectType::LAVA)
  {
    verb = "melts";
  }
  else if (tile == GameObjectType::CHASM)
  {
    verb = "falls down";
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

  int hw = _twHalf;
  int hh = _thHalf;

  nx = Util::Clamp(nx, _playerRef->PosX - hw + 1,
                       _playerRef->PosX + hw - 2);

  ny = Util::Clamp(ny, _playerRef->PosY - hh + 1,
                       _playerRef->PosY + hh - 2);

  _cursorPosition.X = nx;
  _cursorPosition.Y = ny;
}

void TargetState::DrawHint()
{
  // NOTE: for wand of piercing and the like it is probably
  // not a good idea to draw hint line above all objects because
  // it will reveal the wand type in case it is not identified.
  // Same probably goes for projectile flying.
  // In any case, let's not edit this part of code right now.

  Position startPoint = { _playerRef->PosX, _playerRef->PosY };

  int mox = Map::Instance().CurrentLevel->MapOffsetX;
  int moy = Map::Instance().CurrentLevel->MapOffsetY;

  Position mapSize = Map::Instance().CurrentLevel->MapSize;

  std::vector<Position> cellsToHighlight;

  auto line = Util::BresenhamLine(startPoint, _cursorPosition);

  _objectsOnTheLine = FillObjectsOnTheLine(line);

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
      bool isThrowingOk = ((!isThrowing && d > _weaponRef->Data.Range)
                         || (isThrowing && d > _maxThrowingRange));

      if (actorPresent || isCellBlocking || isThrowingOk)
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
    Printer::Instance().PrintFB(p.X + mox, p.Y + moy, '.', Colors::RedColor);
  }
}

std::vector<GameObject*> TargetState::FillObjectsOnTheLine(const std::vector<Position>& line)
{
  std::vector<GameObject*> res;

  Position mapSize = Map::Instance().CurrentLevel->MapSize;

  Position startPoint = { _playerRef->PosX, _playerRef->PosY };

  for (auto& p : line)
  {
    if (p == startPoint)
    {
      continue;
    }

    if (Util::IsInsideMap(p, mapSize))
    {
      // Right now the functionality of this method is based
      // on assumption, that certain items, that use this functionality
      // (e.g. wand of piercing), always strike through the whole
      // targeting line, ignoring any items lying on the ground,
      // until their ray is blocked or their piercing power dissipates.

      auto actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
      if (actor != nullptr)
      {
        res.push_back(actor);
      }
      else
      {
        auto so = Map::Instance().GetStaticGameObjectAtPosition(p.X, p.Y);
        if (so != nullptr)
        {
          res.push_back(so);
        }
      }
    }
  }

  return res;
}

void TargetState::DrawCursor()
{
  int mox = Map::Instance().CurrentLevel->MapOffsetX;
  int moy = Map::Instance().CurrentLevel->MapOffsetY;

  Printer::Instance().PrintFB(_cursorPosition.X + mox + 1,
                              _cursorPosition.Y + moy,
                              ']', Colors::WhiteColor);

  Printer::Instance().PrintFB(_cursorPosition.X + mox - 1,
                              _cursorPosition.Y + moy,
                              '[', Colors::WhiteColor);
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

    int tw = Printer::TerminalWidth;

    Printer::Instance().PrintFB(tw / 2, 0,
                                "Select target then press 'f' or 'Enter' to fire",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(tw / 2, 1,
                                "(TAB to cycle through visible ones)",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

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

  //DebugLog("%s", str.data());

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

  /*
  DebugLog("%s\n%i + %i - %i = %i\n", __PRETTY_FUNCTION__,
                                     baseChance,
                                     (attackChanceScale * skl),
                                     (distanceChanceDrop * d),
                                     chance);
  */

  return chance;
}
