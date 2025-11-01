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
  for (auto& kvp : _commandNameByType)
  {
    if (kvp.second.length() > _longestCommandStringLength)
    {
      _longestCommandStringLength = kvp.second.length();
    }

    _trie.Add(kvp.second);
  }

  _commandTypeByName = Util::FlipMap(_commandNameByType);

  _playerRef = &Game::gApp.PlayerInstance;

  _objectHandles[ObjectHandleType::STATIC] = nullptr;
  _objectHandles[ObjectHandleType::ACTOR]  = nullptr;
  _objectHandles[ObjectHandleType::ITEM]   = nullptr;
  _objectHandles[ObjectHandleType::MAP]    = nullptr;
  _objectHandles[ObjectHandleType::ANY]    = nullptr;

  StdOut("Copyright (C) 1992, Lance Wilson Productions");
  StdOut("\"Ryder, nigga!\"");
  StdOut("");
  StdOut("Type 'help commands' for a list of available commands");
  StdOut("Hit 'TAB' for autocompletion, numpad to scroll.");
}

// =============================================================================

void DevConsole::Prepare()
{
  _closedByCommand      = false;
  _currentLevel         = Game::gMap.CurrentLevel;
  _currentCommand       = Prompt;
  _commandsHistoryIndex = _commandsHistory.size();
  _cursorPosition       = 0;
}

// =============================================================================

void DevConsole::Cleanup()
{
  _currentCommand.clear();
}

// =============================================================================

