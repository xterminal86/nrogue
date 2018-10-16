#ifndef APPLICATION_H
#define APPLICATION_H

#include "singleton.h"
#include "gamestate.h"

#include "logger.h"
#include "player.h"
#include "printer.h"

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
      ENTER_NAME_STATE,
      INTRO_STATE,
      INFO_STATE,
      INVENTORY_STATE,
      SHOW_MESSAGES_STATE,
      SHOW_HELP_STATE,
      LOOK_INPUT_STATE,
      INTERACT_INPUT_STATE,
      EXITING_STATE,
      MESSAGE_BOX_STATE
    };

    void Init() override;
    void Run();
    void ChangeState(const GameStates& gameStateIndex);
    void ShowMessageBox(std::string message);
    void CloseMessageBox();

    Player PlayerInstance;    

  private:
    GameState* _currentState = nullptr;          
    GameState* _previousState = nullptr;

    std::map<int, std::unique_ptr<GameState>> _gameStates;  
};

#endif
