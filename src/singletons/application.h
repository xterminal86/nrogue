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
                        const uint32_t& borderColor = Colors::ShadesOfGrey::Six,
                        const uint32_t& bgColor = Colors::ShadesOfGrey::Two);

    void CloseMessageBox();

    void DisplayAttack(GameObject* defender,
                       uint32_t delayMs,
                       const std::string& messageToPrint,
                       const uint32_t& cursorColor = Colors::None);

    void WriteObituary(bool wasKilled = true);

    static uint64_t GetNewGlobalId();

    const char& CharByCharIndex(uint8_t index);

    Player PlayerInstance;

    struct Config
    {
      double ScaleFactor = 1.0;

      int TileWidth = 0;
      int TileHeight = 0;

      int WindowWidth = 0;
      int WindowHeight = 0;

      //
      // Disables attack display animation thus reducing gameplay lag.
      //
      bool FastCombat = false;

      //
      // Disables force draw update after each visible monster update
      // thus reducing gameplay lag even more, but potentially
      // confusing the player in case of when very fast monster
      // walks around the wall, it will appear as if the monster
      // just spawned in front of player, or even instakilled him
      // in case of very fast attacking.
      //
      bool FastMonsterMovement = false;

      std::string TilesetFilename;
    };

    Config GameConfig;

    //
    // Force redraw current state.
    //
    // Assuming that _currentState is MainState,
    // call this to force redraw screen in that state
    // only when needed ( see Player::WaitForTurn() )
    //
    void ForceDrawMainState();

    void ForceDrawCurrentState();

    GameState* GetGameStateRefByName(GameStates stateName);
    bool CurrentStateIs(GameStates stateName);

    bool AppReady();

#ifdef USE_SDL
    SDL_Renderer* Renderer = nullptr;
    SDL_Window* Window = nullptr;

    SDL_Rect GetWindowSize(int tileWidth, int tileHeight);

    const std::pair<int, int>& GetDefaultWindowSize();
    std::pair<int, int>& GetResizedWindowSize();
#endif

    //
    // Can be used to time global triggers.
    //
    uint64_t MapUpdateCyclesPassed = 0;

    uint64_t PlayerTurnsPassed = 0;

  protected:
    void InitSpecific() override;

  private:
    bool _appReady = false;

    GameState* _currentState = nullptr;
    GameState* _previousState = nullptr;

    std::map<GameStates, std::unique_ptr<GameState>> _gameStates;

    std::map<std::string, std::string> _config;

    void ParseConfig();
    void SetConfig();
    bool InitGraphics();
    void InitGameStates();
    void DrawAttackCursor(int x, int y,
                          GameObject* defender,
                          const uint32_t& cursorColor = Colors::None);
    void SavePrettyAlignedStatInfo(std::stringstream& ss);
    void SaveMapAroundPlayer(std::stringstream& ss, bool wasKilled);

    size_t SavePossessions(std::stringstream& ss);

#ifdef USE_SDL
    std::pair<int, int> _defaultWindowSize;
    std::pair<int, int> _resizedWindowSize;
    bool InitSDL();
    void SetIcon();
#else
    bool InitCurses();
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

    const std::string kConfigKeyFile                = "FILE";
    const std::string kConfigKeyTileW               = "TILE_W";
    const std::string kConfigKeyTileH               = "TILE_H";
    const std::string kConfigKeyScale               = "SCALE";
    const std::string kConfigKeyFastCombat          = "FAST_COMBAT";
    const std::string kConfigKeyFastMonsterMovement = "FAST_MONSTER_MOVEMENT";

    // =========================================================================

    const std::vector<std::string> _statNames =
    {
      "STR", "DEF", "MAG", "RES", "SKL", "SPD"
    };

    const std::map<std::string, Attribute&> _attrsByName =
    {
      { "STR", PlayerInstance.Attrs.Str },
      { "DEF", PlayerInstance.Attrs.Def },
      { "MAG", PlayerInstance.Attrs.Mag },
      { "RES", PlayerInstance.Attrs.Res },
      { "SKL", PlayerInstance.Attrs.Skl },
      { "SPD", PlayerInstance.Attrs.Spd }
    };

    friend class TargetState;
};

#endif
