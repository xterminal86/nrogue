#include "dev-console.h"

#include "application.h"
#include "printer.h"
#include "game-objects-factory.h"
#include "map.h"
#include "util.h"

void DevConsole::Init()
{
  for (auto& kvp : _commandTypeByName)
  {
    _allCommandsList.push_back(kvp.first);
  }

  _playerRef = &Application::Instance().PlayerInstance;
}

void DevConsole::Prepare()
{
  _currentLevel = Map::Instance().CurrentLevel;

  _currentCommand = Prompt;
}

void DevConsole::Cleanup()
{
  _history.clear();
  _currentCommand.clear();
}

void DevConsole::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case '`':
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    case VK_ENTER:
    {
      AddToHistory(_currentCommand);

      ParseCommand();

      _currentCommand = Prompt;
      _cursorX = 1;
      _cursorY++;

      while (_history.size() > 20)
      {
        _history.pop_back();
      }
    }
    break;

    case VK_BACKSPACE:
    {
      if (_currentCommand.length() > 2)
      {
        _currentCommand.pop_back();
      }
    }
    break;

    default:
    {
      if (_keyPressed >= 32 && _keyPressed <= 126)
      {
        _currentCommand += (char)_keyPressed;
      }
    }
    break;
  }
}

void DevConsole::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawHeader(" DEVELOPER'S CONSOLE ");

    int lineCount = 0;
    for (int i = _history.size() - 1; i >= 0; i--)
    {
      Printer::Instance().PrintFB(1, 2 + lineCount, _history[i], Printer::kAlignLeft, "#FFFFFF");
      lineCount++;
    }

    Printer::Instance().PrintFB(1, 2 + lineCount, _currentCommand, Printer::kAlignLeft, "#FFFFFF");
    Printer::Instance().PrintFB(1 + _currentCommand.length(), 2 + lineCount, ' ', "#000000", "#FFFFFF");

    Printer::Instance().Render();
  }
}

void DevConsole::ParseCommand()
{
  std::string entered = _currentCommand.erase(0, 2);
  if (entered.empty())
  {
    return;
  }

  std::vector<std::string> params = Util::StringSplit(entered, ' ');

  std::string commandEntered = params[0];
  for (auto& c : commandEntered)
  {
    c = std::tolower(c);
  }

  params = Util::StringSplit(entered, ' ');
  params.erase(params.begin());

  if (_commandTypeByName.count(commandEntered) == 0)
  {
    std::string errMsg = Util::StringFormat("%s: %s", commandEntered.data(), ErrUnknownCommand.data());
    AddToHistory(errMsg);
  }
  else
  {
    ProcessCommand(commandEntered, params);
  }
}

void DevConsole::ProcessCommand(const std::string& command,
                                const std::vector<std::string>& params)
{
  DevConsoleCommand c = _commandTypeByName.at(command);

  switch (c)
  {
    case DevConsoleCommand::CLEAR:
      _history.clear();
      break;

    case DevConsoleCommand::HELP:
      DisplayHelpAboutCommand(params);
      break;

    case DevConsoleCommand::CLOSE:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    case DevConsoleCommand::CREATE_OBJECT:
      CreateObject(params);
      break;

    case DevConsoleCommand::REMOVE_OBJECT:
      RemoveObject(params);
      break;

    case DevConsoleCommand::CREATE_ACTOR:
      CreateActor(params);
      break;

    case DevConsoleCommand::REMOVE_ACTOR:
      RemoveActor(params);
      break;

    case DevConsoleCommand::CREATE_ITEM:
      CreateItem(params);
      break;

    case DevConsoleCommand::LEVEL_UP:
      _playerRef->LevelUp();
      AddToHistory(Ok);
      break;

    case DevConsoleCommand::LEVEL_DOWN:
      _playerRef->LevelDown();
      AddToHistory(Ok);
      break;

    case DevConsoleCommand::PRINT_MAP:
      Map::Instance().PrintMapLayout();
      break;

    default:
      break;
  }
}

void DevConsole::CreateObject(const std::vector<std::string>& params)
{
  if (params.size() != 3)
  {
    AddToHistory(ErrWrongParams);
    return;
  }

  std::string sx = params[0];
  std::string sy = params[1];

  auto r = CoordinateParamsToInt(sx, sy);
  if (r.first == -1 && r.second == -1)
  {
    return;
  }

  std::string image = params[2];

  if (image.length() > 1)
  {
    AddToHistory("IMAGE must be a char");
    return;
  }

  int x = r.first;
  int y = r.second;

  char img = image[0];

  GameObject* go = GameObjectsFactory::Instance().CreateDummyObject(x, y, "DUMMY", img, Colors::WhiteColor, Colors::MagentaColor);

  go->Blocking    = true;
  go->BlocksSight = true;

  _currentLevel->InsertStaticObject(go);

  AddToHistory(Ok);
}

