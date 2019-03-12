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

  Printer::Instance().AddMessage("Select target (TAB to cycle through visible ones)");
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

    case VK_ENTER:
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
  GameObject* stoppedAt = nullptr;

  bool isWand = (_weaponRef->Data.ItemType_ == ItemType::WAND);
  bool isWeapon = (_weaponRef->Data.ItemType_ == ItemType::WEAPON
                && _weaponRef->Data.Range > 1);

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
  bool isWeapon = (_weaponRef->Data.ItemType_ == ItemType::WEAPON
                && _weaponRef->Data.Range > 1);

  if (isWand)
  {
    switch (_weaponRef->Data.SpellHeld)
    {
      case SpellType::FIREBALL:
        DrawExplosion({ hitPoint->PosX, hitPoint->PosY });
        break;
    }
  }
  else if (isWeapon)
  {
    // TODO:
  }
}

void TargetState::DrawExplosion(Position pos)
{
  for (int range = 1; range <= 3; range++)
  {
    auto res = GetVisiblePointsFrom(pos, range);
    for (auto& p : res)
    {
      int drawX = p.X + Map::Instance().CurrentLevel->MapOffsetX;
      int drawY = p.Y + Map::Instance().CurrentLevel->MapOffsetY;

      if (Map::Instance().CurrentLevel->MapArray[p.X][p.Y]->Visible)
      {
        Printer::Instance().PrintFB(drawX, drawY, 'x', "#FF0000");
      }
    }

    Printer::Instance().Render();

    #ifndef USE_SDL
    Util::Sleep(10);
    #endif

    Update(true);
  }
}

std::vector<Position> TargetState::GetVisiblePointsFrom(Position from, int range)
{
  std::vector<Position> res;

  int lx = from.X - range;
  int ly = from.Y - range;
  int hx = from.X + range;
  int hy = from.Y + range;

  std::vector<Position> perimeterPoints;

  for (int x = lx; x <= hx; x++)
  {
    Position p1 = { x, ly };
    Position p2 = { x, hy };

    perimeterPoints.push_back(p1);
    perimeterPoints.push_back(p2);
  }

  for (int y = ly + 1; y <= hy - 1; y++)
  {
    Position p1 = { lx, y };
    Position p2 = { hx, y };

    perimeterPoints.push_back(p1);
    perimeterPoints.push_back(p2);
  }

  for (auto& p : perimeterPoints)
  {
    auto line = Util::BresenhamLine(from, p);
    for (auto& point : line)
    {
      if (!Util::IsInsideMap(point, Map::Instance().CurrentLevel->MapSize))
      {
        continue;
      }

      auto cell = Map::Instance().CurrentLevel->MapArray[point.X][point.Y].get();
      if (!cell->Blocking)
      {
        res.push_back({ cell->PosX, cell->PosY });
      }
      else
      {
        break;
      }
    }
  }

  return res;
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

void TargetState::DrawCursor()
{
  Printer::Instance().PrintFB(_cursorPosition.X + Map::Instance().CurrentLevel->MapOffsetX + 1,
                              _cursorPosition.Y + Map::Instance().CurrentLevel->MapOffsetY,
                              ']', "#FFFFFF");

  Printer::Instance().PrintFB(_cursorPosition.X + Map::Instance().CurrentLevel->MapOffsetX - 1,
                              _cursorPosition.Y + Map::Instance().CurrentLevel->MapOffsetY,
                              '[', "#FFFFFF");
}

void TargetState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    DrawCursor();

    auto& msgs = Printer::Instance().Messages();
    auto msg1 = msgs.at(0);

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth - 1,
                                Printer::Instance().TerminalHeight - 1,
                                msg1,
                                Printer::kAlignRight,
                                "#FFFFFF");

    Printer::Instance().Render();
  }
}

void TargetState::Setup(ItemComponent* weapon)
{
  _weaponRef = weapon;
}
