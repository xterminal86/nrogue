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
      printf("%s 0x%X\n", typeid(*_currentState).name(), _currentState);
        
      while (_currentState != nullptr)
      {        
        _currentState->HandleInput();
        _currentState->Update();
      }      
    }
    
    void ChangeState(const int gameStateIndex)
    {
      printf("Changing state %s 0x%X => %i ", typeid(*_currentState).name(), _currentState, gameStateIndex);
      
      _currentState = (gameStateIndex == kExitGameIndex) ? nullptr : _gameStates[gameStateIndex].get();
      
      printf("0x%X\n", _currentState);      
    }
  
  private:
    GameState* _currentState = nullptr;          
    
    std::map<int, std::unique_ptr<GameState>> _gameStates;  
};

#endif
