#include "dev-console.h"

#include "application.h"
#include "printer.h"
#include "game-objects-factory.h"
#include "items-factory.h"
#include "monsters-inc.h"
#include "map.h"
#include "util.h"
#include "game-object-info.h"

void DevConsole::Init()
{
  for (auto& kvp : _commandTypeByName)
  {
    _allCommandsList.push_back(kvp.first);
  }

  _playerRef = &Application::Instance().PlayerInstance;

  _objectHandles[ObjectHandleType::STATIC] = nullptr;
  _objectHandles[ObjectHandleType::ACTOR]  = nullptr;
  _objectHandles[ObjectHandleType::ITEM]   = nullptr;
  _objectHandles[ObjectHandleType::MAP]    = nullptr;
  _objectHandles[ObjectHandleType::ANY]    = nullptr;

  StdOut("Copyright (C) 1992, Lance Wilson Productions");
  StdOut("\"Ryder, nigga!\"");
  StdOut("");
  StdOut("Type 'help commands' for a list of available commands");
}

// =============================================================================

void DevConsole::Prepare()
{
  _closedByCommand = false;
  _currentLevel = Map::Instance().CurrentLevel;
  _currentCommand = Prompt;
  _commandsHistoryIndex = _commandsHistory.size();
}

// =============================================================================

void DevConsole::Cleanup()
{
  //_stdout.clear();
  _currentCommand.clear();
}

// =============================================================================

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
        if (_commandsHistoryIndex > 0)
        {
          _commandsHistoryIndex--;
        }

        _currentCommand = Prompt + _commandsHistory[_commandsHistoryIndex];
      }
    }
    break;

    case KEY_DOWN:
    {
      if (!_commandsHistory.empty())
      {
        if (_commandsHistoryIndex < (int)_commandsHistory.size() - 1)
        {
          _commandsHistoryIndex++;
          _currentCommand = Prompt + _commandsHistory[_commandsHistoryIndex];
        }
        else
        {
          _commandsHistoryIndex = _commandsHistory.size();
          _currentCommand = Prompt;
        }
      }
    }
    break;

    case VK_ENTER:
    {
      StdOut(_currentCommand);

      std::string noPrompt = _currentCommand.substr(2);

      bool ok = ParseCommand();

      //
      // Don't add console closing command to history
      // because it will in fact be added as empty string
      // due to indirect calling of Cleanup() via ChangeState()
      // inside ProcessCommand().
      //
      if (_closedByCommand)
      {
        return;
      }

      if (!noPrompt.empty() && ok)
      {
        _commandsHistory.push_back(_currentCommand);
        _commandsHistoryIndex = _commandsHistory.size();
      }

      _currentCommand = Prompt;
      _cursorX = 1;
      _cursorY++;

      while (_stdout.size() > _maxHistory)
      {
        _stdout.pop_back();
      }

      if (_commandsHistory.size() > _maxHistory)
      {
        _commandsHistory.erase(_commandsHistory.begin());
        _commandsHistoryIndex = _commandsHistory.size();
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

// =============================================================================

void DevConsole::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawHeader(" DEVELOPER'S CONSOLE ");

    int lineCount = 0;
    for (int i = _stdout.size() - 1; i >= 0; i--)
    {
      Printer::Instance().PrintFB(1,
                                  2 + lineCount,
                                  _stdout[i],
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor);
      lineCount++;
    }

    Printer::Instance().PrintFB(1,
                                2 + lineCount,
                                _currentCommand,
                                Printer::kAlignLeft,
                                Colors::WhiteColor);

    Printer::Instance().PrintFB(1 + _currentCommand.length(),
                                2 + lineCount,
                                ' ',
                                Colors::BlackColor,
                                Colors::WhiteColor);

    /*
    std::string toPrint = Util::StringFormat("_commandsHistoryIndex = %d",
                                             _commandsHistoryIndex);
    Printer::Instance().PrintFB(_tw - 1,
                                3,
                                toPrint,
                                Printer::kAlignRight,
                                Colors::WhiteColor);
    */

    Printer::Instance().Render();
  }
}

