#ifndef APPLICATION_H
#define APPLICATION_H

#include "singleton.h"
#include "mainstate.h"
#include "infostate.h"
#include "menustate.h"
#include "logger.h"

#include <typeinfo>
#include <memory>
#include <vector>
#include <map>

class Application : public Singleton<Application>
{
  public:        

    enum class GameStates
    {
      EXIT_GAME = -1,
      MENU_STATE,
      MAIN_STATE,
      INFO_STATE
    };

    void Init() override
    {
      _gameStates[(int)GameStates::MAIN_STATE] = std::unique_ptr<GameState>(new MainState());
      _gameStates[(int)GameStates::INFO_STATE] = std::unique_ptr<GameState>(new InfoState());    
      _gameStates[(int)GameStates::MENU_STATE] = std::unique_ptr<GameState>(new MenuState());    

      for (auto& state : _gameStates)
      {
        state.second.get()->Init();
      }
      
      _currentState = _gameStates[(int)GameStates::MENU_STATE].get();
    }
    
    void Run()
    {
      while (_currentState != nullptr)
      {        
        // Since change state happens in HandleInput, if it's called before Update
        // to exit game (change state to nullptr), we get segfault because
        // _currentState->Update() gets called on nullptr
        //
        // TODO: Probably still bad idea to just change order of methods call, since
        // we might get the same situation in Update().
        
        _currentState->Update();
        _currentState->HandleInput();                
      }      
    }
    
    void ChangeState(const GameStates& gameStateIndex)
    {
      if (gameStateIndex != GameStates::EXIT_GAME)
      {
        auto str = Util::StringFormat("Changing state: %s [0x%X] => %s [0x%X]\n", typeid(*_currentState).name(), _currentState, 
                                                     typeid(*_gameStates[(int)gameStateIndex].get()).name(), 
                                                     _gameStates[(int)gameStateIndex].get());
        Logger::Instance().Print(str);
      }
      else
      {
        auto str = Util::StringFormat("Changing state: %s [0x%X] => EXIT_GAME\n", typeid(*_currentState).name(), _currentState);
        Logger::Instance().Print(str);
      }

      _currentState = (gameStateIndex == GameStates::EXIT_GAME) ? nullptr : _gameStates[(int)gameStateIndex].get();
    }
  
  private:
    GameState* _currentState = nullptr;          
    
    std::map<int, std::unique_ptr<GameState>> _gameStates;  
};

#endif
