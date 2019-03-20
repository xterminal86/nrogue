#include "attack-state.h"
#include "printer.h"
#include "application.h"
#include "map.h"
#include "map-level-base.h"

void AttackState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void AttackState::Prepare()
{
  _cursorPosition.X = _playerRef->PosX;
  _cursorPosition.Y = _playerRef->PosY;

  Printer::Instance().AddMessage("Attack in which direction?");
}

void AttackState::HandleInput()
{
  _keyPressed = GetKeyDown();

  bool dirSet = false;

  switch (_keyPressed)
  {
    case NUMPAD_7:
      dirSet = true;
      _cursorPosition.X -= 1;
      _cursorPosition.Y -= 1;
      break;

    case NUMPAD_8:
      dirSet = true;
      _cursorPosition.Y -= 1;
      break;

    case NUMPAD_9:
      dirSet = true;
      _cursorPosition.X += 1;
      _cursorPosition.Y -= 1;
      break;

    case NUMPAD_4:
      dirSet = true;
      _cursorPosition.X -= 1;
      break;

    case NUMPAD_6:
      dirSet = true;
      _cursorPosition.X += 1;
      break;

    case NUMPAD_1:
      dirSet = true;
      _cursorPosition.X -= 1;
      _cursorPosition.Y += 1;
      break;

    case NUMPAD_2:
      dirSet = true;
      _cursorPosition.Y += 1;
      break;

    case NUMPAD_3:
      dirSet = true;
      _cursorPosition.X += 1;
      _cursorPosition.Y += 1;
      break;

    case 'q':
      Printer::Instance().AddMessage("Cancelled");
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }

  if (dirSet)
  {
    auto res = Map::Instance().GetActorAtPosition(_cursorPosition.X, _cursorPosition.Y);
    if (res != nullptr)
    {
      _playerRef->MeleeAttack(res);
    }
    else
    {
      if (CheckPickaxe())
      {
        ProcessPickaxe();
      }
      else
      {
        ProcessWeapon();
      }
    }

    Application::Instance().ChangeState(GameStates::MAIN_STATE);
  }
}

void AttackState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth - 1,
                                Printer::Instance().TerminalHeight - 1,
                                Printer::Instance().GetLastMessage(),
                                Printer::kAlignRight,
                                "#FFFFFF");

    Printer::Instance().Render();
  }
}

bool AttackState::CheckPickaxe()
{
  auto weapon = _playerRef->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  if (weapon != nullptr && weapon->Data.WeaponType_ == WeaponType::PICKAXE)
  {
    return true;
  }

  return false;
}

void AttackState::ProcessPickaxe()
{
  auto& so = Map::Instance().CurrentLevel->StaticMapObjects;

  int x = _cursorPosition.X;
  int y = _cursorPosition.Y;

  if (so[x][y] != nullptr)
  {
    Application::Instance().DisplayAttack(so[x][y].get(),
                                          GlobalConstants::DisplayAttackDelayMs,
                                          "You tear down the wall");
    so[x][y]->IsDestroyed = true;
  }
}

void AttackState::ProcessWeapon()
{
  auto res = Map::Instance().GetGameObjectsAtPosition(_cursorPosition.X, _cursorPosition.Y);
  if (res.size() != 0)
  {
    _playerRef->MeleeAttack(res.back());
  }
  else
  {
    auto* cell = Map::Instance().CurrentLevel->MapArray[_cursorPosition.X][_cursorPosition.Y].get();
    _playerRef->MeleeAttack(cell);
  }
}
