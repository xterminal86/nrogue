#ifndef APPLICATION_H
#define APPLICATION_H

#include <typeinfo>
#include <memory>
#include <vector>
#include <map>

#include "singleton.h"
#include "gamestate.h"
#include "player.h"

class Application : public Singleton<Application>
{
  public:
    void Init() override;
    void Run();
    void ChangeState(const GameStates& gameStateIndex);

    void ShowMessageBox(MessageBoxType type, std::string header, std::vector<std::string> message,
                        std::string borderColor = GlobalConstants::MessageBoxDefaultBorderColor,
                        std::string bgColor = GlobalConstants::MessageBoxDefaultBgColor);

    void CloseMessageBox();

    void DisplayAttack(GameObject* defender, int delayMs, std::string cursorColor = "");

    Player PlayerInstance;

    /// Force redraw current state
    ///
    /// Assuming that _currentState is MainState,
    /// call this to force redraw screen in that state
    /// only when needed ( see Player::WaitForTurn() )
    void DrawCurrentState()
    {
      if (_currentState != nullptr)
      {
        _currentState->Update(true);
      }
    }

    GameState* GetGameStateRefByName(GameStates stateName);

  private:
    GameState* _currentState = nullptr;
    GameState* _previousState = nullptr;

    std::map<GameStates, std::unique_ptr<GameState>> _gameStates;
};

#endif