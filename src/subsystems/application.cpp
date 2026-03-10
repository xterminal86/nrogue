#include "application.h"

#include "gid-generator.h"
#include "map.h"
#include "printer.h"
#include "timer.h"

#include "container-component.h"

// -----------------------------------------------------------------------------

#include "main-state.h"
#include "info-state.h"
#include "attack-state.h"
#include "menu-state.h"
#include "select-class-state.h"
#include "custom-class-state.h"
#include "enter-name-state.h"
#include "intro-state.h"
#include "inventory-state.h"
#include "help-state.h"
#include "start-game-state.h"
#include "container-interact-state.h"
#include "message-log-state.h"
#include "look-input-state.h"
#include "interact-input-state.h"
#include "npc-interact-state.h"
#include "shopping-state.h"
#include "returner-state.h"
#include "repair-state.h"
#include "save-game-state.h"
#include "pickup-item-state.h"
#include "exiting-state.h"
#include "message-box-state.h"
#include "service-state.h"
#include "target-state.h"
#include "gameover-state.h"
#include "obituary-report-state.h"

// -----------------------------------------------------------------------------

#include "globals.h"
#include "map-level-base.h"
#include "util.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#include "dev-console.h"
#endif

#include <fstream>

void Application::Init(bool skipMenu)
{
  if (_initialized)
  {
    return;
  }

  if (!InitGraphics())
  {
    return;
  }

  InitGameStates();

  _currentState = _gameStates[GameStates::MENU_STATE].get();

  if (!skipMenu)
  {
    //
    // In SDL build GetKeyDown() will return -1 on application start, resulting in
    // white screen due to no key being pressed yet, so no drawing of current
    // state happens, which is not what we want.
    //
    _currentState->Update(true);
  }

  PlayerInstance.Attrs.Indestructible = false;

  Game::gPrnt.AddMessage("You begin your quest");
  Game::gPrnt.AddMessage("Press 'h' for help");

  _appReady = true;

  _initialized = true;
}

// =============================================================================

void Application::Run()
{
  while (_currentState != nullptr)
  {
    Game::gTimer.MeasureStart();

    //
    // If player is not alive, it is assumed,
    // that we are now in EndgameState,
    // which needs to be processed.
    //
    if (PlayerInstance.CanAct() || !PlayerInstance.HasNonZeroHP())
    {
      //
      // Since change state usually happens in HandleInput(),
      // if it's called before Update() to exit game
      // (change state to nullptr), we'll get segfault because
      // _currentState->Update() gets called on nullptr.
      //
      // Same thing happens if we change order of methods calls,
      // since theoretically state can be changed in Update() too.
      //
      // Thus, this shitcode below.
      //
      // Also we need to immediately update changes that happened after
      // user pressed some keys that affected visual representation.
      //
      if (_currentState != nullptr)
      {
        _currentState->Update();
      }

      if (_currentState != nullptr)
      {
        _currentState->HandleInput();
      }
    }
    else
    {
      //
      // If player has levelled up, stop updating everything
      // until message box is closed, or we can get attack animations
      // on top of message box.
      //
      if (CurrentStateIs(GameStates::MESSAGE_BOX_STATE))
      {
        _currentState->HandleInput();
      }
      else
      {
        Game::gMap.Update();
        PlayerInstance.WaitForTurn();

        MapUpdateCyclesPassed++;
      }
    }

    Game::gTimer.MeasureEnd();

    #ifdef DEBUG_BUILD
    auto report = Game::gTimer.GetProfilingReport();

    if (!report.empty())
    {
      Game::gLogger.Print("=== PROFILER START ===");
    }

    for (auto& line : report)
    {
      Game::gLogger.Print(line);
    }

    if (!report.empty())
    {
      Game::gLogger.Print("=== PROFILER END ===");
    }
    #endif
  }
}

// =============================================================================

