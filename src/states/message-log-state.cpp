#include "message-log-state.h"

#include "printer.h"
#include "application.h"

void MessageLogState::Prepare()
{
  Game::gPrnt.GetMsgBufferObj().ResetScroll();
}

// =============================================================================

void MessageLogState::Cleanup()
{
  Game::gPrnt.GetMsgBufferObj().ResetScroll();
}

// =============================================================================

void MessageLogState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case NUMPAD_7:
    {
      Game::gPrnt.GetMsgBufferObj().SetScrollState(
        MessageBufferScrollState::TOP
      );
    }
    break;

    case NUMPAD_1:
    {
      Game::gPrnt.GetMsgBufferObj().SetScrollState(
        MessageBufferScrollState::BOTTOM
      );
    }
    break;

    case ALT_K2:
    case NUMPAD_2:
    case NUMPAD_5:
    {
      Game::gPrnt.GetMsgBufferObj().ScrollDown();
    }
    break;

    case ALT_K8:
    case NUMPAD_8:
    {
      Game::gPrnt.GetMsgBufferObj().ScrollUp();
    }
    break;

    case NUMPAD_4:
    {
      Game::gPrnt.GetMsgBufferObj().PageUp();
    }
    break;

    case NUMPAD_6:
    {
      Game::gPrnt.GetMsgBufferObj().PageDown();
    }
    break;

    case 'm':
    case 'M':
    case VK_CANCEL:
      Game::gApp.ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }
}

// =============================================================================

void MessageLogState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    DrawHeader(_windowHeader);
    Game::gPrnt.DrawScrollBars(Game::gPrnt.GetMsgBufferObj());

    int offsetY = 1;

    GameLogMessageData* lm = nullptr;

    auto& msb = Game::gPrnt.GetMsgBufferObj();

    auto msgs = Game::gPrnt.Messages();
    for (GameLogMessageData* m : msgs)
    {
      if (m == nullptr)
      {
        break;
      }

      Game::gPrnt.PrintText(
        1,
        offsetY,
        m->Message,
        Printer::kAlignLeft,
        Colors::ShadesOfGrey::Six,
        Colors::BlackColor
      );

      lm = m;
      offsetY++;
    }

    //
    // Color last added message in the log white for clarity.
    //
    MessageBufferScrollState ss = msb.GetScrollState();

    if (ss == MessageBufferScrollState::BOTTOM
     || ss == MessageBufferScrollState::NONE)
    {
      if (lm != nullptr)
      {
        Game::gPrnt.PrintText(
          1,
          offsetY - 1,
          lm->Message,
          Printer::kAlignLeft,
          Colors::WhiteColor,
          Colors::BlackColor
        );
      }
    }

    Game::gPrnt.Render();
  }
}
