#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include "gamestate.h"
#include "util.h"

enum class DevConsoleCommand
{
  UNKNOWN = 0,
  CLEAR,
  HELP,
  CLOSE,
  TRANSFORM_TILE,
  PLACE_WALL,
  GET_STATIC_OBJECT,
  GET_ACTOR,
  GET_ITEM,
  GET_BY_ADDRESS,
  LIST_TRIGGERS,
  GIVE_MONEY,
  POISON_ACTOR,
  DAMAGE_ACTOR,
  MOVE_STATIC_OBJECT,
  MOVE_ACTOR,
  MOVE_ITEM,
  MOVE_PLAYER,
  REPORT_PLAYER,
  REMOVE_OBJECT,
  SHOW_MAP,
  LEVEL_UP,
  LEVEL_DOWN,
  PRINT_MAP,
  PRINT_COLORS,
  CREATE_MONSTER,
  DISPEL_EFFECTS,
  INFO_HANDLES
};

enum class ObjectHandleType
{
  STATIC = 0,
  ITEM,
  ACTOR,
  ANY
};

class MapLevelBase;
class Player;
class GameObject;

class DevConsole : public GameState
{
  public:
    void Init() override;
    void Prepare() override;
    void Cleanup() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    MapLevelBase* _currentLevel;
    Player* _playerRef;

    std::map<ObjectHandleType, GameObject*> _objectHandles;

    std::string _currentCommand;

    std::vector<std::string> _stdout;
    std::vector<std::string> _commandsHistory;

    int _oldIndex = 0;
    int _commandsHistoryIndex = -1;

    int _cursorX = 1;
    int _cursorY = 2;

    bool _closedByCommand = false;

    const size_t _maxHistory = 20;

    const std::string ErrUnknownCommand = "command not found...";
    const std::string ErrSyntaxError    = "Syntax error";
    const std::string ErrWrongParams    = "Wrong params";
    const std::string ErrNoObjectsFound = "No objects found";
    const std::string ErrHandleNotSet   = "Handle not set";
    const std::string ErrCannotMove     = "Cannot move, probably occupied";
    const std::string ErrInvalidType    = "Invalid type";
    const std::string ErrCmdNotHandled  = "Command not handled";

    const std::string Ok = "Ok";
    const std::string Prompt = "> ";

    const std::map<ObjectHandleType, std::string> _handleNameByType =
    {
      { ObjectHandleType::STATIC, "_static" },
      { ObjectHandleType::ACTOR,  "_actor"  },
      { ObjectHandleType::ITEM,   "_item"   },
      { ObjectHandleType::ANY,    "_any"    }
    };

    const std::map<GameObjectType, std::string> _validTileTransformTypes =
    {
      { GameObjectType::GROUND,        "Ground"        },
      { GameObjectType::LAVA,          "Lava"          },
      { GameObjectType::SHALLOW_WATER, "Shallow Water" },
      { GameObjectType::DEEP_WATER,    "Deep Water"    },
      { GameObjectType::CHASM,         "Chasm"         }
    };

    const std::map<GameObjectType, std::string> _monsters =
    {
      { GameObjectType::RAT,         "Rat"         },
      { GameObjectType::BAT,         "Bat"         },
      { GameObjectType::VAMPIRE_BAT, "Vampire Bat" },
      { GameObjectType::SPIDER,      "Cave Spider" },
      { GameObjectType::SHELOB,      "Shelob"      },
      { GameObjectType::TROLL,       "Troll"       },
      { GameObjectType::MAD_MINER,   "Mad Miner"   },
      { GameObjectType::SKELETON,    "Skeleton"    },
      { GameObjectType::ZOMBIE,      "Zombie"      },
      { GameObjectType::LICH,        "Lich"        },
      { GameObjectType::KOBOLD,      "Kobold"      },
      { GameObjectType::WRAITH,      "Wraith"      },
      { GameObjectType::HEROBRINE,   "Herobrine"   }
    };

    void StdOut(const std::string& str);

    bool ParseCommand();

    void ProcessCommand(const std::string& command,
                        const std::vector<std::string>& params);
    void DisplayHelpAboutCommand(const std::vector<std::string>& params);
    void PrintAdditionalHelp(DevConsoleCommand command);
    void InfoHandles();
    void CreateMonster(const std::vector<std::string>& params);
    void GetObject(const std::vector<std::string>& params,
                   ObjectHandleType handleType);
    void MoveObject(const std::vector<std::string>& params,
                    ObjectHandleType handleType);
    void MovePlayer(const std::vector<std::string>& params);
    void RemoveObject(const std::vector<std::string>& params);
    void DamageActor(const std::vector<std::string>& params);
    void PoisonActor(const std::vector<std::string>& params);
    void PrintColors();
    void TransformTile(const std::vector<std::string>& params);
    void PlaceWall(const std::vector<std::string>& params);
    void GetObjectByAddress(const std::vector<std::string>& params);
    void ReportHandleDebugInfo(ObjectHandleType type);
    void PrintDebugInfo(const std::vector<std::string>& debugInfo);
    void GiveMoney(const std::vector<std::string>& params);
    void ToggleFogOfWar();
    void PrintTriggers();
    void DispelEffects();

    bool StringIsNumbers(const std::string& str);
    std::pair<int, int> CoordinateParamsToInt(const std::string& px,
                                              const std::string& py);

