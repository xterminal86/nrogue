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

    Player PlayerInstance;

    /// Force redraw current state
    ///
    /// Assuming that _currentState is MainState,
    /// call this to force redraw screen in that state
    /// only when needed ( see Player::WaitForTurn() )
    void DrawCurrentState();

    GameState* GetGameStateRefByName(GameStates stateName);

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

  private:
    GameState* _currentState = nullptr;
    GameState* _previousState = nullptr;

    std::map<GameStates, std::unique_ptr<GameState>> _gameStates;    

    void InitGraphics();
    void InitGameStates();
    void DrawAttackCursor(int x, int y,
                          GameObject* defender,
                          const std::string& cursorColor = "");

    #ifdef USE_SDL
    std::map<std::string, std::string> _config;
    void InitSDL();
    #else
    void InitCurses();
    #endif
};

#endif
