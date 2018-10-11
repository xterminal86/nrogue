#include "application.h"

void Application::Init()
{
  _gameStates[(int)GameStates::MAIN_STATE] = std::unique_ptr<GameState>(new MainState());
  _gameStates[(int)GameStates::INFO_STATE] = std::unique_ptr<GameState>(new InfoState());
  _gameStates[(int)GameStates::MENU_STATE] = std::unique_ptr<GameState>(new MenuState());
  _gameStates[(int)GameStates::SELECT_CLASS_STATE] = std::unique_ptr<GameState>(new SelectClassState());
  _gameStates[(int)GameStates::INTRO_STATE] = std::unique_ptr<GameState>(new IntroState());
  _gameStates[(int)GameStates::SHOW_MESSAGES_STATE] = std::unique_ptr<GameState>(new MessageLogState());
  _gameStates[(int)GameStates::SHOW_HELP_STATE] = std::unique_ptr<GameState>(new HelpState());
  _gameStates[(int)GameStates::LOOK_INPUT_STATE] = std::unique_ptr<GameState>(new LookInputState());
  _gameStates[(int)GameStates::INTERACT_INPUT_STATE] = std::unique_ptr<GameState>(new InteractInputState());
  _gameStates[(int)GameStates::EXITING_STATE] = std::unique_ptr<GameState>(new ExitingState());

  for (auto& state : _gameStates)
  {
    state.second.get()->Init();
  }

  _currentState = _gameStates[(int)GameStates::MENU_STATE].get();

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
    // TODO: Probably still a bad idea to just change order of methods call,
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
                                  typeid(*_gameStates[(int)gameStateIndex].get()).name(),
                                  _gameStates[(int)gameStateIndex].get());
    Logger::Instance().Print(str);
  }
  else
  {
    auto str = Util::StringFormat("Changing state: %s [0x%X] => EXIT_GAME", typeid(*_currentState).name(), _currentState);
    Logger::Instance().Print(str);
  }

  _currentState->Cleanup();

  _currentState = (gameStateIndex == GameStates::EXIT_GAME) ? nullptr : _gameStates[(int)gameStateIndex].get();

  if (_currentState != nullptr)
  {
    _currentState->Prepare();    
  }
}
