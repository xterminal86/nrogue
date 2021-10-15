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
  REMOVE_OBJECT,
  LEVEL_UP,
  LEVEL_DOWN,
  PRINT_MAP,
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

    std::vector<std::string> _history;
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

    void AddToHistory(const std::string& str);

    bool ParseCommand();

    void ProcessCommand(const std::string& command,
                        const std::vector<std::string>& params);
    void DisplayHelpAboutCommand(const std::vector<std::string>& params);
    void InfoHandles();
    void GetObject(const std::vector<std::string>& params, ObjectHandleType handleType);
    void MoveObject(const std::vector<std::string>& params, ObjectHandleType handleType);
    void RemoveObject(const std::vector<std::string>& params);

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
      { "o_del",  DevConsoleCommand::REMOVE_OBJECT      },
      { "p_lu",   DevConsoleCommand::LEVEL_UP           },
      { "p_ld",   DevConsoleCommand::LEVEL_DOWN         },
      { "g_pm",   DevConsoleCommand::PRINT_MAP          },
    };

    const std::vector<std::string> _help =
    {
      "help commands - displays list of all available commands",
      "help <command> - displays help about <command>"
    };

    const std::map<std::string, std::vector<std::string>> _helpTextByCommandName =
    {
      { "help",  { "not funny, didn't laugh"  } },
      { "clear", { "clears the screen"        } },
      { "q",     { "close the console"        } },
      { "quit",  { "close the console"        } },
      { "info",  { "show handlers"            } },
      { "exit",  { "close the console"        } },
      { "p_lu",  { "give player a level"      } },
      { "p_ld",  { "take a level from player" } },
      { "g_pm",  { "save current map layout to a file" } },
      { "so_get",
        { "so_get [X Y]", "try to get handle to static object at X Y" }
      },
      { "ao_get",
        { "ao_get [X Y]", "try to get handle to actor at X Y" }
      },
      { "io_get",
        { "io_get [X Y]", "try to get handle to item object at X Y" }
      },
      { "so_move",
        { "so_move X Y", "try to move static object in handle to X Y" }
      },
      { "ao_move",
        { "ao_move X Y", "try to move actor in handle to X Y" }
      },
      { "io_move",
        { "io_move X Y", "try to move item in handle to X Y" }
      },
      { "o_del",
        { "o_del X Y", "delete any object at X Y (actor -> item -> static)" }
      }
    };

    friend class GameObject;
};

#endif // DEVCONSOLE_H
