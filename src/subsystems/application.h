#ifndef APPLICATION_H
#define APPLICATION_H

#include <typeinfo>
#include <memory>
#include <vector>
#include <list>
#include <map>

#include "gamestate.h"
#include "player.h"
#include "serializer.h"

class Application
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

    StringV CollectObituary(bool wasKilled, bool asciiMode);

    void WriteObituary(bool wasKilled = true);

    void LoadGame();
    void SaveGame();

    struct Config
    {
      double ScaleFactor = 1.0;

      int TileSize = 16;

      int WindowWidth  = 0;
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

      //
      // Use graphic tiles.
      //
      bool UseGraphics = false;

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

    bool IsAppReady();

    //
    // Can be used to time global triggers.
    //
    uint64_t MapUpdateCyclesPassed = 0;

    uint64_t PlayerTurnsPassed = 0;

    void Init(bool skipMenu = false);

  private:
    bool _appReady = false;

    GameState* _currentState = nullptr;
    GameState* _previousState = nullptr;

    //
    // Order of class members destruction is opposite of declaration, so because
    // we need to do some shit in ~GameObject() (basically PlayerInstance here)
    // by accessing _gameStates we should create PlayerInstance after
    // _gameStates.
    //
    std::unordered_map<GameStates, std::unique_ptr<GameState>> _gameStates;

  public:
    Player PlayerInstance;

  private:
    NRS _loadedConfig;

    void LoadConfig();

    bool InitGraphics();

    void InitGameStates(bool restart = false);

    void DrawAttackCursor(int x, int y,
                          GameObject* defender,
                          const uint32_t& cursorColor = Colors::None);

    void CollectPrettyAlignedStatInfo(StringV& writeTo, bool asciiMode);
    void CollectMapAroundPlayer(StringV& writeTo,
                                bool wasKilled,
                                bool asciiMode);
    void CollectGeneralInfo(StringV& writeTo, bool wasKilled, bool asciiMode);

    size_t CollectPossessions(StringV& writeTo, bool asciiMode);

    void CollectKills(StringV& writeTo,
                      size_t stringResizeWidth,
                      bool asciiMode);

#ifdef USE_SDL
    bool InitSDL();
#else
    bool InitCurses();
#endif

    void WriteObituaryLine(StringV& writeTo,
                           const std::string& line,
                           bool asciiMode);

    template <typename StateClass>
    inline void RegisterState(GameStates stateName)
    {
      _gameStates[stateName] = std::make_unique<StateClass>();
    }

    struct StatInfo
    {
      std::string AttrName;
      int OriginalValue  = 0;
      int Modifier       = 0;
      int ResultingValue = 0;
    };

    StatInfo GetStatInfo(const std::string& attrName);

    void SaveBaseStuff(NRS& save);
    void SavePlayer(NRS& save);

    void PrepareChars();

    const std::string kConfigKeyTileset             = "tileset";
    const std::string kConfigKeyTileSize            = "tile_size";
    const std::string kConfigKeyScale               = "scale";
    const std::string kConfigKeyFastCombat          = "fast_combat";
    const std::string kConfigKeyFastMonsterMovement = "fast_monster_movement";

    template <typename T>
    bool ParseValue(const std::string& key, T& out)
    {
      const std::string& res = _loadedConfig[key].GetString();
      if (!res.empty())
      {
        for (char c : res)
        {
          bool ok = (std::isdigit(c) || c == '.');
          if (!ok)
          {
            ConsoleLog("[ERR] %s is not a number!", key.data());
            return false;
          }
        }

        if (std::is_floating_point<T>::value)
        {
          out = std::stod(res);
        }
        else
        {
          out = std::stoi(res, nullptr, 0);
        }
      }
      else
      {
        ConsoleLog("[WAR] failed to read value '%s'", key.data());
      }

      return true;
    }

    // =========================================================================

    const std::vector<std::string> _statNames =
    {
      "STR", "DEF", "MAG", "RES", "SKL", "SPD"
    };

    const std::unordered_map<std::string, Attribute&> _attrsByName =
    {
      { "STR", PlayerInstance.Attrs.Str },
      { "DEF", PlayerInstance.Attrs.Def },
      { "MAG", PlayerInstance.Attrs.Mag },
      { "RES", PlayerInstance.Attrs.Res },
      { "SKL", PlayerInstance.Attrs.Skl },
      { "SPD", PlayerInstance.Attrs.Spd }
    };

    bool _initialized = false;

    friend class TargetState;
};

#endif
