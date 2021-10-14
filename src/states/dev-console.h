#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include "gamestate.h"

enum class DevConsoleCommand
{
  UNKNOWN = 0,
  CLEAR,
  HELP,
  CLOSE,
  CREATE_OBJECT,
  REMOVE_OBJECT,
  CREATE_ITEM,
  CREATE_ACTOR,
  REMOVE_ACTOR,
  LEVEL_UP,
  LEVEL_DOWN,
  PRINT_MAP
};

class MapLevelBase;
class Player;

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

    std::string _currentCommand;

    std::vector<std::string> _history;

    int _cursorX = 1;
    int _cursorY = 2;

    const std::string ErrUnknownCommand = "command not found...";
    const std::string ErrSyntaxError = "Syntax error";
    const std::string ErrWrongParams = "Wrong params";
    const std::string Ok = "Ok";
    const std::string Prompt = "> ";

    void AddToHistory(const std::string& str);

    void ParseCommand();
    void ProcessCommand(const std::string& command,
                        const std::vector<std::string>& params);
    void DisplayHelpAboutCommand(const std::vector<std::string>& params);
    void CreateObject(const std::vector<std::string>& params);
    void CreateItem(const std::vector<std::string>& params);
    void RemoveObject(const std::vector<std::string>& params);
    void CreateActor(const std::vector<std::string>& params);
    void RemoveActor(const std::vector<std::string>& params);

    bool StringIsNumbers(const std::string& str);
    std::pair<int, int> CoordinateParamsToInt(const std::string& px, const std::string& py);

    std::vector<std::string> _allCommandsList;

    const std::map<std::string, DevConsoleCommand> _commandTypeByName =
    {
      { "clear", DevConsoleCommand::CLEAR         },
      { "help",  DevConsoleCommand::HELP          },
      { "exit",  DevConsoleCommand::CLOSE         },
      { "q",     DevConsoleCommand::CLOSE         },
      { "quit",  DevConsoleCommand::CLOSE         },
      { "wco",   DevConsoleCommand::CREATE_OBJECT },
      { "wro",   DevConsoleCommand::REMOVE_OBJECT },
      { "wci",   DevConsoleCommand::CREATE_ITEM   },
      { "wca",   DevConsoleCommand::CREATE_ACTOR  },
      { "wra",   DevConsoleCommand::REMOVE_ACTOR  },
      { "plu",   DevConsoleCommand::LEVEL_UP      },
      { "pld",   DevConsoleCommand::LEVEL_DOWN    },
      { "gpm",   DevConsoleCommand::PRINT_MAP     },
    };

    const std::vector<std::string> _help =
    {
      "help commands - displays list of all available commands",
      "help <command> - displays help about <command>"
    };

    const std::map<std::string, std::vector<std::string>> _helpTextByCommandName =
    {
      {
        "help",
        {
          "not funny, didn't laugh"
        }
      },
      {
        "clear",
        {
          "clears the screen"
        }
      },
      {
        "wco",
        {
          "wco X Y IMAGE",
          "create static world object at [X;Y]"
        }
      },
      {
        "wro",
        {
          "wro X Y",
          "remove any game object at [X;Y]"
        }
      },
      {
        "wci",
        {
          "wci X Y TYPE",
          "create item at [X;Y]"
        }
      },
      {
        "wca",
        {
          "wca X Y TYPE",
          "create actor at [X;Y]"
        }
      },
      {
        "wra",
        {
          "wca X Y",
          "remove actor at [X;Y]"
        }
      },
      {
        "q",
        {
          "close the console"
        }
      },
      {
        "quit",
        {
          "close the console"
        }
      },
      {
        "exit",
        {
          "close the console"
        }
      },
      {
        "plu",
        {
          "give player a level"
        }
      },
      {
        "pld",
        {
          "take a level from player"
        }
      },
      {
        "gpm",
        {
          "save current map layout to a file"
        }
      },
    };
};

#endif // DEVCONSOLE_H
