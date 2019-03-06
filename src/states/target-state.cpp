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
      if (Util::IsInsideMap({ x, y }, Map::Instance().CurrentLevel->MapSize)
       && Map::Instance().CurrentLevel->MapArray[x][y]->Visible)
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

void TargetState::FireWeapon()
{
  Position startPoint = { _playerRef->PosX, _playerRef->PosY };
  Position endPoint = _cursorPosition;

  auto line = Util::BresenhamLine(startPoint, endPoint);

  char projectile = (_weaponRef->Data.ItemType_ == ItemType::WAND) ? '*' : '+';

  // Don't include player's position
  for (int i = 1; i < line.size(); i++)
  {
    Update(true);

    int mx = line[i].X;
    int my = line[i].Y;

    auto actor = Map::Instance().GetActorAtPosition(mx, my);
    if (actor != nullptr)
    {
      ProcessHit(actor);
      break;
    }

    auto cell = Map::Instance().CurrentLevel->MapArray[mx][my].get();
    if (cell->Blocking)
    {
      ProcessHit(cell);
      break;
    }

    int drawingPosX = mx + Map::Instance().CurrentLevel->MapOffsetX;
    int drawingPosY = my + Map::Instance().CurrentLevel->MapOffsetY;

    Printer::Instance().PrintFB(drawingPosX, drawingPosY, projectile, "#FFFF00");
    Printer::Instance().Render();
  }

  _playerRef->FinishTurn();
  Map::Instance().UpdateGameObjects();
  Application::Instance().ChangeState(GameStates::MAIN_STATE);
}

void TargetState::ProcessHit(GameObject* objHit)
{
  AIComponent* enemy = objHit->GetComponent<AIComponent>();
  if (enemy != nullptr)
  {
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