// =============================================================================

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

  bool specialCase = (commandEntered[0] == '!');
  if (specialCase)
  {
    RepeatCommand(commandEntered);
  }
  else
  {
    if (_commandTypeByName.count(commandEntered) == 0)
    {
      std::string errMsg = Util::StringFormat(ErrUnknownCommand,
                                              commandEntered.data());
      StdOut(errMsg);
    }
    else
    {
      ProcessCommand(commandEntered, params);
    }
  }

  return true;
}

// =============================================================================

void DevConsole::ProcessCommand(const std::string& command,
                                const std::vector<std::string>& params)
{
  DevConsoleCommand c = _commandTypeByName.at(command);

  switch (c)
  {
    // ------------------ shell builtins ---------------------------------------

    case DevConsoleCommand::CLEAR:
      _stdout.clear();
      break;

    case DevConsoleCommand::HELP:
      DisplayHelpAboutCommand(params);
      break;

    case DevConsoleCommand::CLOSE:
      _closedByCommand = true;
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    case DevConsoleCommand::HISTORY:
      PrintHistory();
      break;

    // -------------------------------------------------------------------------

    case DevConsoleCommand::INFO_HANDLES:
      InfoHandles();
      break;

    case DevConsoleCommand::GET_BY_ADDRESS:
      GetObjectByAddress(params);
      break;

    case DevConsoleCommand::GET_BY_ID:
      GetObjectById(params);
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

    case DevConsoleCommand::GET_MAP_OBJECT:
      GetObject(params, ObjectHandleType::MAP);
      break;

    case DevConsoleCommand::GET_ANY_OBJECT:
      GetObject(params, ObjectHandleType::ANY);
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
    {
      int expToGive = _playerRef->Attrs.Exp.Max().Get() - _playerRef->Attrs.Exp.Min().Get();
      _playerRef->AwardExperience(expToGive);
      StdOut(Ok);
    }
    break;

    case DevConsoleCommand::AWARD_EXP:
      AwardExperience(params);
      break;

    case DevConsoleCommand::LEVEL_DOWN:
    {
      if (_playerRef->Attrs.Lvl.Get() == 1)
      {
        StdOut(ErrCantLevelDown);
      }
      else
      {
        int expToTake = _playerRef->Attrs.Exp.Min().Get() + 1;
        _playerRef->AwardExperience(-expToTake);
        StdOut(Ok);
      }
    }
    break;

    case DevConsoleCommand::PRINT_MAP:
      Map::Instance().PrintMapLayout();
      StdOut(Ok);
      break;

    case DevConsoleCommand::PRINT_COLORS:
      PrintColors();
      break;

    case DevConsoleCommand::REPORT_PLAYER:
    {
      auto lines = _playerRef->DebugInfo();
      PrintDebugInfo(lines);
    }
    break;

    case DevConsoleCommand::TRANSFORM_TILE:
      TransformTile(params);
      break;

    case DevConsoleCommand::PLACE_WALL:
      PlaceWall(params);
      break;

    case DevConsoleCommand::CREATE_MONSTER:
      CreateMonster(params);
      break;

    case DevConsoleCommand::CREATE_DUMMY_ACTOR:
      CreateDummyActor(params);
      break;

    case DevConsoleCommand::CREATE_ALL_GEMS:
      CreateAllGems();
      break;

    case DevConsoleCommand::CREATE_ALL_POTIONS:
      CreateAllPotions();
      break;

    case DevConsoleCommand::CREATE_ALL_SCROLLS:
      CreateAllScrolls();
      break;

    case DevConsoleCommand::CREATE_ITEM:
      CreateItem(params);
      break;

    case DevConsoleCommand::DAMAGE_ACTOR:
      DamageActor(params);
      break;

    case DevConsoleCommand::POISON_ACTOR:
      PoisonActor();
      break;

    case DevConsoleCommand::GIVE_MONEY:
      GiveMoney(params);
      break;

    case DevConsoleCommand::SHOW_MAP:
      ToggleFogOfWar();
      break;

    case DevConsoleCommand::GOD_MODE:
      ToggleGodMode();
      break;

    case DevConsoleCommand::PRINT_TRIGGERS:
      PrintTriggers();
      break;

    case DevConsoleCommand::PRINT_ACTORS:
      PrintActors();
      break;

    case DevConsoleCommand::DISPEL_EFFECTS:
      DispelEffects();
      break;

    case DevConsoleCommand::DISPEL_EFFECTS_ACTOR:
      DispelEffectsActor();
      break;

    default:
      StdOut(ErrCmdNotHandled);
      break;
  }
}

