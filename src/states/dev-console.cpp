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

  _objectHandles[ObjectHandleType::STATIC] = nullptr;
  _objectHandles[ObjectHandleType::ACTOR] = nullptr;
  _objectHandles[ObjectHandleType::ITEM] = nullptr;
}

void DevConsole::Prepare()
{
  _currentLevel = Map::Instance().CurrentLevel;
  _currentCommand = Prompt;
  _commandsHistoryIndex = _commandsHistory.size() - 1;
  _oldIndex = 0;
}

void DevConsole::Cleanup()
{
  _stdout.clear();
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

    case KEY_UP:
    {
      if (!_commandsHistory.empty())
      {
        _oldIndex = _commandsHistoryIndex;

        _currentCommand = Prompt + _commandsHistory[_commandsHistoryIndex];

        _commandsHistoryIndex--;
        if (_commandsHistoryIndex < 0)
        {
          _commandsHistoryIndex = 0;
        }
      }
    }
    break;

    case KEY_DOWN:
    {
      if (!_commandsHistory.empty())
      {
        _commandsHistoryIndex++;
        if (_oldIndex == _commandsHistoryIndex)
        {
          _commandsHistoryIndex++;
        }

        if (_commandsHistoryIndex > (int)_commandsHistory.size() - 1)
        {
          _currentCommand = Prompt;
          _commandsHistoryIndex = _commandsHistory.size() - 1;
        }
        else
        {
          _currentCommand = Prompt + _commandsHistory[_commandsHistoryIndex];
        }
      }
    }
    break;

    case VK_ENTER:
    {
      StdOut(_currentCommand);

      std::string noPrompt = _currentCommand.substr(2);

      bool ok = ParseCommand();

      if (!noPrompt.empty() && ok)
      {
        _commandsHistory.push_back(_currentCommand);
        _commandsHistoryIndex++;
      }

      _currentCommand = Prompt;
      _cursorX = 1;
      _cursorY++;

      while (_stdout.size() > 20)
      {
        _stdout.pop_back();
      }

      if (_commandsHistory.size() > 20)
      {
        _commandsHistory.pop_back();
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
    for (int i = _stdout.size() - 1; i >= 0; i--)
    {
      Printer::Instance().PrintFB(1, 2 + lineCount, _stdout[i], Printer::kAlignLeft, "#FFFFFF");
      lineCount++;
    }

    Printer::Instance().PrintFB(1, 2 + lineCount, _currentCommand, Printer::kAlignLeft, "#FFFFFF");
    Printer::Instance().PrintFB(1 + _currentCommand.length(), 2 + lineCount, ' ', "#000000", "#FFFFFF");

    Printer::Instance().Render();
  }
}

bool DevConsole::ParseCommand()
{
  _currentCommand.erase(0, 2);

  if (_currentCommand.find_first_not_of(' ') == std::string::npos)
  {
    _currentCommand.clear();
  }

  if (_currentCommand.empty())
  {
    return false;
  }

  while(true)
  {
    if (_currentCommand[_currentCommand.length() - 1] == ' ')
    {
      _currentCommand.pop_back();
    }
    else
    {
      break;
    }
  }

  std::vector<std::string> params = Util::StringSplit(_currentCommand, ' ');

  std::string commandEntered = params[0];
  for (auto& c : commandEntered)
  {
    c = std::tolower(c);
  }

  params.erase(params.begin());

  if (_commandTypeByName.count(commandEntered) == 0)
  {
    std::string errMsg = Util::StringFormat("%s: %s", commandEntered.data(), ErrUnknownCommand.data());
    StdOut(errMsg);
  }
  else
  {
    ProcessCommand(commandEntered, params);
  }

  return true;
}

void DevConsole::ProcessCommand(const std::string& command,
                                const std::vector<std::string>& params)
{
  DevConsoleCommand c = _commandTypeByName.at(command);

  switch (c)
  {
    case DevConsoleCommand::CLEAR:
      _stdout.clear();
      break;

    case DevConsoleCommand::HELP:
      DisplayHelpAboutCommand(params);
      break;

    case DevConsoleCommand::CLOSE:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    case DevConsoleCommand::INFO_HANDLES:
      InfoHandles();
      break;

    case DevConsoleCommand::GET_STATIC_OBJECT:
      GetObject(params, ObjectHandleType::STATIC);
      break;

    case DevConsoleCommand::GET_ITEM:
      GetObject(params, ObjectHandleType::ITEM);
      break;

    case DevConsoleCommand::GET_ACTOR:
      GetObject(params, ObjectHandleType::ACTOR);
      break;

    case DevConsoleCommand::MOVE_STATIC_OBJECT:
      MoveObject(params, ObjectHandleType::STATIC);
      break;

    case DevConsoleCommand::MOVE_ACTOR:
      MoveObject(params, ObjectHandleType::ACTOR);
      break;

    case DevConsoleCommand::MOVE_ITEM:
      MoveObject(params, ObjectHandleType::ITEM);
      break;

    case DevConsoleCommand::MOVE_PLAYER:
      MovePlayer(params);
      break;

    case DevConsoleCommand::REMOVE_OBJECT:
      RemoveObject(params);
      break;

    case DevConsoleCommand::LEVEL_UP:
      _playerRef->LevelUp();
      StdOut(Ok);
      break;

    case DevConsoleCommand::LEVEL_DOWN:
      _playerRef->LevelDown();
      StdOut(Ok);
      break;

    case DevConsoleCommand::PRINT_MAP:
      Map::Instance().PrintMapLayout();
      StdOut(Ok);
      break;

    case DevConsoleCommand::PRINT_COLORS:
      PrintColors();
      break;

    default:
      break;
  }
}

void DevConsole::PrintColors()
{
  std::string msg;

  auto& cache = Printer::Instance().GetValidColorsCache();
  for (auto& kvp : cache)
  {
#ifdef USE_SDL
    std::string toAdd =  Util::StringFormat("[%s] ", kvp.first.data());
    std::string total = msg + toAdd;
    if (total.length() > 80)
    {
      StdOut(msg);
      msg = toAdd;
    }
    else
    {
      msg += toAdd;
    }
#else
    NColor fg = kvp.second.FgColor;
    NColor bg = kvp.second.BgColor;

    auto ConvertBack = [](int nColorComponent)
    {
      float converted = ((float)nColorComponent / 1000.0f) * 255.0f;

      return (int)converted;
    };

    int cr = ConvertBack(fg.R);
    int cg = ConvertBack(fg.G);
    int cb = ConvertBack(fg.B);

    std::string r = Util::NumberToHexString(cr);
    std::string g = Util::NumberToHexString(cg);
    std::string b = Util::NumberToHexString(cb);

    std::string fgTotal = r + g + b;

    cr = ConvertBack(bg.R);
    cg = ConvertBack(bg.G);
    cb = ConvertBack(bg.B);

    r = Util::NumberToHexString(cr);
    g = Util::NumberToHexString(cg);
    b = Util::NumberToHexString(cb);

    std::string bgTotal = r + g + b;

    std::string toAdd =  Util::StringFormat("[%s|%s] ", fgTotal.data(), bgTotal.data());
    std::string total = msg + toAdd;
    if (total.length() > 80)
    {
      StdOut(msg);
      msg = toAdd;
    }
    else
    {
      msg += toAdd;
    }
#endif
  }

  StdOut(msg);
}

void DevConsole::InfoHandles()
{
  size_t maxLen = 0;
  for (auto& kvp : _handleNameByType)
  {
    if (kvp.second.length() > maxLen)
    {
      maxLen = kvp.second.length();
    }
  }

  for (auto& kvp : _handleNameByType)
  {
    std::string spaces(maxLen - kvp.second.length(), ' ');
    std::string msg = Util::StringFormat("%s%s = 0x%X", kvp.second.data(), spaces.data(), _objectHandles[kvp.first]);
    StdOut(msg);
  }
}

void DevConsole::GetObject(const std::vector<std::string>& params, ObjectHandleType handleType)
{
  if (params.size() > 0 && params.size() < 2)
  {
    StdOut(ErrWrongParams);
    return;
  }

  auto ReportHandle = [this](ObjectHandleType handleType)
  {
    std::string msg = Util::StringFormat("%s = 0x%X", _handleNameByType.at(handleType).data(), _objectHandles[handleType]);
    StdOut(msg);
  };

  if (params.empty())
  {
    ReportHandle(handleType);

    if (_objectHandles[handleType] != nullptr)
    {
      auto debugInfo = _objectHandles[handleType]->DebugInfo();
      for (auto& line : debugInfo)
      {
        StdOut(line);
      }
    }

    return;
  }

  std::string sx = params[0];
  std::string sy = params[1];

  auto r = CoordinateParamsToInt(sx, sy);
  if (r.first == -1 && r.second == -1)
  {
    return;
  }

  int x = r.first;
  int y = r.second;

  std::string msg;

  switch (handleType)
  {
    case ObjectHandleType::STATIC:
    {
      _objectHandles[handleType] = Map::Instance().GetStaticGameObjectAtPosition(x, y);
    }
    break;

    case ObjectHandleType::ITEM:
    {
      auto res = Map::Instance().GetGameObjectsAtPosition(x, y);
      if (!res.empty())
      {
        _objectHandles[handleType] = res.back();
      }
    }
    break;

    case ObjectHandleType::ACTOR:
    {
      _objectHandles[handleType] = Map::Instance().GetActorAtPosition(x, y);
    }
    break;
  }

  if (_objectHandles[handleType] == nullptr)
  {
    StdOut(ErrNoObjectsFound);
  }
  else
  {
    ReportHandle(handleType);
  }
}

void DevConsole::MoveObject(const std::vector<std::string>& params, ObjectHandleType handleType)
{
  if (_objectHandles[handleType] == nullptr)
  {
    StdOut(ErrHandleNotSet);
    return;
  }

  if (params.size() != 2)
  {
    StdOut(ErrWrongParams);
    return;
  }

  std::string sx = params[0];
  std::string sy = params[1];

  auto r = CoordinateParamsToInt(sx, sy);
  if (r.first == -1 && r.second == -1)
  {
    return;
  }

  int x = r.first;
  int y = r.second;

  switch (handleType)
  {
    case ObjectHandleType::STATIC:
    {
      GameObject* go = Map::Instance().GetStaticGameObjectAtPosition(x, y);
      if (go != nullptr)
      {
        StdOut(ErrCannotMove);
        return;
      }

      int mx = _objectHandles[handleType]->PosX;
      int my = _objectHandles[handleType]->PosY;

      GameObject* obj = _currentLevel->StaticMapObjects[mx][my].release();

      _currentLevel->StaticMapObjects[x][y].reset(obj);

      _currentLevel->StaticMapObjects[x][y]->PosX = x;
      _currentLevel->StaticMapObjects[x][y]->PosY = y;

      _objectHandles[handleType] = _currentLevel->StaticMapObjects[x][y].get();
    }
    break;

    case ObjectHandleType::ITEM:
    case ObjectHandleType::ACTOR:
    {
      bool succ = _objectHandles[handleType]->MoveTo({ x, y });
      if (!succ)
      {
        StdOut(ErrCannotMove);
        return;
      }
    }
    break;
  }

  StdOut(Ok);
}

void DevConsole::MovePlayer(const std::vector<std::string>& params)
{
  if (params.size() != 2)
  {
    StdOut(ErrWrongParams);
    return;
  }

  std::string sx = params[0];
  std::string sy = params[1];

  auto r = CoordinateParamsToInt(sx, sy);
  if (r.first == -1 && r.second == -1)
  {
    return;
  }

  int x = r.first;
  int y = r.second;

  bool succ = _playerRef->MoveTo(x, y);

  if (succ)
  {
    StdOut(Ok);
    _currentLevel->AdjustCamera();
  }
  else
  {
    StdOut(ErrCannotMove);
  }
}

void DevConsole::RemoveObject(const std::vector<std::string>& params)
{
  if (params.size() != 2)
  {
    StdOut(ErrWrongParams);
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

  GameObject* go = Map::Instance().GetActorAtPosition(x, y);
  if (go == nullptr)
  {
    std::vector<GameObject*> res = Map::Instance().GetGameObjectsAtPosition(x, y);
    if (res.empty())
    {
      go = Map::Instance().GetStaticGameObjectAtPosition(x, y);
      if (go == nullptr)
      {
        StdOut(ErrNoObjectsFound);
        return;
      }
    }
    else
    {
      go = res.back();
    }
  }

  if (go != nullptr)
  {
    go->IsDestroyed = true;
  }

  Map::Instance().RemoveDestroyed();

  StdOut(Ok);
}

void DevConsole::DisplayHelpAboutCommand(const std::vector<std::string>& params)
{
  if (params.empty())
  {
    for (auto& line : _help)
    {
      StdOut(line);
    }
  }
  else
  {
    if (_helpTextByCommandName.count(params[0]) == 1)
    {
      for (auto& line : _helpTextByCommandName.at(params[0]))
      {
        StdOut(line);
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
          StdOut(totalString);
          totalString.clear();
          count = 0;
        }

        totalString += t;
        totalString += " ";
      }

      StdOut(totalString);
    }
    else
    {
      std::string msg = Util::StringFormat("No help found for '%s'", params[0].data());
      StdOut(msg);
    }
  }
}

void DevConsole::StdOut(const std::string& str)
{
  _stdout.insert(_stdout.begin(), str);
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
    StdOut("X and Y must be numbers");
    return res;
  }

  res.first  = std::stoi(px);
  res.second = std::stoi(py);

  if (res.first < 1 || res.first > _currentLevel->MapSize.X - 1
   || res.second < 1 || res.second > _currentLevel->MapSize.Y - 1)
  {
    StdOut("Out of bounds");
    res = { -1, -1 };
  }

  return res;
}
