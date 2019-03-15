#include "target-state.h"

#include "application.h"
#include "printer.h"
#include "map.h"
#include "ai-component.h"

void TargetState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void TargetState::Prepare()
{
  _drawHint = true;

  _cursorPosition.X = _playerRef->PosX;
  _cursorPosition.Y = _playerRef->PosY;

  FindTargets();

  if (!_targets.empty())
  {
    int x = _targets[0]->PosX;
    int y = _targets[0]->PosY;

    _cursorPosition.Set(x, y);

    _lastTargetIndex = 0;
  }
}

void TargetState::FindTargets()
{
  _targets.clear();

  int r = _playerRef->VisibilityRadius;
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
      if (x == px && y == py )
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
    case NUMPAD_7:
      MoveCursor(-1, -1);
      break;

    case NUMPAD_8:
      MoveCursor(0, -1);
      break;

    case NUMPAD_9:
      MoveCursor(1, -1);
      break;

    case NUMPAD_4:
      MoveCursor(-1, 0);
      break;

    case NUMPAD_6:
      MoveCursor(1, 0);
      break;

    case NUMPAD_1:
      MoveCursor(-1, 1);
      break;

    case NUMPAD_2:
      MoveCursor(0, 1);
      break;

    case NUMPAD_3:
      MoveCursor(1, 1);
      break;

    case VK_TAB:
      CycleTargets();
      break;

    case 'f':
      FireWeapon();
      break;

    case 'q':
      Printer::Instance().AddMessage("Cancelled");
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;
  }
}

GameObject* TargetState::LaunchProjectile(char image)
{
  GameObject* stoppedAt = nullptr;

  Position startPoint = { _playerRef->PosX, _playerRef->PosY };
  Position endPoint = _cursorPosition;

  auto line = Util::BresenhamLine(startPoint, endPoint);

  int maxDistance = 0;

  // Don't include player's position
  for (int i = 1; i < line.size(); i++)
  {
    Update(true);

    int mx = line[i].X;
    int my = line[i].Y;

    if (maxDistance >= _weaponRef->Data.Range)
    {
      stoppedAt = CheckHit({ mx, my }, { line[i - 1].X, line[i - 1].Y });
      break;
    }

    stoppedAt = CheckHit({ mx, my }, { line[i - 1].X, line[i - 1].Y });
    if (stoppedAt != nullptr)
    {
      break;
    }

    int drawingPosX = mx + Map::Instance().CurrentLevel->MapOffsetX;
    int drawingPosY = my + Map::Instance().CurrentLevel->MapOffsetY;

    Printer::Instance().PrintFB(drawingPosX, drawingPosY, image, "#FFFF00");
    Printer::Instance().Render();

    #ifndef USE_SDL
    Util::Sleep(10);
    #endif

    maxDistance++;
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

  auto cell = Map::Instance().CurrentLevel->MapArray[at.X][at.Y].get();
  if (cell->Blocking)
  {
    auto prevCell = Map::Instance().CurrentLevel->MapArray[prev.X][prev.Y].get();
    return prevCell;
  }

  return nullptr;
}

void TargetState::FireWeapon()
{  
  if (_cursorPosition.X == _playerRef->PosX
   && _cursorPosition.Y == _playerRef->PosY)
  {    
    return;
  }

  _drawHint = false;

  // TODO: randomize end point with regard to SKL / hit chance?

  auto str = "You fire " + _weaponRef->OwnerGameObject->ObjectName;
  Printer::Instance().AddMessage(str);

  int chance = CalculateHitChance();
  if (!Util::Rolld100(chance))
  {
    Printer::Instance().AddMessage("The shot goes astray due to lack of skill");

    auto rect = Util::GetEightPointsAround(_cursorPosition, Map::Instance().CurrentLevel->MapSize);
    int index = RNG::Instance().RandomRange(0, rect.size());
    _cursorPosition = rect[index];
  }

  GameObject* stoppedAt = nullptr;

  bool isWand = (_weaponRef->Data.ItemType_ == ItemType::WAND);
  bool isWeapon = (_weaponRef->Data.ItemType_ == ItemType::RANGED_WEAPON);

  char projectile = ' ';
  if (isWand)
  {
    projectile = '*';
  }
  else if (isWeapon)
  {
    projectile = '+';
  }

  stoppedAt = LaunchProjectile(projectile);
  ProcessHit(stoppedAt);

  _playerRef->FinishTurn();
  Map::Instance().UpdateGameObjects();
  Application::Instance().ChangeState(GameStates::MAIN_STATE);
}

void TargetState::ProcessHit(GameObject *hitPoint)
{
  bool isWand = (_weaponRef->Data.ItemType_ == ItemType::WAND);
  bool isWeapon = (_weaponRef->Data.ItemType_ == ItemType::RANGED_WEAPON);

  if (isWand)
  {    
    _playerRef->MagicAttack(hitPoint, _weaponRef);
  }
  else if (isWeapon)
  {
    _playerRef->RangedAttack(hitPoint, _weaponRef);
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

      if (actor != nullptr
       || mapRef[p.X][p.Y]->Blocking
       || !mapRef[p.X][p.Y]->Visible)
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

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    if (_drawHint)
    {
      DrawHint();
      DrawCursor();
    }

    int tw = Printer::Instance().TerminalWidth;

    Printer::Instance().PrintFB(tw / 2, 0,
                                "Select target (TAB to cycle through visible ones)",
                                Printer::kAlignCenter,
                                "#FFFFFF");

    Printer::Instance().Render();
  }
}

void TargetState::Setup(ItemComponent* weapon)
{
  _weaponRef = weapon;
}

int TargetState::CalculateHitChance()
{
  int chance = 0;

  Position startPoint = { _playerRef->PosX, _playerRef->PosY };
  Position endPoint = _cursorPosition;

  int baseChance = 50;
  int attackChanceScale = 5;

  chance = baseChance;

  int skl = _playerRef->Attrs.Skl.Get();
  if (skl > 0)
  {
    chance += (attackChanceScale * skl);
  }
  else
  {
    chance -= (attackChanceScale * skl);
  }

  int d = (int)Util::LinearDistance(startPoint, endPoint);

  for (int i = 0; i < d; i++)
  {
    chance -= attackChanceScale;
  }

  chance = Util::Clamp(chance, GlobalConstants::MinHitChance, GlobalConstants::MaxHitChance);

  return chance;
}
