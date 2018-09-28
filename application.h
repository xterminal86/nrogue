#ifndef APPLICATION_H
#define APPLICATION_H

#include "singleton.h"
#include "mainstate.h"
#include "infostate.h"

#include <typeinfo>
#include <memory>
#include <vector>
#include <map>

class Application : public Singleton<Application>
{
  public:        
    const int kExitGameIndex = -1;
    const int kMainStateIndex = 0;
    const int kInfoStateIndex = 1;
    
    void Init() override
    {
      _gameStates[kMainStateIndex] = std::unique_ptr<GameState>(new MainState());
      _gameStates[kInfoStateIndex] = std::unique_ptr<GameState>(new InfoState());    
      
      for (auto& state : _gameStates)
      {
        state.second.get()->Init();
      }
      
      _currentState = _gameStates[kMainStateIndex].get();
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
    
    void ChangeState(const int gameStateIndex)
    {
      //printf("Changing state %s 0x%X => %i ", typeid(*_currentState).name(), _currentState, gameStateIndex);
      
      _currentState = (gameStateIndex == kExitGameIndex) ? nullptr : _gameStates[gameStateIndex].get();
    }
  
  private:
    GameState* _currentState = nullptr;          
    
    std::map<int, std::unique_ptr<GameState>> _gameStates;  
};

#endif
