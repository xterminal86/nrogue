#include "application.h"

#include "mainstate.h"
#include "infostate.h"
#include "menustate.h"
#include "select-class-state.h"
#include "enter-name-state.h"
#include "intro-state.h"
#include "inventory-state.h"
#include "container-interact-state.h"
#include "message-log-state.h"
#include "look-input-state.h"
#include "interact-input-state.h"
#include "npc-interact-state.h"
#include "shopping-state.h"
#include "returner-state.h"
#include "exiting-state.h"
#include "message-box-state.h"
#include "endgame-state.h"
#include "attack-state.h"
#include "map.h"
#include "map-level-base.h"

void Application::Init()
{
  _gameStates[GameStates::MAIN_STATE]               = std::unique_ptr<GameState>(new MainState());
  _gameStates[GameStates::INFO_STATE]               = std::unique_ptr<GameState>(new InfoState());
  _gameStates[GameStates::ATTACK_STATE]             = std::unique_ptr<GameState>(new AttackState());
  _gameStates[GameStates::MENU_STATE]               = std::unique_ptr<GameState>(new MenuState());
  _gameStates[GameStates::SELECT_CLASS_STATE]       = std::unique_ptr<GameState>(new SelectClassState());
  _gameStates[GameStates::ENTER_NAME_STATE]         = std::unique_ptr<GameState>(new EnterNameState());
  _gameStates[GameStates::INTRO_STATE]              = std::unique_ptr<GameState>(new IntroState());
  _gameStates[GameStates::INVENTORY_STATE]          = std::unique_ptr<GameState>(new InventoryState());
  _gameStates[GameStates::CONTAINER_INTERACT_STATE] = std::unique_ptr<GameState>(new ContainerInteractState());
  _gameStates[GameStates::SHOW_MESSAGES_STATE]      = std::unique_ptr<GameState>(new MessageLogState());
  _gameStates[GameStates::LOOK_INPUT_STATE]         = std::unique_ptr<GameState>(new LookInputState());
  _gameStates[GameStates::INTERACT_INPUT_STATE]     = std::unique_ptr<GameState>(new InteractInputState());
  _gameStates[GameStates::NPC_INTERACT_STATE]       = std::unique_ptr<GameState>(new NPCInteractState());
  _gameStates[GameStates::SHOPPING_STATE]           = std::unique_ptr<GameState>(new ShoppingState());
  _gameStates[GameStates::RETURNER_STATE]           = std::unique_ptr<GameState>(new ReturnerState());
  _gameStates[GameStates::EXITING_STATE]            = std::unique_ptr<GameState>(new ExitingState());
  _gameStates[GameStates::MESSAGE_BOX_STATE]        = std::unique_ptr<GameState>(new MessageBoxState());
  _gameStates[GameStates::ENDGAME_STATE]            = std::unique_ptr<GameState>(new EndgameState());

  for (auto& state : _gameStates)
  {
    state.second.get()->Init();
  }

  _currentState = _gameStates[GameStates::MENU_STATE].get();

  Printer::Instance().AddMessage("You begin your quest");
}

void Application::Run()
{
  while (_currentState != nullptr)
  {
    // Since change state happens in HandleInput, if it's called before Update
    // to exit game (change state to nullptr) we get segfault because
    // _currentState->Update() gets called on nullptr.
    //
    // NOTE: Probably still a bad idea to just change order of methods call,
    // since we might get the same situation in Update().

    _currentState->Update();
    _currentState->HandleInput();
  }
}

void Application::ChangeState(const GameStates& gameStateIndex)
{
  if (gameStateIndex != GameStates::EXIT_GAME)
  {
    auto str = Util::StringFormat("Changing state: %s [0x%X] => %s [0x%X]",
                                  typeid(*_currentState).name(), _currentState,
                                  typeid(*_gameStates[gameStateIndex].get()).name(),
                                  _gameStates[gameStateIndex].get());
    Logger::Instance().Print(str);
  }
  else
  {
    auto str = Util::StringFormat("Changing state: %s [0x%X] => EXIT_GAME", typeid(*_currentState).name(), _currentState);
    Logger::Instance().Print(str);
  }

  _currentState->Cleanup();

  _currentState = (gameStateIndex == GameStates::EXIT_GAME) ? nullptr : _gameStates[gameStateIndex].get();

  if (_currentState != nullptr)
  {
    _currentState->Prepare();    
  }
}

GameState* Application::GetGameStateRefByName(GameStates stateName)
{
  if (_gameStates.count(stateName) == 1)
  {
    return _gameStates[stateName].get();
  }

  return nullptr;
}

void Application::ShowMessageBox(MessageBoxType type, std::string header, std::vector<std::string> message,
                                 std::string borderColor, std::string bgColor)
{
  _previousState = _currentState;

  auto ptr = _gameStates[GameStates::MESSAGE_BOX_STATE].get();
  MessageBoxState* mbs = static_cast<MessageBoxState*>(ptr);
  mbs->SetMessage(type, header, message, borderColor, bgColor);

  _currentState = ptr;
}

void Application::CloseMessageBox()
{
  _currentState = _previousState;
}

void Application::DisplayAttack(GameObject* defender, int delayMs, std::string cursorColor)
{
  int posX = defender->PosX + Map::Instance().CurrentLevel->MapOffsetX;
  int posY = defender->PosY + Map::Instance().CurrentLevel->MapOffsetY;

  if (cursorColor.length() == 0)
  {
    Printer::Instance().PrintFB(posX, posY, defender->Image, defender->FgColor, defender->BgColor);
    Printer::Instance().Render();
  }
  else
  {
    Printer::Instance().PrintFB(posX, posY, ' ', "#000000", cursorColor);
    Printer::Instance().Render();
  }

  Util::Sleep(delayMs);
}
