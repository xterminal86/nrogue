#ifndef APPLICATION_H
#define APPLICATION_H

#include <typeinfo>
#include <memory>
#include <vector>
#include <map>
#include <chrono>

#include "singleton.h"
#include "gamestate.h"
#include "player.h"

using Timer = std::chrono::high_resolution_clock;
using Ms = std::chrono::milliseconds;
using Ns = std::chrono::nanoseconds;
using Sec = std::chrono::seconds;

class Application : public Singleton<Application>
{
  public:
    void Init() override;
    void Run();
    void Cleanup();
    void ChangeState(const GameStates& gameStateIndex);

    void ShowMessageBox(MessageBoxType type,
                        const std::string& header,
                        const std::vector<std::string>& message,
                        const std::string& borderColor = GlobalConstants::MessageBoxDefaultBorderColor,
                        const std::string& bgColor = GlobalConstants::MessageBoxDefaultBgColor);

    void CloseMessageBox();

    void DisplayAttack(GameObject* defender,
                       int delayMs,
                       const std::string& messageToPrint,
                       const std::string& cursorColor = "");

    void WriteObituary(bool wasKilled = true);

    static uint64_t GetNewId();

    Player PlayerInstance;

    /// Force redraw current state
    ///
    /// Assuming that _currentState is MainState,
    /// call this to force redraw screen in that state
    /// only when needed ( see Player::WaitForTurn() )
    void ForceDrawMainState();

    GameState* GetGameStateRefByName(GameStates stateName);
    bool CurrentStateIs(GameStates stateName);

    Ns DeltaTime();
    Ns TimePassed();

    #ifdef USE_SDL
    SDL_Renderer* Renderer = nullptr;
    SDL_Window* Window = nullptr;

    float ScaleFactor = 1.0f;

    int TileWidth = 0;
    int TileHeight = 0;

    int WindowWidth = 0;
    int WindowHeight = 0;

    std::string TilesetFilename;
    #endif

    unsigned int TurnsPassed = 0;

  private:
    GameState* _currentState = nullptr;
    GameState* _previousState = nullptr;

    std::map<GameStates, std::unique_ptr<GameState>> _gameStates;    

    void ParseConfig();
    void ProcessConfig();
    void InitGraphics();
    void InitGameStates();
    void DrawAttackCursor(int x, int y,
                          GameObject* defender,
                          const std::string& cursorColor = "");

    #ifdef USE_SDL
    std::map<std::string, std::string> _config;
    void InitSDL();
    void SetIcon();
    #else
    void InitCurses();
    #endif

    template <typename StateClass>
    void RegisterState(GameStates stateName)
    {
      _gameStates[stateName] = std::unique_ptr<GameState>(new StateClass());
    }

    // chrono clock now() uses
    // nanoseconds as internal duration period
    // (see time_point struct declaration)
    Ns _deltaTime = Ns{0};
    Ns _timePassed = Ns{0};

    friend class TargetState;    
};

#endif