// =============================================================================

void DevConsole::GetObjectByAddress(const std::vector<std::string>& params)
{
  if (params.size() > 1)
  {
    StdOut(ErrWrongParams);
    return;
  }

  if (params.size() == 0)
  {
    ReportHandle(ObjectHandleType::ANY);

    if (_objectHandles[ObjectHandleType::ANY] != nullptr)
    {
      ReportHandleDebugInfo(ObjectHandleType::ANY);
    }

    return;
  }

  std::string str = params[0];

  auto res = Util::StringSplit(str, 'x');
  if (res.size() == 1 || res.size() > 2)
  {
    StdOut(ErrWrongParams);
    return;
  }

  std::string addr = res[1];
  std::transform(addr.begin(),
                 addr.end(),
                 addr.begin(),
                 [](unsigned char c)
                 {
                   return std::toupper(c);
                 });

  for (auto& c : addr)
  {
    auto found = std::find(Strings::HexChars.begin(),
                           Strings::HexChars.end(),
                           c);

    if (found == Strings::HexChars.end())
    {
      StdOut(ErrWrongParams);
      return;
    }
  }

  std::string hexAddr = "0x" + addr;

  _objectHandles[ObjectHandleType::ANY] = _currentLevel->FindObjectByAddress(hexAddr);

  ReportHandle(ObjectHandleType::ANY);
  ReportHandleDebugInfo(ObjectHandleType::ANY);
}

// =============================================================================

void DevConsole::GetObjectById(const std::vector<std::string>& params)
{
  if (params.size() > 1)
  {
    StdOut(ErrWrongParams);
    return;
  }

  if (params.size() == 0)
  {
    ReportHandle(ObjectHandleType::ANY);

    if (_objectHandles[ObjectHandleType::ANY] != nullptr)
    {
      ReportHandleDebugInfo(ObjectHandleType::ANY);
    }

    return;
  }

  std::string str = params[0];

  if (!StringIsNumbers(str))
  {
    StdOut(ErrWrongParams);
    return;
  }

  uint64_t id = std::stoull(str);

  if (GameObjectsById.count(id) == 1)
  {
    _objectHandles[ObjectHandleType::ANY] = GameObjectsById[id];
    ReportHandle(ObjectHandleType::ANY);
  }
  else
  {
    StdOut(ErrNoObjectsFound);
  }
}

// =============================================================================

void DevConsole::ReportHandleDebugInfo(ObjectHandleType type)
{
  if (_objectHandles[type] != nullptr)
  {
    auto lines = _objectHandles[type]->DebugInfo();
    PrintDebugInfo(lines);
  }
}

// =============================================================================

void DevConsole::PrintDebugInfo(const std::vector<std::string>& debugInfo)
{
  for (auto& l : debugInfo)
  {
    StdOut(l);
  }
}

// =============================================================================

