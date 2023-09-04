#include "attack-state.h"
#include "printer.h"
#include "application.h"
#include "map.h"

void AttackState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;

  RECORD_NUMPAD();

  _keysToRecord[ALT_K5]   = false;
  _keysToRecord[NUMPAD_5] = false;

  _keysToRecord[VK_CANCEL] = true;
}

// =============================================================================

void AttackState::Prepare()
{
  _cursorPosition.X = _playerRef->PosX;
  _cursorPosition.Y = _playerRef->PosY;

  Printer::Instance().AddMessage("Attack in which direction?");
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
      Printer::Instance().AddMessage(Strings::MsgCancelled);
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
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
    auto res = Map::Instance().GetActorAtPosition(_cursorPosition.X, _cursorPosition.Y);
    if (res != nullptr)
    {
      _playerRef->MeleeAttack(res);
    }
    else
    {
      //
      // Check static game objects.
      //
      auto so = Map::Instance().GetStaticGameObjectAtPosition(_cursorPosition.X, _cursorPosition.Y);
      if (so != nullptr)
      {
        //
        // Always hit static objects in front of you.
        //
        _playerRef->MeleeAttack(so, true);
      }
      else
      {
        auto* cell = Map::Instance().CurrentLevel->MapArray[_cursorPosition.X][_cursorPosition.Y].get();
        Application::Instance().DisplayAttack(cell,
                                              GlobalConstants::DisplayAttackDelayMs,
                                              "*whoosh*",
                                              Colors::WhiteColor);
      }
    }

    _playerRef->FinishTurn();

    Application::Instance().ChangeState(GameStates::MAIN_STATE);
  }

  RECORD_ACTION(_keyPressed);
}

// =============================================================================

void AttackState::Update(bool forceUpdate)
{
  DONT_SHOW_REPLAY();

  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw();

    _playerRef->Draw();

    auto lastMsg = Printer::Instance().GetLastMessage();

    Printer::Instance().PrintFB(Printer::TerminalWidth - 1,
                                Printer::TerminalHeight - 1,
                                lastMsg.Message,
                                Printer::kAlignRight,
                                lastMsg.FgColor,
                                lastMsg.BgColor);

    Printer::Instance().Render();
  }
}