    void ReportHandle(ObjectHandleType handleType);

    template <typename Map>
    void PrintMap(const Map& map)
    {
      StdOut("Valid types:");
      for (auto& kvp : map)
      {
        std::string str = Util::StringFormat("%i = %s", (int)kvp.first, kvp.second.data());
        StdOut(str);
      }
    }

    std::vector<std::string> _allCommandsList;

    const std::map<std::string, DevConsoleCommand> _commandTypeByName =
    {
      { "clear",  DevConsoleCommand::CLEAR              },
      { "help",   DevConsoleCommand::HELP               },
      { "exit",   DevConsoleCommand::CLOSE              },
      { "q",      DevConsoleCommand::CLOSE              },
      { "quit",   DevConsoleCommand::CLOSE              },
      { "m_trns", DevConsoleCommand::TRANSFORM_TILE     },
      { "m_pw",   DevConsoleCommand::PLACE_WALL         },
      { "m_show", DevConsoleCommand::SHOW_MAP           },
      { "info",   DevConsoleCommand::INFO_HANDLES       },
      { "i_trig", DevConsoleCommand::LIST_TRIGGERS      },
      { "so_get", DevConsoleCommand::GET_STATIC_OBJECT  },
      { "ao_get", DevConsoleCommand::GET_ACTOR          },
      { "io_get", DevConsoleCommand::GET_ITEM           },
      { "so_mov", DevConsoleCommand::MOVE_STATIC_OBJECT },
      { "ao_mov", DevConsoleCommand::MOVE_ACTOR         },
      { "ao_psn", DevConsoleCommand::POISON_ACTOR       },
      { "ao_dmg", DevConsoleCommand::DAMAGE_ACTOR       },
      { "io_mov", DevConsoleCommand::MOVE_ITEM          },
      { "p_mov",  DevConsoleCommand::MOVE_PLAYER        },
      { "p_de",   DevConsoleCommand::DISPEL_EFFECTS     },
      { "o_del",  DevConsoleCommand::REMOVE_OBJECT      },
      { "p_lu",   DevConsoleCommand::LEVEL_UP           },
      { "p_ld",   DevConsoleCommand::LEVEL_DOWN         },
      { "p_info", DevConsoleCommand::REPORT_PLAYER      },
      { "g_pm",   DevConsoleCommand::PRINT_MAP          },
      { "g_pc",   DevConsoleCommand::PRINT_COLORS       },
      { "g_cm",   DevConsoleCommand::CREATE_MONSTER     },
      { "g_go",   DevConsoleCommand::GET_BY_ADDRESS     },
      { "g_gm",   DevConsoleCommand::GIVE_MONEY         }
    };

    const std::vector<std::string> _help =
    {
      "help commands  - displays list of all available commands",
      "help <command> - displays help about <command>"
    };

    const std::map<std::string, std::vector<std::string>> _helpTextByCommandName =
    {
      { "help",   { "Not funny, didn't laugh"  } },
      { "clear",  { "Clears the screen"        } },
      { "q",      { "Close the console"        } },
      { "quit",   { "Close the console"        } },
      { "info",   { "Show handlers"            } },
      { "exit",   { "Close the console"        } },
      { "p_lu",   { "Give player a level"      } },
      { "p_ld",   { "Take a level from player" } },
      { "p_info", { "Print debug info about player" } },
      { "g_pm",   { "Save current map layout to a file" } },
      { "g_pc",   { "Prints colors used so far" } },
      { "g_go",   { "g_go 0x%X", "Get game object by address (slow!)" } },
      { "ao_psn", { "Add lingering damage to actor in handle" } },
      { "m_show", { "Toggle fog of war" } },
      { "i_trig", { "Print current level triggers" } },
      { "p_de",   { "Dispel effects from player" } },
      {
        "so_get",
        { "so_get [X Y]", "Try to get handle to static object at X Y" }
      },
      {
        "ao_get",
        { "ao_get [X Y]", "Try to get handle to actor at X Y" }
      },
      {
        "io_get",
        { "io_get [X Y]", "Try to get handle to item object at X Y" }
      },
      {
        "so_mov",
        { "so_mov X Y", "Try to move static object in handle to X Y" }
      },
      {
        "ao_mov",
        { "ao_mov X Y", "Try to move actor in handle to X Y" }
      },
      {
        "ao_dmg",
        { "ao_dmg N <DIRECT>", "Inflict N damage to actor in handle" }
      },
      {
        "io_mov",
        { "io_mov X Y", "Try to move item in handle to X Y" }
      },
      {
        "p_mov",
        { "p_mov X Y", "Try to move player to X Y" }
      },
      {
        "o_del",
        { "o_del X Y", "Delete any object at X Y (actor -> item -> static)" }
      },
      {
        "m_trns",
        { "m_trns X Y <TYPE>", "Transform tile X Y to type <TYPE>" }
      },
      {
        "m_pw",
        { "m_pw X Y", "Places generic wall at X Y" }
      },
      {
        "g_cm",
        { "g_cm X Y <TYPE>", "Create monster <TYPE> at X Y" }
      },
      {
        "g_gm",
        { "g_gm <AMOUNT>", "Give player <AMOUNT> money" }
      }
    };

    friend class GameObject;
};

#endif // DEVCONSOLE_H