void DevConsole::TransformTile(const std::vector<std::string>& params)
{
  if (params.size() < 3)
  {
    StdOut(ErrWrongParams);
    return;
  }

  std::string sx = params[0];
  std::string sy = params[1];

  auto r = CoordinateParamsToInt(sx, sy);
  if (r.first == -1 && r.second == -1)
  {
    StdOut(ErrWrongParams);
    return;
  }

  int x = r.first;
  int y = r.second;

  std::string tileType = params[2];
  if (!StringIsNumbers(tileType))
  {
    StdOut(ErrInvalidType);
    return;
  }

  int tileTypeInt = std::stoi(tileType);
  GameObjectType newTileType = (GameObjectType)tileTypeInt;

  bool found = (_validTileTransformTypes.count(newTileType) == 1);

  if (!found)
  {
    StdOut(ErrInvalidType);
    return;
  }

  _currentLevel->MapArray[x][y]->Type = newTileType;

  switch (newTileType)
  {
    case GameObjectType::GROUND:
      _currentLevel->PlaceGroundTile(x, y, '.', Colors::BlackColor, Colors::DirtColor, "Ground");
      break;

    case GameObjectType::LAVA:
      _currentLevel->PlaceLavaTile(x, y);
      break;

    case GameObjectType::SHALLOW_WATER:
      _currentLevel->PlaceShallowWaterTile(x, y);
      break;

    case GameObjectType::DEEP_WATER:
      _currentLevel->PlaceDeepWaterTile(x, y);
      break;

    case GameObjectType::CHASM:
      _currentLevel->PlaceChasmTile(x, y);
      break;
  }

  StdOut(Ok);
}

// =============================================================================

void DevConsole::PlaceWall(const std::vector<std::string>& params)
{
  if (params.size() < 2)
  {
    StdOut(ErrWrongParams);
    return;
  }

  std::string sx = params[0];
  std::string sy = params[1];

  auto r = CoordinateParamsToInt(sx, sy);
  if (r.first == -1 && r.second == -1)
  {
    StdOut(ErrWrongParams);
    return;
  }

  int x = r.first;
  int y = r.second;

  GameObject* wall = new GameObject(_currentLevel, x, y, '#', Colors::WhiteColor, Colors::MagentaColor);
  wall->ObjectName = "Dev Wall";
  wall->Blocking = true;
  wall->BlocksSight = true;
  wall->Type = GameObjectType::PICKAXEABLE;
  _currentLevel->PlaceStaticObject(wall);

  StdOut(Ok);
}

// =============================================================================

void DevConsole::PrintColors()
{
  std::string msg;

  auto& cache = Printer::Instance().GetValidColorsCache();
  for (auto& kvp : cache)
  {
#ifdef USE_SDL
    std::string toAdd =  Util::StringFormat("[%06X] ", kvp.first);
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
      double converted = ((double)nColorComponent / 1000.0) * 255.0;

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

// =============================================================================

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
    std::string msg = Util::StringFormat("%s%s = 0x%X",
                                         kvp.second.data(),
                                         spaces.data(),
                                         _objectHandles[kvp.first]);
    StdOut(msg);
  }
}

// =============================================================================

void DevConsole::CreateDummyActor(const std::vector<std::string>& params)
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

  GameObject* actor = MonstersInc::Instance().CreateNPC(x,
                                                        y,
                                                        NPCType::UNDEFINED);

  Map::Instance().CurrentLevel->PlaceActor(actor);

  StdOut(Ok);
}

// =============================================================================

void DevConsole::CreateMonster(const std::vector<std::string>& params)
{
  if (params.size() != 3)
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

  std::string monsterIndex = params[2];
  if (!StringIsNumbers(monsterIndex))
  {
    StdOut(ErrWrongParams);
    return;
  }

  int monsterInd = std::stoi(monsterIndex);
  GameObjectType objType = (GameObjectType)monsterInd;

  bool found = (_monsters.count(objType) == 1);
  if (!found)
  {
    StdOut(ErrWrongParams);
    return;
  }

  auto go = MonstersInc::Instance().CreateMonster(x, y, objType);
  _currentLevel->PlaceActor(go);

  StdOut(Ok);
}

