#include "npc-interact-state.h"

#include "application.h"
#include "constants.h"
#include "printer.h"
#include "util.h"
#include "ai-npc.h"

void NPCInteractState::Cleanup()
{
  _npcRef = nullptr;
}

void NPCInteractState::HandleInput()
{
  _keyPressed = getch();

  switch (_keyPressed)
  {
    case 'q':
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;
  }
}

void NPCInteractState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    Printer::Instance().PrintFB(tw / 2, 0, _npcRef->Data().UnacquaintedDescription, Printer::kAlignCenter, "#FFFFFF");

    Printer::Instance().Render();
  }
}

void NPCInteractState::SetNPCRef(AINPC* npcRef)
{
  _npcRef = npcRef;
}
