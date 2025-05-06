#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include "gamestate.h"
#include "util.h"
#include "trie.h"

enum class DevConsoleCommand
{
  //
  // https://pvs-studio.com/en/blog/posts/cpp/1053/
  //
  // "Beautiful code is correct code"
  //
  // By putting comma on the left side, it's easier to reformat if needed,
  // without having to insert spaces on the right side everywhere.
  // And in case of missing comma, it's more obvious.
  // Also, as a bonus, it makes it easier to write line comments to the right
  // of enum entry.
  //
    UNKNOWN = 0
  , CLEAR
  , HELP
  , CLOSE
  , CLOSE2
  , CLOSE3
  , CLOSE4
  , HISTORY
  , REPEAT_COMMAND
  , TRANSFORM_TILE
  , PLACE_WALL
  , CREATE_DUMMY_ACTOR
  , CREATE_ALL_POTIONS
  , CREATE_ALL_GEMS
  , CREATE_ALL_SCROLLS
  , CREATE_SHRINE
  , CREATE_DUMMY_OBJECT
  , CREATE_CHEST
  , CREATE_BREAKABLE
  , CREATE_DOOR
  , CREATE_ITEM
  , DISPEL_EFFECTS
  , DISPEL_EFFECTS_ACTOR
  , GET_MAP_OBJECT
  , GET_STATIC_OBJECT
  , GET_ACTOR
  , GET_ITEM
  , GET_BY_ADDRESS
  , GET_BY_ID
  , GET_ANY_OBJECT
  , GOD_MODE
  , IGNORE_PLAYER
  , PRINT_TRIGGERS
  , PRINT_ACTORS
  , GIVE_MONEY
  , POISON_ACTOR
  , DAMAGE_ACTOR
  , MOVE_STATIC_OBJECT
  , MOVE_ACTOR
  , MOVE_ITEM
  , MOVE_PLAYER
  , REPORT_PLAYER
  , REMOVE_OBJECT
  , SHOW_MAP
  , AWARD_EXP
  , LEVEL_UP
  , LEVEL_DOWN
  , PRINT_MAP
  , PRINT_COLORS
  , CREATE_MONSTER
  , INFO_HANDLES
};

enum class ObjectHandleType
{
    STATIC = 0
  , ITEM
  , ACTOR
  , MAP
  , ANY
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

    std::unordered_map<ObjectHandleType, GameObject*> _objectHandles;

    std::string _currentCommand;

    std::vector<std::string> _stdout;
    std::vector<std::string> _commandsHistory;

    int _commandsHistoryIndex = -1;

    int _cursorPosition = 0;

    int _cursorX = 1;
    int _cursorY = 2;

    bool _closedByCommand = false;

    const size_t _maxHistory = 20;

    const std::string ErrUnknownCommand = "%s: command not found...";
    const std::string ErrSyntaxError    = "Syntax error";
    const std::string ErrWrongParams    = "Wrong params";
    const std::string ErrOutOfBounds    = "Out of bounds!";
    const std::string ErrNoObjectsFound = "No objects found";
    const std::string ErrHandleNotSet   = "Handle not set";
    const std::string ErrCannotMove     = "Cannot move, probably occupied";
    const std::string ErrInvalidType    = "Invalid type";
    const std::string ErrCmdNotHandled  = "Command not handled";
    const std::string ErrCantLevelDown  = "Error: current level is 1";
    const std::string ErrEventNotFound  = "!%d: event not found";

    const std::string Ok = "Ok";
    const std::string Prompt = "> ";

    const std::unordered_map<ObjectHandleType, std::string> _handleNameByType =
    {
      { ObjectHandleType::STATIC, "_static" },
      { ObjectHandleType::ACTOR,  "_actor"  },
      { ObjectHandleType::ITEM,   "_item"   },
      { ObjectHandleType::MAP,    "_map"    },
      { ObjectHandleType::ANY,    "_any"    }
    };

    const std::unordered_map<GameObjectType, std::string> _validTileTransformTypes =
    {
      { GameObjectType::GROUND,        "Ground"        },
      { GameObjectType::LAVA,          "Lava"          },
      { GameObjectType::SHALLOW_WATER, "Shallow Water" },
      { GameObjectType::DEEP_WATER,    "Deep Water"    },
      { GameObjectType::CHASM,         "Chasm"         }
    };