// =============================================================================

void DevConsole::CreateAllGems()
{
  auto map = GlobalConstants::GemNameByType;
  for (int i = 0; i < 2; i++)
  {
    int count = 0;
    for (auto& kvp : map)
    {
      int yOffset = (i == 0) ? 0 : 1;

      auto go = ItemsFactory::Instance().CreateGem(1 + count, 9 + yOffset,
                                                   kvp.first,
                                                   100,
                                                   ItemQuality::RANDOM);
      ItemComponent* ic = go->GetComponent<ItemComponent>();
      ic->Data.IsIdentified = (i == 0) ? true : false;
      _currentLevel->PlaceGameObject(go);
      count++;
    }
  }

  StdOut(Ok);
}

// =============================================================================

void DevConsole::CreateAllPotions()
{
  int count = 0;
  auto map = GlobalConstants::PotionNameByType;
  for (auto& kvp : map)
  {
    auto go = ItemsFactory::Instance().CreatePotion(kvp.first);
    go->PosX = 1 + count;
    go->PosY = 1;
    ItemComponent* ic = go->GetComponent<ItemComponent>();
    ic->Data.IsIdentified = false;
    _currentLevel->PlaceGameObject(go);
    count++;
  }

  StdOut(Ok);
}

// =============================================================================

void DevConsole::CreateAllScrolls()
{
  std::vector<ItemPrefix> prefixes =
  {
    ItemPrefix::CURSED,
    ItemPrefix::UNCURSED,
    ItemPrefix::BLESSED
  };

  for (int i = 0; i < 3; i++)
  {
    ItemPrefix p = prefixes[i];

    int xOffset = 0;

    for (auto& item : GlobalConstants::ScrollValidSpellTypes)
    {
      auto scroll = ItemsFactory::Instance().CreateScroll(1 + xOffset, 10 + i, item, p);
      ItemComponent* ic = scroll->GetComponent<ItemComponent>();
      ic->Data.IsIdentified = true;
      _currentLevel->PlaceGameObject(scroll);
      xOffset++;
    }
  }

  StdOut(Ok);
}

// =============================================================================

void DevConsole::CreateItem(const std::vector<std::string>& params)
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

  GameObject* go = ItemsFactory::Instance().CreateRandomItem(x, y);
  if (go == nullptr)
  {
    StdOut("Generated object type is not implemented yet!");
    return;
  }

  _currentLevel->PlaceGameObject(go);

  StdOut(Ok);
}

// =============================================================================