void Application::ChangeState(const GameStates& gameStateIndex)
{
  //
  // Don't self-change.
  //
  if (_gameStates[gameStateIndex].get() == _currentState)
  {
    _currentState->Update(true);
    return;
  }

  #ifdef DEBUG_BUILD
  if (gameStateIndex != GameStates::EXIT_GAME)
  {
    auto str =
        Util::StringFormat("Changing state: %s [0x%" PRIXLEAST64 "] => "
                           "%s [0x%" PRIXLEAST64 "]",
                           typeid(*_currentState).name(),
                           _currentState,
                           typeid(*_gameStates[gameStateIndex].get()).name(),
                           _gameStates[gameStateIndex].get());
    LogPrint(str);
    //DebugLog("%s\n", str.data());
  }
  else
  {
    auto str = Util::StringFormat("Changing state: %s [0x%" PRIXLEAST64 "] "
                                  "=> EXIT_GAME [0x0]",
                                  typeid(*_currentState).name(),
                                  _currentState);
    LogPrint(str);
    //DebugLog("%s\n", str.data());
  }
  #endif

  _currentState->Cleanup();

  _currentState = (gameStateIndex == GameStates::EXIT_GAME)
                  ? nullptr
                  : _gameStates[gameStateIndex].get();

  if (_currentState != nullptr)
  {
    _currentState->Prepare();

    //
    // I don't know how it worked before without this line.
    //
    _currentState->Update(true);
  }
}

// =============================================================================

GameState* Application::GetGameStateRefByName(GameStates stateName)
{
  if (_gameStates.count(stateName) == 1)
  {
    return _gameStates[stateName].get();
  }

  return nullptr;
}

// =============================================================================

bool Application::IsAppReady()
{
  return _appReady;
}

// =============================================================================

bool Application::CurrentStateIs(GameStates stateName)
{
  if (_gameStates.count(stateName) == 1)
  {
    return (_currentState == _gameStates[stateName].get());
  }

  return false;
}

// =============================================================================

void Application::ShowMessageBox(MessageBoxType type,
                                 const std::string& header,
                                 const std::vector<std::string>& message,
                                 const uint32_t& borderColor,
                                 const uint32_t& bgColor)
{
  _previousState = _currentState;

  auto ptr = _gameStates[GameStates::MESSAGE_BOX_STATE].get();
  MessageBoxState* mbs = static_cast<MessageBoxState*>(ptr);
  mbs->SetMessage(type, header, message, borderColor, bgColor);

  _currentState = ptr;

  _currentState->Update(true);
}

// =============================================================================

void Application::CloseMessageBox()
{
  _currentState = _previousState;
  _currentState->Update(true);
}

// =============================================================================

void Application::DisplayAttack(GameObject* defender,
                                uint32_t delayMs,
                                const std::string& messageToPrint,
                                const uint32_t& cursorColor)
{
  if (GameConfig.FastCombat)
  {
    if (messageToPrint.length() != 0)
    {
      Game::gPrnt.AddMessage(messageToPrint);
    }
  }
  else
  {
    int posX = defender->PosX + Game::gMap.CurrentLevel->MapOffsetX;
    int posY = defender->PosY + Game::gMap.CurrentLevel->MapOffsetY;

    DrawAttackCursor(posX, posY, defender, cursorColor);

    Util::Sleep(delayMs);

    if (messageToPrint.length() != 0)
    {
      Game::gPrnt.AddMessage(messageToPrint);
    }

    DrawAttackCursor(posX, posY, defender);

    Util::Sleep(delayMs);
  }
}

// =============================================================================

void Application::DrawAttackCursor(int x, int y,
                                   GameObject* defender,
                                   const uint32_t& cursorColor)
{
  //
  // TODO: try to make attack animation fancy (maybe?) in SDL build
  //
  if (cursorColor == Colors::None)
  {
    if (defender->FgColor != Colors::None
     && defender->BgColor != Colors::None)
    {
#ifdef USE_SDL
      Game::gPrnt.DrawSubstituteGraphicsTile(x,
                                             y,
                                             defender->Image,
                                             defender->FgColor);
#else
      Game::gPrnt.PrintChar(
        x,
        y,
        defender->Image,
        defender->FgColor,
        defender->BgColor
      );
#endif
      Game::gPrnt.Render();
    }
  }
  else
  {
#ifdef USE_SDL
    Game::gPrnt.DrawSubstituteGraphicsTile(x,
                                           y,
                                           (int)NameCP437::BLOCK,
                                           cursorColor);
#else
    Game::gPrnt.PrintChar(
      x,
      y,
      ' ',
      Colors::Black,
      cursorColor
    );
#endif
    Game::gPrnt.Render();
  }
}

// =============================================================================

void Application::WriteObituary(bool wasKilled)
{
  std::ofstream postMortem("obituary.txt");

  std::stringstream ss;

  StringV data = CollectObituary(wasKilled, true);

  for (auto& line : data)
  {
    ss << line;
  }

  postMortem << ss.str();

  postMortem.close();
}

// =============================================================================