void DevConsole::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    // -------------------------------------------------------------------------

    case '`':
      Game::gApp.ChangeState(GameStates::MAIN_STATE);
      break;

    // -------------------------------------------------------------------------

    case KEY_UP:
    {
      if (!_commandsHistory.empty())
      {
        if (_commandsHistoryIndex > 0)
        {
          _commandsHistoryIndex--;
        }

        _currentCommand =
            Util::StringFormat("%s%s",
                               Prompt.data(),
                               _commandsHistory[_commandsHistoryIndex].data());

        _cursorPosition = _commandsHistory[_commandsHistoryIndex].length();
      }
    }
    break;

    // -------------------------------------------------------------------------

    case KEY_DOWN:
    {
      if (!_commandsHistory.empty())
      {
        if (_commandsHistoryIndex < (int)_commandsHistory.size() - 1)
        {
          _commandsHistoryIndex++;
          _currentCommand =
              Util::StringFormat("%s%s", Prompt.data(),
                                 _commandsHistory[_commandsHistoryIndex].data());

          _cursorPosition = _commandsHistory[_commandsHistoryIndex].length();
        }
        else
        {
          _commandsHistoryIndex = _commandsHistory.size();
          _currentCommand = Prompt;
          _cursorPosition = 0;
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case KEY_LEFT:
    {
      if (_cursorPosition > 0)
      {
        _cursorPosition--;
      }
    }
    break;

    // -------------------------------------------------------------------------

    case KEY_RIGHT:
    {
      if (_cursorPosition < (int)_currentCommand.substr(2).length())
      {
        _cursorPosition++;
      }
    }
    break;

    // -------------------------------------------------------------------------

    case VK_HOME:
    {
      _cursorPosition = 0;
    }
    break;

    // -------------------------------------------------------------------------

    case VK_END:
    {
      _cursorPosition = (int)_currentCommand.substr(2).length();
    }
    break;

    // -------------------------------------------------------------------------

    case VK_TAB:
    {
      _stdout.ResetScroll();

      std::string noPrompt = _currentCommand.substr(2);
      std::string lastCmd;

      StringV spl = Util::StringSplit(noPrompt, ' ');
      if (!spl.empty())
      {
        lastCmd = spl.back();
      }

      auto hints = _trie.FindAll(lastCmd);

      if (hints.size() == 1)
      {
        //
        // If there are several words on command line, attach hint only to the
        // last one, while preserving what was already inputted.
        //
        if (spl.size() > 1)
        {
          _currentCommand = Prompt;

          for (size_t i = 0; i < spl.size() - 1; i++)
          {
            _currentCommand.append(spl[i]);
            _currentCommand.append(" ");
          }

          _currentCommand.append(*hints.begin());
          _currentCommand.append(" ");
        }
        else
        {
          _currentCommand =
              Util::StringFormat("%s%s ", Prompt.data(), (*hints.begin()).data());
        }

        //
        // Just in case.
        //
        if (_currentCommand.length() > Prompt.length())
        {
          _cursorPosition = _currentCommand.length() - Prompt.length();
        }
      }
      else if (hints.size() != 0)
      {
        StdOut(Prompt);

        //
        // Check if there's common prefix in returned hints.
        // If yes - autocomplete to it.
        //
        StringV hintsV;
        hintsV.reserve(hints.size());

        size_t shortestStringLength = std::numeric_limits<size_t>::max();
        for (auto& item : hints)
        {
          if (item.length() < shortestStringLength)
          {
            shortestStringLength = item.length();
          }

          hintsV.push_back(item);
        }

        std::string commonPrefix;

        bool commonPrefixFound = true;
        if (not hintsV.empty())
        {
          for (size_t i = 0; i < shortestStringLength; i++)
          {
            char c = hintsV[0][i];
            for (auto& s : hintsV)
            {
              if (s[i] != c)
              {
                commonPrefixFound = false;
                break;
              }
            }

            if (not commonPrefixFound)
            {
              break;
            }

            commonPrefix.append(1, c);
          }
        }

        //
        // Prefix found, attach it to current command with all that was
        // inputted.
        //
        if (not commonPrefix.empty())
        {
          if (spl.size() > 1)
          {
            _currentCommand = Prompt;

            for (size_t i = 0; i < spl.size() - 1; i++)
            {
              _currentCommand.append(spl[i]);
              _currentCommand.append(" ");
            }

            _currentCommand.append(commonPrefix);
          }
          else
          {
            _currentCommand = Util::StringFormat("%s%s",
                                                  Prompt.data(),
                                                  commonPrefix.data());
          }

          //
          // Just in case.
          //
          if (_currentCommand.length() > Prompt.length())
          {
            _cursorPosition = _currentCommand.length() - Prompt.length();
          }

          hints = _trie.FindAll(commonPrefix);
        }

        for (auto& item : hints)
        {
          StdOut(item);
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

#ifdef USE_SDL
    case NUMPAD_8:
#else
    case KEY_PPAGE:
#endif
    {
      _stdout.ScrollUp();
    }
    break;

    // -------------------------------------------------------------------------

#ifdef USE_SDL
    case NUMPAD_5:
#else
    case KEY_NPAGE:
#endif
    {
      _stdout.ScrollDown();
    }
    break;

#ifdef USE_SDL
    case NUMPAD_2:
    {
      _stdout.ResetScroll();
    }
    break;
#endif

    // -------------------------------------------------------------------------

    case VK_ENTER:
    {
      _stdout.ResetScroll();

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

      if (_commandsHistory.size() > _maxHistory)
      {
        _commandsHistory.erase(_commandsHistory.begin());
        _commandsHistoryIndex = _commandsHistory.size();
      }

      _cursorPosition = 0;
    }
    break;

    // -------------------------------------------------------------------------

    case VK_BACKSPACE:
    {
      if (_currentCommand.length() > 2)
      {
        if (_cursorPosition > 0)
        {
          size_t pos = 2 + _cursorPosition - 1;
          _currentCommand.erase(_currentCommand.begin() + pos);
          _cursorPosition--;
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    case VK_DELETE:
    {
      if (_currentCommand.length() > 2)
      {
        if ((int)_currentCommand.substr(2).length() > _cursorPosition)
        {
          _currentCommand.erase(_currentCommand.begin() + 2 + _cursorPosition);
        }
      }
    }
    break;

    // -------------------------------------------------------------------------

    default:
    {
      if (_keyPressed >= 32 && _keyPressed <= 126)
      {
        _currentCommand.insert(_currentCommand.begin() + 2 + _cursorPosition,
                               (char)_keyPressed);
        _cursorPosition++;
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
    Game::gPrnt.Clear();

    DrawHeader(" DEVELOPER'S CONSOLE ");
    Game::gPrnt.DrawScrollBars(_stdout);

    auto msgs = _stdout.GetMessages();

    int lineCount = 0;
    for (const std::string* msg : msgs)
    {
      if (msg == nullptr)
      {
        break;
      }

      Game::gPrnt.PrintFB(1,
                          1 + lineCount,
                          *msg,
                          Printer::kAlignLeft,
                          Colors::WhiteColor,
                          Colors::BlackColor);

      lineCount++;
    }

    Game::gPrnt.PrintFB(1,
                        1 + lineCount,
                        _currentCommand,
                        Printer::kAlignLeft,
                        Colors::WhiteColor,
                        Colors::BlackColor);

    Game::gPrnt.PrintFB(3 + _cursorPosition,
                        1 + lineCount,
                        ' ',
                        Colors::BlackColor,
                        Colors::WhiteColor);

    Game::gPrnt.Render();
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

  StringV params = Util::StringSplit(_currentCommand, ' ');

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
                                const StringV& params)
{
  DevConsoleCommand c = _commandTypeByName.at(command);

  switch (c)
  {
    // ------------------ shell builtins ---------------------------------------

    case DevConsoleCommand::CLEAR:
      _stdout.Clear();
      break;

    case DevConsoleCommand::HELP:
    case DevConsoleCommand::HELP2:
      DisplayHelpAboutCommand(params);
      break;

    case DevConsoleCommand::CLOSE:
    case DevConsoleCommand::CLOSE2:
    case DevConsoleCommand::CLOSE3:
    case DevConsoleCommand::CLOSE4:
    {
      _closedByCommand = true;
      Game::gApp.ChangeState(GameStates::MAIN_STATE);
    }
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
      int expToGive = _playerRef->Attrs.Exp.Max().Get() -
                      _playerRef->Attrs.Exp.Min().Get();

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
      Game::gMap.PrintMapLayout();
      StdOut(Ok);
      break;

    case DevConsoleCommand::PRINT_COLORS:
      PrintColors();
      break;

    case DevConsoleCommand::REPORT_PLAYER:
    {
      auto lines = _playerRef->Dump();
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

    case DevConsoleCommand::CREATE_SHRINE:
      CreateShrine(params);
      break;

    case DevConsoleCommand::CREATE_DUMMY_OBJECT:
      CreateDummyObject(params);
      break;

    case DevConsoleCommand::CREATE_CHEST:
      CreateChest(params);
      break;

    case DevConsoleCommand::CREATE_BREAKABLE:
      CreateBreakable(params);
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

    case DevConsoleCommand::IGNORE_PLAYER:
      TogglePlayerIgnore();
      break;

    case DevConsoleCommand::KILL_PLAYER:
      KillPlayer();
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

    case DevConsoleCommand::SPAM_TO_LOG:
      SpamToLog(params);
      break;

    case DevConsoleCommand::LAUNCH_PROJECTILE:
      LaunchProjectile(params);
      break;

    case DevConsoleCommand::INSPECT:
      Inspect(params);
      break;

    default:
      StdOut(ErrCmdNotHandled);
      break;
  }
}

// =============================================================================

void DevConsole::GetObjectByAddress(const StringV& params)
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

  std::string hexAddr;
  if (!ParamIsHex(str, hexAddr))
  {
    StdOut(ErrNotAHexString);
    return;
  }

  _objectHandles[ObjectHandleType::ANY] =
      _currentLevel->FindObjectByAddress(hexAddr);

  ReportHandle(ObjectHandleType::ANY);
  ReportHandleDebugInfo(ObjectHandleType::ANY);
}

// =============================================================================

void DevConsole::GetObjectById(const StringV& params)
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
    auto lines = _objectHandles[type]->Dump();
    PrintDebugInfo(lines);
  }
}

// =============================================================================

void DevConsole::PrintDebugInfo(const StringV& debugInfo)
{
  for (auto& l : debugInfo)
  {
    StdOut(l);
  }
}

// =============================================================================

void DevConsole::TransformTile(const StringV& params)
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
      _currentLevel->PlaceGroundTile(x,
                                     y,
                                     '.',
                                     Colors::BlackColor,
                                     Colors::DirtColor,
                                     "Ground");
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

void DevConsole::PlaceWall(const StringV& params)
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

  GameObject* wall = new GameObject(_currentLevel,
                                    x,
                                    y,
                                    '#',
                                    Colors::WhiteColor,
                                    Colors::MagentaColor);

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

  auto& cache = Game::gPrnt.GetValidColorsCache();
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

    std::string toAdd =  Util::StringFormat("[%s|%s] ",
                                            fgTotal.data(),
                                            bgTotal.data());
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
    std::string msg = Util::StringFormat("%s%s = 0x%" PRIXLEAST64,
                                         kvp.second.data(),
                                         spaces.data(),
                                         _objectHandles[kvp.first]);
    StdOut(msg);
  }
}

// =============================================================================

void DevConsole::CreateDummyActor(const StringV& params)
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

  GameObject* actor = Game::gMI.CreateNPC(x, y, NPCType::UNDEFINED);

  Game::gMap.CurrentLevel->PlaceActor(actor);

  StdOut(actor->HexAddressString);
}

// =============================================================================

void DevConsole::CreateMonster(const StringV& params)
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

  auto go = Game::gMI.CreateMonster(x, y, objType);
  _currentLevel->PlaceActor(go);

  StdOut(go->HexAddressString);
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

      auto go = Game::gIF.CreateGem(1 + count, 9 + yOffset,
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
    auto go = Game::gIF.CreatePotion(kvp.first);
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
      auto scroll = Game::gIF.CreateScroll(1 + xOffset,
                                            10 + i,
                                            item,
                                            p);

      ItemComponent* ic = scroll->GetComponent<ItemComponent>();
      ic->Data.IsIdentified = true;
      _currentLevel->PlaceGameObject(scroll);
      xOffset++;
    }
  }

  StdOut(Ok);
}

// =============================================================================

void DevConsole::CreateItem(const StringV& params)
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

  GameObject* go = Game::gIF.CreateRandomItem(x, y);
  if (go == nullptr)
  {
    StdOut("Generated object type is not implemented yet!");
    return;
  }

  _currentLevel->PlaceGameObject(go);

  StdOut(go->HexAddressString);
}

// =============================================================================

void DevConsole::CreateDummyObject(const StringV& params)
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
    return;
  }

  char image = 'D';

  if (params.size() >= 3)
  {
    image = params[2][0];
  }

  GameObject* go =
      Game::gGOF.CreateDummyObject(r.first,
                                    r.second,
                                    "Dummy",
                                    image,
                                    Colors::WhiteColor,
                                    Colors::BlackColor);

  _currentLevel->PlaceGameObject(go);

  StdOut(go->HexAddressString);
}

