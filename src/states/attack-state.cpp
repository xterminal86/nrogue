#include "attack-state.h"
#include "printer.h"
#include "application.h"
#include "map.h"

void AttackState::Init()
{
  _playerRef = &Game::gApp.PlayerInstance;
}

// =============================================================================

void AttackState::Prepare()
{
  _cursorPosition.X = _playerRef->PosX;
  _cursorPosition.Y = _playerRef->PosY;

  Game::gPrnt.AddMessage("Attack in which direction?");
}

// =============================================================================

void AttackState::HandleInput()
{
  _keyPressed = GetKeyDown();

  bool dirSet = false;

  switch (_keyPressed)
  {
    case ALT_K7:
    case NUMPAD_7:
      dirSet = true;
      _cursorPosition.X -= 1;
      _cursorPosition.Y -= 1;
      _dir.X = -1;
      _dir.Y = -1;
      break;

    case ALT_K8:
    case NUMPAD_8:
      dirSet = true;
      _cursorPosition.Y -= 1;
      _dir.X = 0;
      _dir.Y = -1;
      break;

    case ALT_K9:
    case NUMPAD_9:
      dirSet = true;
      _cursorPosition.X += 1;
      _cursorPosition.Y -= 1;
      _dir.X = 1;
      _dir.Y = -1;
      break;

    case ALT_K4:
    case NUMPAD_4:
      dirSet = true;
      _cursorPosition.X -= 1;
      _dir.X = -1;
      _dir.Y = 0;
      break;

    case ALT_K6:
    case NUMPAD_6:
      dirSet = true;
      _cursorPosition.X += 1;
      _dir.X = 1;
      _dir.Y = 0;
      break;

    case ALT_K1:
    case NUMPAD_1:
      dirSet = true;
      _cursorPosition.X -= 1;
      _cursorPosition.Y += 1;
      _dir.X = -1;
      _dir.Y = 1;
      break;

    case ALT_K2:
    case NUMPAD_2:
      dirSet = true;
      _cursorPosition.Y += 1;
      _dir.X = 0;
      _dir.Y = 1;
      break;

    case ALT_K3:
    case NUMPAD_3:
      dirSet = true;
      _cursorPosition.X += 1;
      _cursorPosition.Y += 1;
      _dir.X = 1;
      _dir.Y = 1;
      break;

    case VK_CANCEL:
      Game::gPrnt.AddMessage(Strings::MsgCancelled);
      Game::gApp.ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }

  if (dirSet)
  {
    _playerRef->SetAttackDir(_dir);

    //
    // Check actors first.
    //
    auto res = Game::gMap.GetActorAtPosition(_cursorPosition.X,
                                               _cursorPosition.Y);
    if (res != nullptr)
    {
      _playerRef->MeleeAttack(res);
    }
    else
    {
      //
      // Check static game objects.
      //
      auto so =
          Game::gMap.GetStaticGameObjectAtPosition(_cursorPosition.X,
                                                     _cursorPosition.Y);
      if (so != nullptr)
      {
        //
        // Always hit static objects in front of you.
        //
        _playerRef->MeleeAttack(so, true);
      }
      else
      {
        auto& ma = Game::gMap.CurrentLevel->MapArray;
        auto* cell = ma[_cursorPosition.X][_cursorPosition.Y].get();
        Game::gApp.DisplayAttack(
              cell,
              GlobalConstants::DisplayAttackDelayMs,
              "*whoosh*",
              Colors::White
        );
      }
    }

    _playerRef->FinishTurn();

    Game::gApp.ChangeState(GameStates::MAIN_STATE);
  }
}

// =============================================================================

void AttackState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    _playerRef->CheckVisibility();

    Game::gMap.Draw();

    _playerRef->Draw();

    GameLogMessageData* msg = Game::gPrnt.GetLastMessage();

    if (msg != nullptr)
    {
      Game::gPrnt.PrintText(
        Printer::TerminalWidth - 1,
        Printer::TerminalHeight - 1,
        (*msg).Message,
        Printer::kAlignRight,
        (*msg).FgColor,
        (*msg).BgColor
      );
    }

    Game::gPrnt.Render();
  }
}