    const std::unordered_map<GameObjectType, std::string> _monsters =
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
      { GameObjectType::NECROMANCER, "Necromancer" },
      { GameObjectType::KOBOLD,      "Kobold"      },
      { GameObjectType::WRAITH,      "Wraith"      },
      { GameObjectType::HEROBRINE,   "Herobrine"   },
      { GameObjectType::STALKER,     "Gollum"      }
    };

    void StdOut(const std::string& str);

    bool ParseCommand();

    void ProcessCommand(const std::string& command,
                        const std::vector<std::string>& params);
    void DisplayHelpAboutCommand(const std::vector<std::string>& params);
    void PrintAdditionalHelp(DevConsoleCommand command);
    void PrintHistory();
    void RepeatCommand(const std::string& shellCmd);
    void InfoHandles();
    void CreateMonster(const std::vector<std::string>& params);
    void CreateDummyActor(const std::vector<std::string>& params);
    void CreateDummyObject(const std::vector<std::string>& params);
    void CreateChest(const std::vector<std::string>& params);
    void CreateAllGems();
    void CreateAllPotions();
    void CreateAllScrolls();
    void CreateItem(const std::vector<std::string>& params);
    void CreateShrine(const std::vector<std::string>& params);
    void CreateBreakable(const std::vector<std::string>& params);
    void GetObject(const std::vector<std::string>& params,
                   ObjectHandleType handleType);
    void MoveObject(const std::vector<std::string>& params,
                    ObjectHandleType handleType);
    void MovePlayer(const std::vector<std::string>& params);
    void RemoveObject(const std::vector<std::string>& params);
    void DamageActor(const std::vector<std::string>& params);
    void PoisonActor();
    void PrintColors();
    void TransformTile(const std::vector<std::string>& params);
    void PlaceWall(const std::vector<std::string>& params);
    void GetObjectByAddress(const std::vector<std::string>& params);
    void GetObjectById(const std::vector<std::string>& params);
    void ReportHandleDebugInfo(ObjectHandleType type);
    void PrintDebugInfo(const std::vector<std::string>& debugInfo);
    void GiveMoney(const std::vector<std::string>& params);
    void AwardExperience(const std::vector<std::string>& params);
    void ToggleFogOfWar();
    void ToggleGodMode();
    void TogglePlayerIgnore();
    void PrintTriggers();
    void PrintActors();

#ifdef DEBUG_BUILD
    void DispelEffects();
    void DispelEffectsActor();
