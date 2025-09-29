#include "message-log-state.h"

#include "application.h"
#include "printer.h"
#include "util.h"

void MessageLogState::Prepare()
{
  Printer::Instance().GetMsgBufferObj().ResetScroll();
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
      Printer::Instance().GetMsgBufferObj().ScrollDown();
    }
    break;

    case ALT_K8:
    case NUMPAD_8:
    {
      Printer::Instance().GetMsgBufferObj().ScrollUp();
    }
    break;

    case 'm':
    case 'M':
    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
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
    Printer::Instance().Clear();

    DrawHeader(_windowHeader);
    DrawScrollBars();

    int offsetY = 1;

    GameLogMessageData* lm = nullptr;

    auto& msb = Printer::Instance().GetMsgBufferObj();

    auto msgs = Printer::Instance().Messages();
    for (GameLogMessageData* m : msgs)
    {
      if (m == nullptr)
      {
        break;
      }

      Printer::Instance().PrintFB(1,
                                  offsetY,
                                  m->Message,
                                  Printer::kAlignLeft,
                                  m->FgColor,
                                  m->BgColor);
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
        Printer::Instance().PrintFB(1,
                                    offsetY - 1,
                                    "=> " + lm->Message,
                                    Printer::kAlignLeft,
                                    lm->FgColor,
                                    lm->BgColor);
      }
    }

    Printer::Instance().Render();
  }
}

// =============================================================================

void MessageLogState::DrawScrollBars()
{
  auto DrawArrow = [](int x, int y, int arrowChar)
  {
    #ifdef USE_SDL
    Printer::Instance().PrintFB(x,
                                y,
                                arrowChar,
                                Colors::WhiteColor,
                                Colors::BlackColor);
    #else
    Printer::Instance().PrintFB(x,
                                y,
                                arrowChar,
                                Colors::WhiteColor,
                                Colors::BlackColor);
    #endif
  };

  auto s = Printer::Instance().GetMsgBufferObj().GetScrollState();
  switch (s)
  {
    case MessageBufferScrollState::NONE:
      break;

    default:
    {
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
    double progress = Printer::Instance().GetMsgBufferObj().GetScrollProgress();
    Printer::Instance().PrintFB(_tw - 1,
                                _th - 2 - (int)(21.0 * progress),
                                '*',
                                Colors::WhiteColor,
                                Colors::BlackColor);
  }
}