void DevConsole::GetObject(const std::vector<std::string>& params, ObjectHandleType handleType)
{
  if (params.size() > 0 && params.size() < 2)
  {
    StdOut(ErrWrongParams);
    return;
  }

  if (params.empty())
  {
    ReportHandle(handleType);
    ReportHandleDebugInfo(handleType);
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
      _objectHandles[handleType] = Map::Instance().GetStaticGameObjectAtPosition(x, y);
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
      _objectHandles[handleType] = Map::Instance().GetActorAtPosition(x, y);
      break;

    case ObjectHandleType::MAP:
      _objectHandles[handleType] = Map::Instance().GetMapObjectAtPosition(x, y);
      break;

    case ObjectHandleType::ANY:
    {
      GameObject* res = nullptr;

      res = Map::Instance().GetActorAtPosition(x, y);

      if (res == nullptr)
      {
        auto vector = Map::Instance().GetGameObjectsAtPosition(x, y);
        if (vector.empty())
        {
          res = Map::Instance().GetStaticGameObjectAtPosition(x, y);
          if (res == nullptr)
          {
            res = Map::Instance().GetMapObjectAtPosition(x, y);
          }
        }
        else
        {
          res = vector.back();
        }
      }

      _objectHandles[handleType] = res;
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

// =============================================================================

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

// =============================================================================

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

// =============================================================================

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

// =============================================================================

void DevConsole::DamageActor(const std::vector<std::string>& params)
{
  if (params.size() != 1)
  {
    StdOut(ErrWrongParams);
    return;
  }

  if (_objectHandles[ObjectHandleType::ACTOR] == nullptr)
  {
    StdOut(ErrHandleNotSet);
    return;
  }

  std::string n = params[0];
  if (!StringIsNumbers(n))
  {
    StdOut(ErrSyntaxError);
    return;
  }

  bool isDirect = false;
  if (params.size() == 2)
  {
    isDirect = params[1].empty();
  }

  int dmg = std::stoi(n);
  _objectHandles[ObjectHandleType::ACTOR]->ReceiveDamage(nullptr, dmg, true, true, isDirect, false);

  Map::Instance().RemoveDestroyed();

  StdOut(Ok);
}

// =============================================================================

void DevConsole::PoisonActor()
{
  if (_objectHandles[ObjectHandleType::ACTOR] == nullptr)
  {
    StdOut(ErrHandleNotSet);
    return;
  }

  ItemBonusStruct ibs;

  ibs.Type       = ItemBonusType::POISONED;
  ibs.Cumulative = true;
  ibs.BonusValue = -1;
  ibs.Duration   = -1;

  _objectHandles[ObjectHandleType::ACTOR]->AddEffect(ibs);

  StdOut(Ok);
}

// =============================================================================

void DevConsole::AwardExperience(const std::vector<std::string>& params)
{
  if (params.size() != 1)
  {
    StdOut(ErrWrongParams);
    return;
  }

  std::string expStr = params[0];
  if (!StringIsNumbers(expStr))
  {
    return;
  }

  int exp = std::stoi(expStr);
  _playerRef->AwardExperience(exp);

  StdOut(Ok);
}

// =============================================================================

void DevConsole::GiveMoney(const std::vector<std::string>& params)
{
  if (params.size() != 1)
  {
    StdOut(ErrWrongParams);
    return;
  }

  std::string n = params[0];
  if (!StringIsNumbers(n))
  {
    StdOut(ErrSyntaxError);
    return;
  }

  int amount = std::stoi(n);
  _playerRef->Money += amount;

  StdOut(Ok);
}

// =============================================================================

void DevConsole::ToggleFogOfWar()
{
  _playerRef->ToggleFogOfWar = !_playerRef->ToggleFogOfWar;

  auto state = Application::Instance().GetGameStateRefByName(GameStates::MAIN_STATE);
  state->Update(true);

  auto str = Util::StringFormat("For of war %s", _playerRef->ToggleFogOfWar ? "off" : "on");

  StdOut(str);
}

// =============================================================================

void DevConsole::ToggleGodMode()
{
  _playerRef->GodMode = !_playerRef->GodMode;

  auto str = Util::StringFormat("God mode %s", _playerRef->GodMode ? "on" : "off");
  StdOut(str);
}

// =============================================================================

void DevConsole::PrintTriggers()
{
  auto out = Util::StringFormat("Triggers on this level: %u", _currentLevel->FinishTurnTriggers.size());
  StdOut(out);

  for (auto& t : _currentLevel->FinishTurnTriggers)
  {
    auto str = Util::StringFormat("0x%X at %i %i", t.get(), t->PosX, t->PosY);
    StdOut(str);
  }
}

// =============================================================================

void DevConsole::PrintActors()
{
  auto out = Util::StringFormat("Actors on this level: %u",
                                _currentLevel->ActorGameObjects.size());
  StdOut(out);

  for (auto& a : _currentLevel->ActorGameObjects)
  {
    auto str = Util::StringFormat("0x%X at %i %i",
                                  a.get(), a->PosX, a->PosY);
    StdOut(str);
  }
}

#ifdef DEBUG_BUILD

// =============================================================================

void DevConsole::DispelEffects()
{
  _playerRef->DispelEffects();
  StdOut(Ok);
}

// =============================================================================

void DevConsole::DispelEffectsActor()
{
  if (_objectHandles[ObjectHandleType::ACTOR] == nullptr)
  {
    StdOut(ErrHandleNotSet);
    return;
  }

  _objectHandles[ObjectHandleType::ACTOR]->DispelEffects();
  StdOut(Ok);
}

#endif

// =============================================================================

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

      PrintAdditionalHelp(_commandTypeByName.at(params[0]));
    }
    else if (params[0] == "commands")
    {
      size_t count = 0;
      std::string totalString;
      for (auto& t : _allCommandsList)
      {
        count += (t.length() + _commandsDelimiter.length());
        if (count > 78)
        {
          StdOut(totalString);
          totalString.clear();
          count = (t.length() + _commandsDelimiter.length());
        }

        totalString += t;
        totalString += _commandsDelimiter;
      }

      totalString.pop_back();

      StdOut(totalString);
    }
    else
    {
      std::string msg = Util::StringFormat("No help found for '%s'", params[0].data());
      StdOut(msg);
    }
  }
}