#endif

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

    const std::string _commandsDelimiter = " | ";

    const std::map<DevConsoleCommand, std::string> _commandNameByType =
    {
      { DevConsoleCommand::CLEAR               , "clear"             },
      { DevConsoleCommand::HELP                , "help"              },
      { DevConsoleCommand::CLOSE               , "q"                 },
      { DevConsoleCommand::CLOSE2              , "quit"              },
      { DevConsoleCommand::CLOSE3              , "exit"              },
      { DevConsoleCommand::CLOSE4              , "close"             },
      { DevConsoleCommand::HISTORY             , "history"           },
      { DevConsoleCommand::REPEAT_COMMAND      , "!"                 },
      { DevConsoleCommand::TRANSFORM_TILE      , "map_trnstile"      },
      { DevConsoleCommand::PLACE_WALL          , "map_placewall"     },
      { DevConsoleCommand::SHOW_MAP            , "map_show"          },
      { DevConsoleCommand::PRINT_MAP           , "map_print2file"    },
      { DevConsoleCommand::INFO_HANDLES        , "info_handles"      },
      { DevConsoleCommand::PRINT_TRIGGERS      , "info_triggers"     },
      { DevConsoleCommand::PRINT_ACTORS        , "info_actors"       },
      { DevConsoleCommand::GET_STATIC_OBJECT   , "get_staticobj"     },
      { DevConsoleCommand::GET_MAP_OBJECT      , "get_mapobj"        },
      { DevConsoleCommand::GET_ACTOR           , "get_actor"         },
      { DevConsoleCommand::GET_ITEM            , "get_item"          },
      { DevConsoleCommand::GET_ANY_OBJECT      , "get_anyobj"        },
      { DevConsoleCommand::DISPEL_EFFECTS_ACTOR, "actor_dispel"      },
      { DevConsoleCommand::POISON_ACTOR        , "actor_poison"      },
      { DevConsoleCommand::DAMAGE_ACTOR        , "actor_damage"      },
      { DevConsoleCommand::MOVE_STATIC_OBJECT  , "mov_staticobj"     },
      { DevConsoleCommand::MOVE_ACTOR          , "mov_actor"         },
      { DevConsoleCommand::MOVE_ITEM           , "mov_item"          },
      { DevConsoleCommand::MOVE_PLAYER         , "mov_player"        },
      { DevConsoleCommand::DISPEL_EFFECTS      , "plr_dispel"        },
      { DevConsoleCommand::GIVE_MONEY          , "plr_givemoney"     },
      { DevConsoleCommand::AWARD_EXP           , "plr_awardexp"      },
      { DevConsoleCommand::LEVEL_UP            , "plr_levelup"       },
      { DevConsoleCommand::LEVEL_DOWN          , "plr_leveldown"     },
      { DevConsoleCommand::GOD_MODE            , "plr_godmode"       },
      { DevConsoleCommand::IGNORE_PLAYER       , "plr_ignore"        },
      { DevConsoleCommand::REPORT_PLAYER       , "plr_info"          },
      { DevConsoleCommand::REMOVE_OBJECT       , "eng_deleteobj"     },
      { DevConsoleCommand::PRINT_COLORS        , "eng_printclrs"     },
      { DevConsoleCommand::GET_BY_ADDRESS      , "eng_getbyaddr"     },
      { DevConsoleCommand::GET_BY_ID           , "eng_getbyid"       },
      { DevConsoleCommand::CREATE_MONSTER      , "create_monster"    },
      { DevConsoleCommand::CREATE_DUMMY_ACTOR  , "create_dummyactor" },
      { DevConsoleCommand::CREATE_ALL_GEMS     , "create_allgems"    },
      { DevConsoleCommand::CREATE_ALL_POTIONS  , "create_allpotions" },
      { DevConsoleCommand::CREATE_ALL_SCROLLS  , "create_allscrolls" },
      { DevConsoleCommand::CREATE_ITEM         , "create_item"       },
      { DevConsoleCommand::CREATE_SHRINE       , "create_shrine"     },
      { DevConsoleCommand::CREATE_DUMMY_OBJECT , "create_dummyobj"   },
      { DevConsoleCommand::CREATE_CHEST        , "create_chest"      },
      { DevConsoleCommand::CREATE_BREAKABLE    , "create_breakable"  }

    };

    std::map<std::string, DevConsoleCommand> _commandTypeByName;

    const std::vector<std::string> _help =
    {
      "help commands  - displays list of all available commands",
      "help <command> - displays help about <command>"
    };

    const std::unordered_map<std::string, std::vector<std::string>>
    _helpTextByCommandName =
    {
      {
        _commandNameByType.at(DevConsoleCommand::HELP),
        {
          "Not funny, didn't laugh"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CLEAR),
        {
          "Clears the screen"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CLOSE),
        {
          "Close the console"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CLOSE2),
        {
          "Close the console"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CLOSE3),
        {
          "Close the console"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CLOSE4),
        {
          "Close the console"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::HISTORY),
        {
          "Display commands history"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::REPEAT_COMMAND),
        {
          "!<N> to repeat command no. <N> in history"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::INFO_HANDLES),
        {
          "Show handlers"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::LEVEL_UP),
        {
          "Give player a level"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::LEVEL_DOWN),
        {
          "Take a level from player"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::REPORT_PLAYER),
        {
          "Print debug info about player"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::GOD_MODE),
        {
          "Toggles player's invulnerability"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::IGNORE_PLAYER),
        {
          "Make monsters ignore player"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::PRINT_MAP),
        {
          "Save current map layout to a file"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::PRINT_COLORS),
        {
          "Prints colors used so far"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::GET_BY_ADDRESS),
        {
          _commandNameByType.at(DevConsoleCommand::GET_BY_ADDRESS) + " [0x%X]",
          "Get game object by address. Print debug info if handle already set"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::GET_BY_ID),
        {
          _commandNameByType.at(DevConsoleCommand::GET_BY_ID) + " [<ID>]",
          "Get game object by global id value. "
          "Print debug info if handle already set."
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::POISON_ACTOR),
        {
          "Add lingering damage to actor in handle"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::SHOW_MAP),
        {
          "Toggle fog of war"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::PRINT_TRIGGERS),
        {
          "Print current level triggers"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::PRINT_ACTORS),
        {
          "Print current level actors"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::DISPEL_EFFECTS),
        {
          "Dispel effects from player"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::DISPEL_EFFECTS_ACTOR),
        {
          "Dispel all effects from actor in handle"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CREATE_ALL_GEMS),
        {
          "Create all gems and place them in town"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CREATE_ALL_POTIONS),
        {
          "Create all potions and place them in town"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CREATE_ALL_SCROLLS),
        {
          "Create all scrolls and place them in town"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CREATE_DUMMY_ACTOR),
        {
          _commandNameByType.at(DevConsoleCommand::CREATE_DUMMY_ACTOR) + " X Y",
          "Creates dummy actor at X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::AWARD_EXP),
        {
          _commandNameByType.at(DevConsoleCommand::AWARD_EXP) + " <AMOUNT>",
          "Give <AMOUNT> experience to player"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::GET_STATIC_OBJECT),
        {
          _commandNameByType.at(DevConsoleCommand::GET_STATIC_OBJECT) +
          " [X Y]",
          "Try to get handle to static object at X Y. "
          "Print debug info if already set."
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::GET_ACTOR),
        {
          _commandNameByType.at(DevConsoleCommand::GET_ACTOR) + " [X Y]",
          "Try to get handle to actor at X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::GET_ITEM),
        {
          _commandNameByType.at(DevConsoleCommand::GET_ITEM) + " [X Y]",
          "Try to get handle to item object at X Y. "
          "Print debug info if already set."
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::GET_MAP_OBJECT),
        {
          _commandNameByType.at(DevConsoleCommand::GET_MAP_OBJECT) + " [X Y]",
          "Try to get handle to map object at X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::GET_ANY_OBJECT),
        {
          _commandNameByType.at(DevConsoleCommand::GET_ANY_OBJECT) + " [X Y]",
          "Try to get handle to any game object at X Y. "
          "Print debug info if already set."
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::MOVE_STATIC_OBJECT),
        {
          _commandNameByType.at(DevConsoleCommand::MOVE_STATIC_OBJECT) + " X Y",
          "Try to move static object in handle to X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::MOVE_ACTOR),
        {
          _commandNameByType.at(DevConsoleCommand::MOVE_ACTOR) + " X Y",
          "Try to move actor in handle to X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::DAMAGE_ACTOR),
        {
          _commandNameByType.at(DevConsoleCommand::DAMAGE_ACTOR) +
          " N <DIRECT>",
          "Inflict N damage to actor in handle"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::MOVE_ITEM),
        {
          _commandNameByType.at(DevConsoleCommand::MOVE_ITEM) + " X Y",
          "Try to move item in handle to X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::MOVE_PLAYER),
        {
          _commandNameByType.at(DevConsoleCommand::MOVE_PLAYER) + " X Y",
          "Try to move player to X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::REMOVE_OBJECT),
        {
          _commandNameByType.at(DevConsoleCommand::REMOVE_OBJECT) + " X Y",
          "Delete any object at X Y (actor -> item -> static)"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::TRANSFORM_TILE),
        {
          _commandNameByType.at(DevConsoleCommand::TRANSFORM_TILE) +
          " X Y <TYPE>",
          "Transform tile X Y to type <TYPE>"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::PLACE_WALL),
        {
          _commandNameByType.at(DevConsoleCommand::PLACE_WALL) + " X Y",
          "Places generic wall at X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CREATE_MONSTER),
        {
          _commandNameByType.at(DevConsoleCommand::CREATE_MONSTER) +
          " X Y <TYPE>",
          "Create monster <TYPE> at X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::GIVE_MONEY),
        {
          _commandNameByType.at(DevConsoleCommand::GIVE_MONEY) + " <AMOUNT>",
          "Give player <AMOUNT> money"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CREATE_ITEM),
        {
          _commandNameByType.at(DevConsoleCommand::CREATE_ITEM) + " X Y",
          "Create random item at X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CREATE_SHRINE),
        {
          _commandNameByType.at(DevConsoleCommand::CREATE_SHRINE) +
          " X Y <TYPE>",
          "Create shrine at X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CREATE_DUMMY_OBJECT),
        {
          _commandNameByType.at(DevConsoleCommand::CREATE_DUMMY_OBJECT) +
          " X Y [<IMAGE> = 'D']",
          "Create dummy object at X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CREATE_CHEST),
        {
          _commandNameByType.at(DevConsoleCommand::CREATE_CHEST) + " X Y",
          "Create chest at X Y"
        }
      },
      {
        _commandNameByType.at(DevConsoleCommand::CREATE_BREAKABLE),
        {
          _commandNameByType.at(DevConsoleCommand::CREATE_BREAKABLE) + " X Y",
          "Create breakable object with random loot at X Y"
        }
      }
    };

    size_t _longestCommandStringLength = 0;

    Trie _trie;

    friend class GameObject;
};

#endif // DEVCONSOLE_H
