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
    case ALT_K2:
    case NUMPAD_2:
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
    DrawScrollBars();

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

      Game::gPrnt.PrintFB(1,
                           offsetY,
                           m->Message,
                           Printer::kAlignLeft,
                           Colors::ShadesOfGrey::Six,
                           Colors::BlackColor);
      lm = m;
      offsetY++;
    }

    //
    // Mark last added message in the log for clarity.
    //
    MessageBufferScrollState ss = msb.GetScrollState();

    if (ss == MessageBufferScrollState::BOTTOM
     || ss == MessageBufferScrollState::NONE)
    {
      if (lm != nullptr)
      {
        Game::gPrnt.PrintFB(1,
                             offsetY - 1,
                             lm->Message,
                             Printer::kAlignLeft,
                             Colors::WhiteColor,
                             Colors::BlackColor);
      }
    }

    Game::gPrnt.Render();
  }
}

// =============================================================================

void MessageLogState::DrawScrollBars()
{
  auto DrawArrow = [](int x, int y, int arrowChar)
  {
    #ifdef USE_SDL
    Game::gPrnt.PrintFB(x,
                         y,
                         arrowChar,
                         Colors::WhiteColor,
                         Colors::BlackColor);
    #else
    Game::gPrnt.PrintFB(x,
                        y,
                        arrowChar,
                        Colors::WhiteColor,
                        Colors::BlackColor);
    #endif
  };

  auto s = Game::gPrnt.GetMsgBufferObj().GetScrollState();
  switch (s)
  {
    case MessageBufferScrollState::NONE:
      break;

    default:
    {
      for (int y = 2; y < _th - 1; y++)
      {
        Game::gPrnt.PrintFB(_tw - 1,
                             y,
                             '|',
                             Colors::ShadesOfGrey::Six,
                             Colors::BlackColor);
      }

      #ifdef USE_SDL
      int arrowDown = (s == MessageBufferScrollState::BOTTOM)
                      ? 'x'
                      : (int)NameCP437::DARROW_2;
      int arrowUp   = (s == MessageBufferScrollState::TOP)
                      ? 'x'
                      : (int)NameCP437::UARROW_2;
      #else
      int arrowDown = (s == MessageBufferScrollState::BOTTOM)
                      ? 'x'
                      : ACS_DARROW;
      int arrowUp   = (s == MessageBufferScrollState::TOP)
                      ? 'x'
                      : ACS_UARROW;
      #endif
      DrawArrow(_tw - 1, _th - 1, arrowDown);
      DrawArrow(_tw - 1, 1, arrowUp);
    }
    break;
  }

  //
  // Draw scroll progress.
  //
  if (s != MessageBufferScrollState::NONE)
  {
    double progress = Game::gPrnt.GetMsgBufferObj().GetScrollProgress();
    Game::gPrnt.PrintFB(_tw - 1,
                         _th - 2 - (int)(21.0 * progress),
                         '=',
                         Colors::WhiteColor,
                         Colors::BlackColor);
  }
}