// =============================================================================

void DevConsole::PrintHistory()
{
  size_t ind = 1;
  for (auto& line : _commandsHistory)
  {
    std::string ln = Util::StringFormat("%2d %s", ind, line.data());
    StdOut(ln);
    ind++;
  }

  //
  // Display just entered 'history' command in history as well.
  //
  std::string additional = Util::StringFormat("%2d history", ind);
  StdOut(additional);
}

// =============================================================================

void DevConsole::RepeatCommand(const std::string& shellCmd)
{
  if (shellCmd.size() == 1)
  {
    StdOut(ErrSyntaxError);
    return;
  }

  auto it = std::find_if(shellCmd.begin() + 1,
                         shellCmd.end(),
                         [](char c)
                         {
                           return (c < '0' || c > '9');
                         });

  if (it != shellCmd.end())
  {
    StdOut(ErrWrongParams);
    return;
  }

  std::string str = shellCmd.substr(1);

  int cmdIndex = std::stoi(str);
  int vecIndex = cmdIndex - 1;

  if (vecIndex < 0 || vecIndex > (int)_commandsHistory.size() - 1)
  {
    std::string ln = Util::StringFormat(ErrEventNotFound, cmdIndex);
    StdOut(ln);
    return;
  }

  StdOut(_commandsHistory[vecIndex]);

  _currentCommand = Prompt + _commandsHistory[vecIndex];
  ParseCommand();
}

// =============================================================================

void DevConsole::PrintAdditionalHelp(DevConsoleCommand command)
{
  switch (command)
  {
    case DevConsoleCommand::CREATE_MONSTER:
      PrintMap(_monsters);
      break;

    case DevConsoleCommand::TRANSFORM_TILE:
      PrintMap(_validTileTransformTypes);
      break;
  }
}

// =============================================================================

void DevConsole::StdOut(const std::string& str)
{
  _stdout.insert(_stdout.begin(), str);
}

// =============================================================================

bool DevConsole::StringIsNumbers(const std::string& str)
{
  for (auto& c : str)
  {
    auto res = std::find(Strings::Numbers.begin(),
                         Strings::Numbers.end(),
                         c);

    if (res == Strings::Numbers.end())
    {
      return false;
    }
  }

  return true;
}

// =============================================================================

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

  if (res.first  < 0 || res.first  > _currentLevel->MapSize.X - 1
   || res.second < 0 || res.second > _currentLevel->MapSize.Y - 1)
  {
    StdOut("Out of bounds");
    res = { -1, -1 };
  }

  return res;
}

// =============================================================================

void DevConsole::ReportHandle(ObjectHandleType handleType)
{
  std::string msg = Util::StringFormat("%s = 0x%X",
                                       _handleNameByType.at(handleType).data(),
                                       _objectHandles[handleType]);
  StdOut(msg);
}
