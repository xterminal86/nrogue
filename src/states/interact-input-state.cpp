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
  _playerRef = &Application::Instance().PlayerInstance;
}

void InteractInputState::Prepare()
{
  _cursorPosition.X = _playerRef->PosX;
  _cursorPosition.Y = _playerRef->PosY;

  Printer::Instance().AddMessage("Interact in which direction?");
}

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
      Printer::Instance().AddMessage("Cancelled");
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }

  if (dirSet)
  {
    ProcessInteraction();
  }
}

bool InteractInputState::SetDir(const Position& dir)
{
  _cursorPosition.X += dir.X;
  _cursorPosition.Y += dir.Y;

  return true;
}

void InteractInputState::ProcessInteraction()
{
  auto actor = Map::Instance().GetActorAtPosition(_cursorPosition.X, _cursorPosition.Y);
  if (actor != nullptr)
  {
    TryToInteractWithActor(actor);
  }
  else
  {
    auto res = Map::Instance().GetGameObjectsAtPosition(_cursorPosition.X, _cursorPosition.Y);
    if (res.size() != 0)
    {
      TryToInteractWithObject(res.back());
    }
    else
    {
      auto staticObject = Map::Instance().GetStaticGameObjectAtPosition(_cursorPosition.X, _cursorPosition.Y);
      if (staticObject != nullptr)
      {
        TryToInteractWithObject(staticObject);
      }
      else
      {
        Printer::Instance().AddMessage("There's nothing there");
        Application::Instance().ChangeState(GameStates::MAIN_STATE);
      }
    }
  }
}

void InteractInputState::TryToInteractWithObject(GameObject* go)
{
  ContainerComponent* cc = go->GetComponent<ContainerComponent>();
  if (cc != nullptr)
  {
    // TODO: locked containers?
    if (!cc->Interact())
    {
      Printer::Instance().AddMessage(go->ObjectName + " can't be opened");
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
    }
    else
    {
      Printer::Instance().AddMessage("You opened: " + go->ObjectName);
    }
  }
  else
  {
    if (go->Interact())
    {
      DoorComponent* dc = go->GetComponent<DoorComponent>();
      if (dc != nullptr)
      {
        if (dc->OpenedBy == GlobalConstants::OpenedByAnyone)
        {
          auto str = Util::StringFormat("You %s: %s", (dc->IsOpen ? "opened" : "closed"), go->ObjectName.data());
          Printer::Instance().AddMessage(str);
        }
      }

      _playerRef->FinishTurn();
      Map::Instance().UpdateGameObjects();
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
    }
    else
    {
      Printer::Instance().AddMessage("Can't interact with: " + go->ObjectName);
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
    }
  }
}

void InteractInputState::TryToInteractWithActor(GameObject* actor)
{
  AIComponent* aic = actor->GetComponent<AIComponent>();
  if (aic->CurrentModel->IsAgressive)
  {
    auto str = Util::StringFormat("%s is attacking!", actor->ObjectName.data());
    Printer::Instance().AddMessage(str);
    Application::Instance().ChangeState(GameStates::MAIN_STATE);
  }
  else
  {
    auto state = Application::Instance().GetGameStateRefByName(GameStates::NPC_INTERACT_STATE);
    NPCInteractState* nis = static_cast<NPCInteractState*>(state);
    AINPC* npcAi = aic->GetModel<AINPC>();
    if (npcAi != nullptr)
    {
      nis->SetNPCRef(npcAi);
      Application::Instance().ChangeState(GameStates::NPC_INTERACT_STATE);
    }
  }
}

void InteractInputState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw();

    _playerRef->Draw();

    auto lastMessage = Printer::Instance().GetLastMessage();

    Printer::Instance().PrintFB(Printer::TerminalWidth - 1,
                                Printer::TerminalHeight - 1,
                                lastMessage.Message,
                                Printer::kAlignRight,
                                lastMessage.FgColor,
                                lastMessage.BgColor);

    Printer::Instance().Render();
  }
}
