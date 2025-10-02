#include "interact-input-state.h"

#include "application.h"
#include "map.h"
#include "printer.h"
#include "ai-component.h"
#include "ai-npc.h"
#include "npc-interact-state.h"
#include "door-component.h"

void InteractInputState::Init()
{
  _playerRef = &Game::gApp.PlayerInstance;
}

// =============================================================================

void InteractInputState::Prepare()
{
  _cursorPosition.X = _playerRef->PosX;
  _cursorPosition.Y = _playerRef->PosY;

  Game::gPrnt.AddMessage(Strings::MsgInteractDir);
}

// =============================================================================

void InteractInputState::HandleInput()
{
  _keyPressed = GetKeyDown();

  bool dirSet = false;

  switch (_keyPressed)
  {
    case ALT_K7:
    case NUMPAD_7:
      dirSet = SetDir({ -1, -1 });
      break;

    case ALT_K8:
    case NUMPAD_8:
      dirSet = SetDir({ 0, -1 });
      break;

    case ALT_K9:
    case NUMPAD_9:
      dirSet = SetDir({ 1, -1 });
      break;

    case ALT_K4:
    case NUMPAD_4:
      dirSet = SetDir({ -1, 0 });
      break;

    case ALT_K6:
    case NUMPAD_6:
      dirSet = SetDir({ 1, 0 });
      break;

    case ALT_K1:
    case NUMPAD_1:
      dirSet = SetDir({ -1, 1 });
      break;

    case ALT_K2:
    case NUMPAD_2:
      dirSet = SetDir({ 0, 1 });
      break;

    case ALT_K3:
    case NUMPAD_3:
      dirSet = SetDir({ 1, 1 });
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
    ProcessInteraction();
  }
}

// =============================================================================

bool InteractInputState::SetDir(const Position& dir)
{
  _cursorPosition.X += dir.X;
  _cursorPosition.Y += dir.Y;

  return true;
}

// =============================================================================

void InteractInputState::ProcessInteraction()
{
  auto actor = Game::gMap.GetActorAtPosition(_cursorPosition.X,
                                               _cursorPosition.Y);
  if (actor != nullptr)
  {
    TryToInteractWithActor(actor);
  }
  else
  {
    auto res = Game::gMap.GetGameObjectsAtPosition(_cursorPosition.X,
                                                     _cursorPosition.Y);
    if (res.size() != 0)
    {
      TryToInteractWithObject(res.back());
    }
    else
    {
      auto staticObject =
          Game::gMap.GetStaticGameObjectAtPosition(_cursorPosition.X,
                                                     _cursorPosition.Y);

      if (staticObject != nullptr)
      {
        TryToInteractWithObject(staticObject);
      }
      else
      {
        Game::gPrnt.AddMessage(Strings::MsgNothingHere);
        Game::gApp.ChangeState(GameStates::MAIN_STATE);
      }
    }
  }
}

// =============================================================================

void InteractInputState::TryToInteractWithObject(GameObject* go)
{
  IR ir = go->Interact();
  if (ir.first == InteractionResult::UNDEFINED)
  {
    Game::gPrnt.AddMessage("Can't interact with: " + go->ObjectName);
  }

  if (ir.first == InteractionResult::SUCCESS)
  {
    _playerRef->FinishTurn();
  }

  GameStates changeTo = (ir.second == GameStates::UNDEIFNED) ?
                         GameStates::MAIN_STATE :
                         ir.second;

  Game::gApp.ChangeState(changeTo);
}

// =============================================================================

void InteractInputState::TryToInteractWithActor(GameObject* actor)
{
  AIComponent* aic = actor->GetComponent<AIComponent>();
  if (aic->CurrentModel->IsAgressive)
  {
    auto str = Util::StringFormat("%s is attacking!", actor->ObjectName.data());
    Game::gPrnt.AddMessage(str);
    Game::gApp.ChangeState(GameStates::MAIN_STATE);
  }
  else
  {
    GameStates s = GameStates::NPC_INTERACT_STATE;
    auto state = Game::gApp.GetGameStateRefByName(s);
    NPCInteractState* nis = static_cast<NPCInteractState*>(state);
    AINPC* npcAi = aic->GetModel<AINPC>();
    if (npcAi != nullptr)
    {
      if (npcAi->Data.CanSpeak)
      {
        nis->SetNPCRef(npcAi);
        Game::gApp.ChangeState(GameStates::NPC_INTERACT_STATE);
      }
      else
      {
        auto str = Util::StringFormat("%s is not responding",
                                      actor->ObjectName.data());
        Game::gPrnt.AddMessage(str);
        Game::gApp.ChangeState(GameStates::MAIN_STATE);
      }
    }
  }
}

// =============================================================================

void InteractInputState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    _playerRef->CheckVisibility();

    Game::gMap.Draw();

    _playerRef->Draw();

    GameLogMessageData* lastMessage = Game::gPrnt.GetLastMessage();
    if (lastMessage != nullptr)
    {
      Game::gPrnt.PrintFB(Printer::TerminalWidth - 1,
                          Printer::TerminalHeight - 1,
                          lastMessage->Message,
                          Printer::kAlignRight,
                          lastMessage->FgColor,
                          lastMessage->BgColor);
    }

    Game::gPrnt.Render();
  }
}