void DevConsole::RemoveObject(const std::vector<std::string>& params)
{
  if (params.size() != 2)
  {
    AddToHistory(ErrWrongParams);
    return;
  }

  std::string sx = params[0];
  std::string sy = params[1];

  auto r = CoordinateParamsToInt(sx, sy);
  if (r.first == -1 || r.second == -1)
  {
    return;
  }

  int x = r.first;
  int y = r.second;

  std::vector<GameObject*> res = Map::Instance().GetGameObjectsAtPosition(x, y);
  if (res.empty())
  {
    GameObject* so = Map::Instance().GetStaticGameObjectAtPosition(x, y);
    if (so == nullptr)
    {
      AddToHistory("No objects found");
      return;
    }

    so->IsDestroyed = true;
  }
  else
  {
    res[0]->IsDestroyed = true;
  }

  Map::Instance().RemoveDestroyed();

  AddToHistory(Ok);
}

void DevConsole::CreateActor(const std::vector<std::string>& params)
{
}

void DevConsole::RemoveActor(const std::vector<std::string>& params)
{
  if (params.size() != 2)
  {
    AddToHistory(ErrWrongParams);
    return;
  }

  std::string sx = params[0];
  std::string sy = params[1];

  auto r = CoordinateParamsToInt(sx, sy);
  if (r.first == -1 || r.second == -1)
  {
    return;
  }

  int x = r.first;
  int y = r.second;

  GameObject* actor = Map::Instance().GetActorAtPosition(x, y);
  if (actor == nullptr)
  {
    AddToHistory("No actor found");
    return;
  }

  actor->IsDestroyed = true;

  Map::Instance().RemoveDestroyed();

  AddToHistory(Ok);
}

void DevConsole::CreateItem(const std::vector<std::string>& params)
{
  /*
  if (params.size() != 3)
  {
    AddToHistory(ErrWrongParams);
    return;
  }

  std::string sx = params[0];
  std::string sy = params[1];

  auto r = CoordinateParamsToInt(sx, sy);
  if (r.first == -1 && r.second == -1)
  {
    return;
  }

  std::string type = params[2];

  if (image.length() > 1)
  {
    AddToHistory("IMAGE must be a char");
    return;
  }

  int x = r.first;
  int y = r.second;
  */

}

void DevConsole::DisplayHelpAboutCommand(const std::vector<std::string>& params)
{
  if (params.empty())
  {
    for (auto& line : _help)
    {
      AddToHistory(line);
    }
  }
  else
  {
    if (_helpTextByCommandName.count(params[0]) == 1)
    {
      for (auto& line : _helpTextByCommandName.at(params[0]))
      {
        AddToHistory(line);
      }
    }
    else if (params[0] == "commands")
    {
      size_t count = 0;
      std::string totalString;
      for (auto& t : _allCommandsList)
      {
        count += (t.length() + 1);
        if (count > 79)
        {
          AddToHistory(totalString);
          totalString.clear();
          count = 0;
        }

        totalString += t;
        totalString += " ";
      }

      AddToHistory(totalString);
    }
    else
    {
      std::string msg = Util::StringFormat("No help found for '%s'", params[0].data());
      AddToHistory(msg);
    }
  }
}

void DevConsole::AddToHistory(const std::string& str)
{
  _history.insert(_history.begin(), str);
}

bool DevConsole::StringIsNumbers(const std::string& str)
{
  for (auto& c : str)
  {
    auto res = std::find(GlobalConstants::Numbers.begin(),
                         GlobalConstants::Numbers.end(),
                         c);

    if (res == GlobalConstants::Numbers.end())
    {
      return false;
    }
  }

  return true;
}

std::pair<int, int> DevConsole::CoordinateParamsToInt(const std::string &px, const std::string &py)
{
  std::pair<int, int> res = { -1, -1 };

  if (!StringIsNumbers(px) || !StringIsNumbers(py))
  {
    AddToHistory("X and Y must be numbers");
    return res;
  }

  res.first  = std::stoi(px);
  res.second = std::stoi(py);

  if (res.first < 1 || res.first > _currentLevel->MapSize.X - 1
   || res.second < 1 || res.second > _currentLevel->MapSize.Y - 1)
  {
    AddToHistory("Out of bounds");
    res = { -1, -1 };
  }

  return res;
}