StringV Application::CollectObituary(bool wasKilled, bool asciiMode)
{
  StringV res;

  CollectMapAroundPlayer(res, wasKilled, asciiMode);
  CollectGeneralInfo(res, wasKilled, asciiMode);
  CollectPrettyAlignedStatInfo(res, asciiMode);

  size_t stringResizeWidth = CollectPossessions(res, asciiMode);

  CollectKills(res, stringResizeWidth, asciiMode);

  return res;
}

// =============================================================================

void Application::CollectKills(StringV& writeTo,
                               size_t stringResizeWidth,
                               bool asciiMode)
{
  WriteObituaryLine(writeTo, "", asciiMode);
  WriteObituaryLine(writeTo, "**********    KILLS    **********", asciiMode);
  WriteObituaryLine(writeTo, "", asciiMode);
  WriteObituaryLine(writeTo, "", asciiMode);

  for (auto& kvp : PlayerInstance.TotalKills)
  {
    std::string name = kvp.first;
    int totalKills   = kvp.second;

    name.resize(stringResizeWidth, ' ');

    WriteObituaryLine(
      writeTo,
      Util::StringFormat("%s %d", name.data(), totalKills),
      asciiMode
    );
  }
}

// =============================================================================

void Application::SaveGame()
{
  using C  = std::chrono::system_clock;
  using TP = std::chrono::time_point<C>;

  NRS save{"game saver"};

  DebugLog("saving game...");

  TP before = C::now();

  SaveBaseStuff(save);

  //
  // FIXME: one level for now
  //
  Game::gMap.CurrentLevel->Serialize(save);

  SavePlayer(save);

  if (!save.Save(Strings::SaveFileName))
  {
    ConsoleLog("[ERR] couldn't save at %s !", Strings::SaveFileName.data());
  }
  else
  {
    FT::duration<double, std::milli> dur = C::now() - before;
    DebugLog("done in %.4f ms", dur.count());
  }
}

// =============================================================================

void Application::SaveBaseStuff(NRS& save)
{
  namespace SK = Strings::SerializationKeys;

  NRS& root = save[SK::Root];

  root[SK::Gid].SetUInt(Game::gGid.GetCurrentGlobalId());
  {
    NRS& node = root[SK::Seed];

    node[SK::Name].SetString(Game::gRng.GetSeedString().first);
    node[SK::Value].SetUInt(Game::gRng.Seed);
  }
}

// =============================================================================

void Application::SavePlayer(NRS& save)
{
  namespace SK = Strings::SerializationKeys;

  NRS& root = save[SK::Root];

  PlayerInstance.Serialize(root);
}

// =============================================================================

void Application::LoadGame()
{
  // TODO:
}

// =============================================================================

void Application::WriteObituaryLine(StringV& writeTo,
                                    const std::string& line,
                                    bool asciiMode)
{
  writeTo.push_back(line);

  if (asciiMode)
  {
    writeTo.push_back("\n");
  }
}

// =============================================================================

size_t Application::CollectPossessions(StringV&writeTo, bool asciiMode)
{
  WriteObituaryLine(writeTo, "", asciiMode);
  WriteObituaryLine(writeTo, "********** POSSESSIONS **********", asciiMode);
  WriteObituaryLine(writeTo, "", asciiMode);

  size_t stringResizeWidth = 0;
  for (auto& i : PlayerInstance.Inventory->Contents)
  {
    ItemComponent* ic = i->GetComponent<ItemComponent>();
    std::string name = ic->Data.IdentifiedName;
    if (name.length() > stringResizeWidth)
    {
      stringResizeWidth = name.length();
    }
  }

  for (auto& i : PlayerInstance.Inventory->Contents)
  {
    ItemComponent* ic = i->GetComponent<ItemComponent>();

    std::string name = ic->Data.IdentifiedName;
    name.resize(stringResizeWidth, ' ');

    std::stringstream ss;

    ss << name;

    if (ic->Data.IsStackable || ic->Data.IsChargeable)
    {
      ss << " (" << ic->Data.Amount << ")";
    }

    if (ic->Data.IsEquipped)
    {
      ss << " (E)";
    }

    WriteObituaryLine(writeTo, ss.str(), asciiMode);
  }

  return stringResizeWidth;
}

// =============================================================================

