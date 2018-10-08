#ifndef APPLICATION_H
#define APPLICATION_H

#include "singleton.h"
#include "mainstate.h"
#include "infostate.h"
#include "menustate.h"
#include "select-class-state.h"
#include "intro-state.h"
#include "message-log-state.h"
#include "logger.h"
#include "player.h"

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
      SELECT_CLASS_STATE,
      INTRO_STATE,
      INFO_STATE,
      SHOW_MESSAGES_STATE
    };

    void Init() override;
    void Run();
    void ChangeState(const GameStates& gameStateIndex);
  
    Player PlayerInstance;

  private:
    GameState* _currentState = nullptr;          
        
    std::map<int, std::unique_ptr<GameState>> _gameStates;  
};

#endif