// =============================================================================

void DevConsole::CreateChest(const StringV& params)
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

  GameObject* go = Game::gGOF.CreateChest(r.first,
                                           r.second,
                                           false);

  _currentLevel->PlaceStaticObject(go);

  StdOut(go->HexAddressString);
}

// =============================================================================

void DevConsole::CreateShrine(const StringV& params)
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

  std::string shrineType = params[2];
  if (!StringIsNumbers(shrineType))
  {
    StdOut(ErrWrongParams);
    return;
  }

  int type = std::stoul(shrineType);
  if (type >= (int)ShrineType::LAST_ELEMENT)
  {
    StdOut(ErrWrongParams);
    return;
  }

  GameObject* go = Game::gGOF.CreateShrine(r.first,
                                            r.second,
                                            (ShrineType)type,
                                            100);

  _currentLevel->PlaceStaticObject(go);

  StdOut(go->HexAddressString);
}

// =============================================================================

void DevConsole::CreateBreakable(const StringV& params)
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

  static GameObjectsFactory& gof = Game::gGOF;

  GameObject* go =
      gof.CreateBreakableObjectWithRandomLoot(r.first,
                                              r.second,
                                              'B',
                                              "Breakable",
                                              Colors::WoodColor,
                                              Colors::BlackColor);
  _currentLevel->PlaceStaticObject(go);

  StdOut(go->HexAddressString);
}