void Application::CollectMapAroundPlayer(StringV& writeTo,
                                         bool wasKilled,
                                         bool asciiMode)
{
  MapLevelBase* curLvl = Game::gMap.CurrentLevel;

  int px = PlayerInstance.PosX;
  int py = PlayerInstance.PosY;

  int range = 10;

  int lx = PlayerInstance.PosX - range;
  int ly = PlayerInstance.PosY - range;
  int hx = PlayerInstance.PosX + range;
  int hy = PlayerInstance.PosY + range;

  CharV2 map;

  for (int y = ly; y <= hy; y++)
  {
    std::vector<char> row;
    for (int x = lx; x <= hx; x++)
    {
      if (!Util::IsInsideMap({ x, y }, curLvl->MapSize, false))
      {
        row.push_back(' ');
      }
      else
      {
        char ch = ' ';

        bool isVisibleOrRevealed = (curLvl->MapArray[x][y]->Visible
                                 || curLvl->MapArray[x][y]->Revealed);

        bool isPlayer = (x == px && y == py);

        if (isVisibleOrRevealed)
        {
          ch = curLvl->MapArray[x][y]->Image;

          //
          // If walls are ' ', display them as '#'
          //
          if (curLvl->MapArray[x][y]->Blocking
           && curLvl->MapArray[x][y]->BlocksSight
           && ch == ' ')
          {
            ch = '#';
          }

          //
          // Check items first.
          //
          auto gos = Game::gMap.GetGameObjectsAtPosition(x, y);
          if (!gos.empty())
          {
            ch = gos.back()->Image;

            if (asciiMode && ch == ' ')
            {
              ch = 'o';
            }
          }

          //
          // If there are no objects lying above static game object,
          // draw static game object.
          //
          if (gos.empty())
          {
            auto so = Game::gMap.GetStaticGameObjectAtPosition(x, y);
            if (so != nullptr)
            {
              ch = (so->Image == ' ') ? '#' : so->Image;
            }
          }

          //
          // If actor is standing on this cell, draw him instead.
          //
          auto actor = Game::gMap.GetActorAtPosition(x, y);
          if (actor != nullptr)
          {
            bool imageNonPrintable = (actor->Image < 33);
            ch = (asciiMode && imageNonPrintable) ? '@' : actor->Image;
          }

          //
          // If character is not printable, replace it with 'x' character.
          //
          if (asciiMode && ch < 32)
          {
            ch = 'x';
          }
        }

        //
        // Draw player on top of everything.
        //
        if (isPlayer)
        {
          ch = wasKilled ? '%' : '@';
        }

        row.push_back(ch);
      }
    }

    map.push_back(row);
  }

  for (size_t x = 0; x < map.size(); x++)
  {
    std::stringstream ss;

    for (size_t y = 0; y < map[x].size(); y++)
    {
      ss << Util::StringFormat("%c", map[x][y]);
    }

    WriteObituaryLine(writeTo, ss.str(), asciiMode);
  }
}

// =============================================================================

void Application::CollectGeneralInfo(StringV& writeTo,
                                     bool wasKilled,
                                     bool asciiMode)
{
  MapLevelBase* curLvl = Game::gMap.CurrentLevel;

  std::string playerEndCause = wasKilled
                               ? "has perished at"
                               : "has quit at";

  WriteObituaryLine(writeTo, "", asciiMode);
  WriteObituaryLine(writeTo, "********** OBITUARY **********", asciiMode);
  WriteObituaryLine(writeTo, "", asciiMode);

  WriteObituaryLine(
    writeTo,
    Util::StringFormat("World seed was: 0x%s (%s)",
                       Game::gRng.GetSeedAsHex().data(),
                       Game::gRng.GetSeedString().first.data()),
    asciiMode
  );
  WriteObituaryLine(writeTo, "", asciiMode);

  std::string nameAndTitle =
      Util::StringFormat("%s the %s",
                         PlayerInstance.Name.data(),
                         PlayerInstance.GetClassName().data());

  WriteObituaryLine(
    writeTo,
    Util::StringFormat("%s of level %d %s %s",
                       nameAndTitle.data(),
                       PlayerInstance.Attrs.Lvl.Get(),
                       playerEndCause.data(),
                       curLvl->LevelName.data()),
    asciiMode
  );
  WriteObituaryLine(writeTo, "", asciiMode);

  WriteObituaryLine(
    writeTo,
    Util::StringFormat("He survived %d turns", PlayerTurnsPassed),
    asciiMode
  );
  WriteObituaryLine(writeTo, "", asciiMode);

  WriteObituaryLine(
    writeTo,
    Util::StringFormat("HP %d / %d",
                       PlayerInstance.Attrs.HP.Min().Get(),
                       PlayerInstance.Attrs.HP.Max().Get()),
    asciiMode
  );

  WriteObituaryLine(
    writeTo,
    Util::StringFormat("MP %d / %d",
                       PlayerInstance.Attrs.MP.Min().Get(),
                       PlayerInstance.Attrs.MP.Max().Get()),
    asciiMode
  );
  WriteObituaryLine(writeTo, "", asciiMode);
}

