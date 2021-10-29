#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include "gamestate.h"

enum class DevConsoleCommand
{
  UNKNOWN = 0,
  CLEAR,
  HELP,
  CLOSE,
  GET_STATIC_OBJECT,
  GET_ACTOR,
  GET_ITEM,
  MOVE_STATIC_OBJECT,
  MOVE_ACTOR,
  MOVE_ITEM,
  MOVE_PLAYER,
  REMOVE_OBJECT,
  LEVEL_UP,
  LEVEL_DOWN,
  PRINT_MAP,
  PRINT_COLORS,
  INFO_HANDLES
};

enum ObjectHandleType
{
  STATIC = 0,
  ITEM,
  ACTOR
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

    const std::string ErrUnknownCommand = "command not found...";
    const std::string ErrSyntaxError    = "Syntax error";
    const std::string ErrWrongParams    = "Wrong params";
    const std::string ErrNoObjectsFound = "No objects found";
    const std::string ErrHandleNotSet   = "Handle not set";
    const std::string ErrCannotMove     = "Cannot move, probably occupied";

    const std::string Ok = "Ok";
    const std::string Prompt = "> ";

    const std::map<ObjectHandleType, std::string> _handleNameByType =
    {
      { ObjectHandleType::STATIC, "_static" },
      { ObjectHandleType::ACTOR,  "_actor"  },
      { ObjectHandleType::ITEM,   "_item"   }
    };

    void StdOut(const std::string& str);

    bool ParseCommand();

    void ProcessCommand(const std::string& command,
                        const std::vector<std::string>& params);
    void DisplayHelpAboutCommand(const std::vector<std::string>& params);
    void InfoHandles();
    void GetObject(const std::vector<std::string>& params, ObjectHandleType handleType);
    void MoveObject(const std::vector<std::string>& params, ObjectHandleType handleType);
    void MovePlayer(const std::vector<std::string>& params);
    void RemoveObject(const std::vector<std::string>& params);
    void PrintColors();

    bool StringIsNumbers(const std::string& str);
    std::pair<int, int> CoordinateParamsToInt(const std::string& px, const std::string& py);

    std::vector<std::string> _allCommandsList;

    const std::map<std::string, DevConsoleCommand> _commandTypeByName =
    {
      { "clear",  DevConsoleCommand::CLEAR              },
      { "help",   DevConsoleCommand::HELP               },
      { "exit",   DevConsoleCommand::CLOSE              },
      { "q",      DevConsoleCommand::CLOSE              },
      { "quit",   DevConsoleCommand::CLOSE              },
      { "info",   DevConsoleCommand::INFO_HANDLES       },
      { "so_get", DevConsoleCommand::GET_STATIC_OBJECT  },
      { "ao_get", DevConsoleCommand::GET_ACTOR          },
      { "io_get", DevConsoleCommand::GET_ITEM           },
      { "so_mov", DevConsoleCommand::MOVE_STATIC_OBJECT },
      { "ao_mov", DevConsoleCommand::MOVE_ACTOR         },
      { "io_mov", DevConsoleCommand::MOVE_ITEM          },
      { "p_mov",  DevConsoleCommand::MOVE_PLAYER        },
      { "o_del",  DevConsoleCommand::REMOVE_OBJECT      },
      { "p_lu",   DevConsoleCommand::LEVEL_UP           },
      { "p_ld",   DevConsoleCommand::LEVEL_DOWN         },
      { "g_pm",   DevConsoleCommand::PRINT_MAP          },
      { "g_pc",   DevConsoleCommand::PRINT_COLORS       }
    };

    const std::vector<std::string> _help =
    {
      "help commands  - displays list of all available commands",
      "help <command> - displays help about <command>"
    };

    const std::map<std::string, std::vector<std::string>> _helpTextByCommandName =
    {
      { "help",  { "Not funny, didn't laugh"  } },
      { "clear", { "Clears the screen"        } },
      { "q",     { "Close the console"        } },
      { "quit",  { "Close the console"        } },
      { "info",  { "Show handlers"            } },
      { "exit",  { "Close the console"        } },
      { "p_lu",  { "Give player a level"      } },
      { "p_ld",  { "Take a level from player" } },
      { "g_pm",  { "Save current map layout to a file" } },
      { "g_pc",  { "Prints colors used so far" } },
      { "so_get",
        { "so_get [X Y]", "Try to get handle to static object at X Y" }
      },
      { "ao_get",
        { "ao_get [X Y]", "Try to get handle to actor at X Y" }
      },
      { "io_get",
        { "io_get [X Y]", "Try to get handle to item object at X Y" }
      },
      { "so_mov",
        { "so_mov X Y", "Try to move static object in handle to X Y" }
      },
      { "ao_mov",
        { "ao_mov X Y", "Try to move actor in handle to X Y" }
      },
      { "io_mov",
        { "io_mov X Y", "Try to move item in handle to X Y" }
      },
      { "p_mov",
        { "p_mov X Y", "Try to move player to X Y" }
      },
      { "o_del",
        { "o_del X Y", "Delete any object at X Y (actor -> item -> static)" }
      }
    };

    friend class GameObject;
};

#endif // DEVCONSOLE_H