// =============================================================================

void DevConsole::GetObject(const StringV& params,
                           ObjectHandleType handleType)
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
      _objectHandles[handleType] =
          Game::gMap.GetStaticGameObjectAtPosition(x, y);
      break;

    case ObjectHandleType::ITEM:
    {
      auto res = Game::gMap.GetGameObjectsAtPosition(x, y);
      if (!res.empty())
      {
        _objectHandles[handleType] = res.back();
      }
    }
    break;

    case ObjectHandleType::ACTOR:
      _objectHandles[handleType] = Game::gMap.GetActorAtPosition(x, y);
      break;

    case ObjectHandleType::MAP:
      _objectHandles[handleType] = Game::gMap.GetMapObjectAtPosition(x, y);
      break;

    case ObjectHandleType::ANY:
    {
      GameObject* res = nullptr;

      res = Game::gMap.GetActorAtPosition(x, y);

      if (res == nullptr)
      {
        auto vector = Game::gMap.GetGameObjectsAtPosition(x, y);
        if (vector.empty())
        {
          res = Game::gMap.GetStaticGameObjectAtPosition(x, y);
          if (res == nullptr)
          {
            res = Game::gMap.GetMapObjectAtPosition(x, y);
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

void DevConsole::MoveObject(const StringV& params,
                            ObjectHandleType handleType)
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
      GameObject* go = Game::gMap.GetStaticGameObjectAtPosition(x, y);
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

void DevConsole::MovePlayer(const StringV& params)
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

void DevConsole::RemoveObject(const StringV& params)
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

  GameObject* go = Game::gMap.GetActorAtPosition(x, y);
  if (go == nullptr)
  {
    std::vector<GameObject*> res =
        Game::gMap.GetGameObjectsAtPosition(x, y);

    if (res.empty())
    {
      go = Game::gMap.GetStaticGameObjectAtPosition(x, y);
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
    go->Destroy();
  }

  Game::gMap.RemoveDestroyed();

  StdOut(Ok);
}

// =============================================================================

void DevConsole::DamageActor(const StringV& params)
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
  _objectHandles[ObjectHandleType::ACTOR]->ReceiveDamage(nullptr,
                                                         dmg,
                                                         true,
                                                         true,
                                                         isDirect,
                                                         false);

  Game::gMap.RemoveDestroyed();

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

void DevConsole::AwardExperience(const StringV& params)
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

void DevConsole::GiveMoney(const StringV& params)
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

  auto state =
      Game::gApp.GetGameStateRefByName(GameStates::MAIN_STATE);

  state->Update(true);

  auto str = Util::StringFormat("For of war %s",
                                _playerRef->ToggleFogOfWar
                                ? "off"
                                : "on");

  StdOut(str);
}

// =============================================================================

void DevConsole::ToggleGodMode()
{
  _playerRef->GodMode = !_playerRef->GodMode;

  StdOut(
    Util::StringFormat("God mode: %s", _playerRef->GodMode ? "ON" : "OFF")
  );
}

// =============================================================================

void DevConsole::TogglePlayerIgnore()
{
  _playerRef->IgnoreMe = !_playerRef->IgnoreMe;

  StdOut(
    Util::StringFormat("Player ignore: %s", _playerRef->IgnoreMe ? "ON" : "OFF")
  );
}

// =============================================================================

void DevConsole::KillPlayer()
{
  _playerRef->Attrs.HP.SetMin(0);
  Game::gApp.ChangeState(GameStates::GAMEOVER_STATE);
}

// =============================================================================

void DevConsole::PrintTriggers()
{
  auto out = Util::StringFormat("Triggers on this level: %u",
                                _currentLevel->FinishTurnTriggers.size());
  StdOut(out);

  for (auto& t : _currentLevel->FinishTurnTriggers)
  {
    auto str = Util::StringFormat("0x%" PRIXLEAST64 " at %i %i",
                                   t.get(),
                                   t->PosX,
                                   t->PosY);
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
    auto str = Util::StringFormat("0x%" PRIXLEAST64 " at %i %i",
                                  a.get(), a->PosX, a->PosY);
    StdOut(str);
  }
}

// =============================================================================

void DevConsole::SpamToLog(const StringV& params)
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

  int count = std::stoi(n);

  for (int i = 0; i < count; i++)
  {
    Game::gPrnt.AddMessage(
      Util::StringFormat("Message %02d", (i + 1))
    );
  }

  StdOut(Ok);
}

// =============================================================================

void DevConsole::LaunchProjectile(const StringV& params)
{
  if (params.size() != 4)
  {
    StdOut(ErrWrongParams);
    return;
  }

  std::string x1s = params[0];
  std::string y1s = params[1];
  std::string x2s = params[2];
  std::string y2s = params[3];

  if (!StringIsNumbers(x1s)
   || !StringIsNumbers(y1s)
   || !StringIsNumbers(x2s)
   || !StringIsNumbers(y2s))
  {
    StdOut(ErrSyntaxError);
    return;
  }

  int x1 = std::stoi(x1s);
  int y1 = std::stoi(y1s);
  int x2 = std::stoi(x2s);
  int y2 = std::stoi(y2s);

  Position from(x1, y1);
  Position to(x2, y2);

  Game::gApp.ChangeState(GameStates::MAIN_STATE);

  Util::LaunchProjectile(from, to, '*', Colors::YellowColor);
}

// =============================================================================

void DevConsole::Inspect(const StringV& params)
{
  if (params.size() != 1)
  {
    StdOut(ErrWrongParams);
    return;
  }

  std::string n = params[0];

  std::string hexString;
  if (!ParamIsHex(n, hexString))
  {
    StdOut(ErrNotAHexString);
    return;
  }

  uintptr_t addr = std::stoull(hexString, nullptr, 16);

  void* casted = reinterpret_cast<void*>(addr);

  bool objFound = (AnyObjectByAddr.count(casted) == 1);

  StringV lines = DumpObj(objFound ? AnyObjectByAddr[casted] : nullptr);

  PrintDebugInfo(lines);
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

void DevConsole::DisplayHelpAboutCommand(const StringV& params)
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
    if (params[0] == "commands")
    {
      size_t count = 0, spacesCount = 0;
      std::string totalString;
      std::stringstream ss;
      for (auto& kvp : _commandTypeByName)
      {
        const std::string& t = kvp.first;

        spacesCount = _longestCommandStringLength - t.length();
        count += t.length() + spacesCount;

        if (count > 78)
        {
          StdOut(ss.str());
          ss.str(std::string());
          spacesCount = _longestCommandStringLength - t.length();
          count = t.length() + spacesCount;
        }

        std::string spaces(spacesCount, ' ');
        totalString = Util::StringFormat("%s%s ", t.data(), spaces.data());

        ss << totalString;
      }

      StdOut(ss.str());
    }
    else if (_helpTextByCommandName.count(params[0]) == 1)
    {
      for (auto& line : _helpTextByCommandName.at(params[0]))
      {
        StdOut(line);
      }

      PrintAdditionalHelp(_commandTypeByName.at(params[0]));
    }
    else
    {
      std::string msg = Util::StringFormat("No help found for '%s'",
                                           params[0].data());
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
                           return ( (c < '0') || (c > '9') );
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
  _stdout.AddMessage(str);
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

std::pair<int, int> DevConsole::CoordinateParamsToInt(const std::string &px,
                                                      const std::string &py)
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
    StdOut(ErrOutOfBounds);
    res = { -1, -1 };
  }

  return res;
}

// =============================================================================

bool DevConsole::ParamIsHex(const std::string& param, std::string& out)
{
  auto res = Util::StringSplit(param, 'x');
  if (res.size() == 1 || res.size() > 2)
  {
    return false;
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
      return false;
    }
  }

  out = "0x" + addr;

  return true;
}

// =============================================================================

void DevConsole::ReportHandle(ObjectHandleType handleType)
{
  std::string msg = Util::StringFormat("%s = 0x%" PRIXLEAST64,
                                       _handleNameByType.at(handleType).data(),
                                       _objectHandles[handleType]);
  StdOut(msg);
}