// =============================================================================

void Application::CollectPrettyAlignedStatInfo(StringV& writeTo, bool asciiMode)
{
  std::vector<std::string> statInfoStrings;
  std::vector<StatInfo> statInfos;

  for (auto& i : _statNames)
  {
    StatInfo statInfo = GetStatInfo(i);
    statInfos.push_back(statInfo);

    std::stringstream ss;
    ss << statInfo.AttrName << ": "
       << statInfo.OriginalValue
       << " (";

    if (statInfo.Modifier > 0)
    {
      ss << "+";
    }

    ss << statInfo.Modifier << ") ";

    statInfoStrings.push_back(ss.str());
  }

  size_t statInfoLongestLength = Util::FindLongestStringLength(statInfoStrings);

  std::vector<std::string> resultingValuesStringList;

  size_t statInfoIndex = 0;
  for (auto& i : statInfoStrings)
  {
    if (i.length() < statInfoLongestLength)
    {
      size_t spacesCount = statInfoLongestLength - i.length();
      if (spacesCount != 0)
      {
        i.append(spacesCount, ' ');
      }
    }

    std::string str = std::to_string(statInfos[statInfoIndex].ResultingValue);
    resultingValuesStringList.push_back(str);

    statInfoIndex++;
  }

  statInfoIndex = 0;

  size_t longestResultingStatLen =
      Util::FindLongestStringLength(resultingValuesStringList);

  for (auto& i : statInfoStrings)
  {
    std::stringstream ss;

    ss << i;

    std::string res = std::to_string(statInfos[statInfoIndex].ResultingValue);
    size_t origLen = res.length();
    res.insert(0, longestResultingStatLen - origLen, ' ');

    ss << "= " << res;

    WriteObituaryLine(writeTo, ss.str(), asciiMode);

    statInfoIndex++;
  }
}

// =============================================================================

Application::StatInfo Application::GetStatInfo(const std::string& attrName)
{
  StatInfo res;

  for (auto& i : _attrsByName)
  {
    if (i.first == attrName)
    {
      int modifiers = i.second.GetModifiers();

      res.AttrName       = attrName;
      res.Modifier       = modifiers;
      res.OriginalValue  = i.second.OriginalValue();
      res.ResultingValue = i.second.Get();

      break;
    }
  }

  return res;
}

// =============================================================================

bool Application::InitGraphics()
{
#ifdef USE_SDL
  return InitSDL();
#else
  return InitCurses();
#endif
}

// =============================================================================

#ifndef USE_SDL
bool Application::InitCurses()
{
  initscr();
  nodelay(stdscr, true);    // non-blocking getch()
  keypad(stdscr, true);     // enable numpad
  use_extended_names(true); // enable modifier keys
  noecho();
  curs_set(false);

  start_color();

  LoadConfig();

  Game::gPrnt.Init();

  Game::gPrnt.InitMsgBufferObj();

  return Game::gPrnt.IsReady();
}
#endif

// =============================================================================

#ifdef USE_SDL

#include <SDL2/SDL.h>

// =============================================================================

bool Application::InitSDL()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    ConsoleLog("[ERR] SDL_Init Error: '%s'", SDL_GetError());
    return false;
  }

  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

  LoadConfig();

  Game::gPrnt.Init();

  if (!Game::gPrnt.IsReady())
  {
    ConsoleLog("[ERR] failed to initialize Printer subsystem!");
    return false;
  }

  Game::gPrnt.InitMsgBufferObj();

  return true;
}
#endif

// =============================================================================

