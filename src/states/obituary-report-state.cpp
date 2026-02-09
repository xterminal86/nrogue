#include "obituary-report-state.h"

#include "printer.h"
#include "application.h"

ObituaryReportState::ObituaryReportState()
{
  _obituary = std::make_unique<MsgBuffer>(Printer::TerminalHeight - 1, 5);
}

// =============================================================================

void ObituaryReportState::Prepare()
{
  StringV data = Game::gApp.CollectObituary(true, false);
  for (auto& line : data)
  {
    if (line == "\n")
    {
      continue;
    }

    _obituary->AddMessage(line);
  }

  _obituary->SetScrollState(MessageBufferScrollState::TOP);
}

// =============================================================================

void ObituaryReportState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
#ifdef USE_SDL
    case NUMPAD_8:
#else
    case KEY_PPAGE:
#endif
    {
      _obituary->ScrollUp();
    }
    break;

    // -------------------------------------------------------------------------

#ifdef USE_SDL
    case NUMPAD_5:
#else
    case KEY_NPAGE:
#endif
    {
      _obituary->ScrollDown();
    }
    break;

    // -------------------------------------------------------------------------

#ifdef USE_SDL
    case NUMPAD_2:
    {
      _obituary->ScrollDown();
    }
    break;
#endif

    // -------------------------------------------------------------------------

#ifdef USE_SDL
    case NUMPAD_7:
    {
      _obituary->SetScrollState(MessageBufferScrollState::TOP);
    }
    break;

    // -------------------------------------------------------------------------

    case NUMPAD_1:
    {
      _obituary->SetScrollState(MessageBufferScrollState::BOTTOM);
    }
    break;
#endif

    // -------------------------------------------------------------------------

    case VK_CANCEL:
    case VK_ENTER:
      Game::gApp.ChangeState(GameStates::EXIT_GAME);
      break;

    // -------------------------------------------------------------------------

    default:
      break;
  }
}

// =============================================================================

void ObituaryReportState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    DrawHeader(" OBITUARY ");
    Game::gPrnt.DrawScrollBars(*_obituary.get());

    auto msgs = _obituary->GetMessages();

    int lineCount = 0;
    for (const std::string* msg : msgs)
    {
      if (msg == nullptr)
      {
        break;
      }

      Game::gPrnt.PrintText(
        1,
        1 + lineCount,
        *msg,
        Printer::kAlignLeft,
        Colors::White,
        Colors::Black
      );

      lineCount++;
    }

    Game::gPrnt.Render();
  }
}
