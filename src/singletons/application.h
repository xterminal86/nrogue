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
    void Run();
    void Cleanup();
    void ChangeState(const GameStates& gameStateIndex);

    void ShowMessageBox(MessageBoxType type,
                        const std::string& header,
                        const std::vector<std::string>& message,
                        const std::string& borderColor = Colors::ShadesOfGrey::Six,
                        const std::string& bgColor = Colors::ShadesOfGrey::Two);

    void CloseMessageBox();

    void DisplayAttack(GameObject* defender,
                       int delayMs,
                       const std::string& messageToPrint,
                       const std::string& cursorColor = "");

    void WriteObituary(bool wasKilled = true);

    static uint64_t GetNewId();

    const char& CharByCharIndex(uint8_t index);

    Player PlayerInstance;

    /// Force redraw current state
    ///
    /// Assuming that _currentState is MainState,
    /// call this to force redraw screen in that state
    /// only when needed ( see Player::WaitForTurn() )
    void ForceDrawMainState();

    void ForceDrawCurrentState();

    GameState* GetGameStateRefByName(GameStates stateName);
    bool CurrentStateIs(GameStates stateName);

#ifdef USE_SDL
    SDL_Renderer* Renderer = nullptr;
    SDL_Window* Window = nullptr;

    float ScaleFactor = 1.0f;

    int TileWidth = 0;
    int TileHeight = 0;

    int WindowWidth = 0;
    int WindowHeight = 0;

    std::string TilesetFilename;

    SDL_Rect GetWindowSize(int tileWidth, int tileHeight);

    const std::pair<int, int>& GetDefaultWindowSize();
#endif

    uint64_t TurnsPassed = 0;

  protected:
    void InitSpecific() override;

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
    void SavePrettyAlignedStatInfo(std::stringstream& ss);
    void SaveMapAroundPlayer(std::stringstream& ss, bool wasKilled);

    size_t SavePossessions(std::stringstream& ss);

#ifdef USE_SDL
    std::pair<int, int> _defaultWindowSize;
    std::map<std::string, std::string> _config;
    void InitSDL();
    void SetIcon();
#else
    void InitCurses();
#endif

    template <typename StateClass>
    inline void RegisterState(GameStates stateName)
    {
      _gameStates[stateName] = std::unique_ptr<GameState>(new StateClass());
    }

    struct StatInfo
    {
      std::string AttrName;
      int OriginalValue;
      int Modifier;
      int ResultingValue;
    };

    StatInfo GetStatInfo(const std::string& attrName);

    std::map<uint8_t, char> _charByCharIndex;

    void PrepareChars();

    friend class TargetState;
};

#endif