void Application::LoadConfig()
{
  NRS::LoadResult res = _loadedConfig.Load("config.txt");
  switch (res)
  {
    case NRS::LoadResult::INVALID_FORMAT:
      ConsoleLog("[WAR] config format is invalid - check syntax! "
                 "Will assume default values for now.");
      break;

    case NRS::LoadResult::ERROR:
      ConsoleLog("[WAR] couldn't load config - check if file exists! "
                 "Will assume default values for now.");
      break;

    default:
    {
      DebugLog("Config loaded:\n%s\n", _loadedConfig.ToPrettyString().data());

      std::ifstream tileset(_loadedConfig[kConfigKeyTileset].GetString());
      if (tileset.is_open())
      {
        GameConfig.TilesetFilename =
            _loadedConfig[kConfigKeyTileset].GetString();
      }

      AppData.UseGraphics = !GameConfig.TilesetFilename.empty();

      if (!ParseValue<int>(kConfigKeyTileSize, GameConfig.TileSize))
      {
        ConsoleLog("[WAR] failed to parse value as integer, assuming default");
        GameConfig.TileSize = 16;
      }

      GameConfig.FastCombat =
          (_loadedConfig[kConfigKeyFastCombat].GetString() == "Y");

      GameConfig.FastMonsterMovement =
          (_loadedConfig[kConfigKeyFastMonsterMovement].GetString() == "Y");

      GameConfig.PreserveAspectRatio =
          (_loadedConfig[kConfigKeyPreserveAspect].GetString() == "Y");
    }
    break;
  }
}

// =============================================================================

void Application::Cleanup()
{
#ifdef USE_SDL
  SDL_Quit();
#else
  endwin();
#endif

  //
  // To control objects' order of destruction
  // for dev console's object handle check.
  // Otherwise it might be so that Application is no longer
  // available when we check object handle in ~GameObject().
  //
  Game::gMap.Cleanup();

  _gameStates.clear();

  LogPrint("Application::Cleanup()");

  ConsoleLog("Goodbye!\n");
}

// =============================================================================

void Application::ForceDrawMainState()
{
  if (CurrentStateIs(GameStates::MAIN_STATE))
  {
    _currentState->Update(true);
  }
}

// =============================================================================

void Application::ForceDrawCurrentState()
{
  if (_currentState != nullptr)
  {
    _currentState->Update(true);
  }
}

// =============================================================================

void Application::InitGameStates(bool restart)
{
  if (!restart)
  {
    RegisterState<MenuState>(GameStates::MENU_STATE);
  }

  RegisterState<MainState>             (GameStates::MAIN_STATE);
  RegisterState<InfoState>             (GameStates::INFO_STATE);
  RegisterState<AttackState>           (GameStates::ATTACK_STATE);
  RegisterState<SelectClassState>      (GameStates::SELECT_CLASS_STATE);
  RegisterState<CustomClassState>      (GameStates::CUSTOM_CLASS_STATE);
  RegisterState<EnterNameState>        (GameStates::ENTER_NAME_STATE);
  RegisterState<IntroState>            (GameStates::INTRO_STATE);
  RegisterState<InventoryState>        (GameStates::INVENTORY_STATE);
  RegisterState<HelpState>             (GameStates::HELP_STATE);
  RegisterState<StartGameState>        (GameStates::START_GAME_STATE);
  RegisterState<ContainerInteractState>(GameStates::CONTAINER_INTERACT_STATE);
  RegisterState<MessageLogState>       (GameStates::SHOW_MESSAGES_STATE);
  RegisterState<LookInputState>        (GameStates::LOOK_INPUT_STATE);
  RegisterState<InteractInputState>    (GameStates::INTERACT_INPUT_STATE);
  RegisterState<NPCInteractState>      (GameStates::NPC_INTERACT_STATE);
  RegisterState<ShoppingState>         (GameStates::SHOPPING_STATE);
  RegisterState<ReturnerState>         (GameStates::RETURNER_STATE);
  RegisterState<RepairState>           (GameStates::REPAIR_STATE);
  RegisterState<SaveGameState>         (GameStates::SAVE_GAME_STATE);
  RegisterState<PickupItemState>       (GameStates::PICKUP_ITEM_STATE);
  RegisterState<ExitingState>          (GameStates::EXITING_STATE);
  RegisterState<MessageBoxState>       (GameStates::MESSAGE_BOX_STATE);
  RegisterState<ServiceState>          (GameStates::SERVICE_STATE);
  RegisterState<TargetState>           (GameStates::TARGET_STATE);
  RegisterState<GameOverState>         (GameStates::GAMEOVER_STATE);
  RegisterState<ObituaryReportState>   (GameStates::OBITUARY_REPORT_STATE);

  #ifdef DEBUG_BUILD
  RegisterState<DevConsole>(GameStates::DEV_CONSOLE);
  #endif

  for (auto& state : _gameStates)
  {
    GameState* st = state.second.get();
    st->Init();
  }
}
