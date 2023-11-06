#include "replay-end-state.h"

#include "application.h"
#include "map.h"
#include "printer.h"

void ReplayEndState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MENU_STATE);
      Application::Instance().Reset();
      break;
  }
}

// =============================================================================

void ReplayEndState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Map::Instance().Draw();

    Application::Instance().PlayerInstance.Draw();

    Printer::Instance().PrintFB(_twHalf, 1,
                                "Replay has finished",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(_twHalf, 2,
                                "Press 'q' to exit",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);

    Printer::Instance().Render();
  }
}
