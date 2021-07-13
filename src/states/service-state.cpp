#include "service-state.h"
#include "printer.h"
#include "application.h"
#include "npc-interact-state.h"
#include "trader-component.h"
#include "ai-npc.h"

void ServiceState::Prepare()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void ServiceState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_CANCEL:
    {
      auto res = Application::Instance().GetGameStateRefByName(GameStates::NPC_INTERACT_STATE);
      NPCInteractState* nis = static_cast<NPCInteractState*>(res);
      nis->SetNPCRef(_shopOwner->NpcRef);
      Application::Instance().ChangeState(GameStates::NPC_INTERACT_STATE);
    }
    break;
  }
}

void ServiceState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawHeader(_serviceNameByType.at(_shopOwner->NpcRef->Data.ProvidesService));

    Printer::Instance().PrintFB(_tw / 2, _th - 1, "'q' - exit ", Printer::kAlignCenter, GlobalConstants::WhiteColor);

    Printer::Instance().Render();
  }
}

void ServiceState::Setup(TraderComponent* shopOwner)
{
  _shopOwner = shopOwner;
}
