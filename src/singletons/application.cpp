#include "application.h"
#include "gid-generator.h"

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

// -----------------------------------------------------------------------------

#include "map.h"
#include "map-level-base.h"
#include "printer.h"
#include "timer.h"
#include "util.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#include "dev-console.h"
#endif

#include <fstream>

void Application::InitSpecific()
{
  if (!InitGraphics())
  {
    return;
  }

  InitGameStates();

  _currentState = _gameStates[GameStates::MENU_STATE].get();

  //
  // In SDL build GetKeyDown() will return -1 on application
  // start resulting in white screen during to not
  // redundantly drawing the "scene"
  // because no key has been pressed yet.
  //
  _currentState->Update(true);

  PlayerInstance.Attrs.Indestructible = false;

  Printer::Instance().AddMessage("You begin your quest");
  Printer::Instance().AddMessage("Press 'h' for help");

  _appReady = true;
}

// =============================================================================

void Application::Run()
{
  while (_currentState != nullptr)
  {
    Timer::Instance().MeasureStart();

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
        Map::Instance().Update();
        PlayerInstance.WaitForTurn();

        MapUpdateCyclesPassed++;
      }
    }

    Timer::Instance().MeasureEnd();
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

  if (gameStateIndex != GameStates::EXIT_GAME)
  {
    auto str = Util::StringFormat("Changing state: %s [0x%X] => %s [0x%X]",
                                  typeid(*_currentState).name(), _currentState,
                                  typeid(*_gameStates[gameStateIndex].get()).name(),
                                  _gameStates[gameStateIndex].get());
    LogPrint(str);
    //DebugLog("%s\n", str.data());
  }
  else
  {
    auto str = Util::StringFormat("Changing state: %s [0x%X] => EXIT_GAME [0x0]", typeid(*_currentState).name(), _currentState);
    LogPrint(str);
    //DebugLog("%s\n", str.data());
  }

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

  /*
  std::vector<std::string> totalMessage = message;
  if (type == MessageBoxType::WAIT_FOR_INPUT)
  {
    totalMessage.push_back("");
    totalMessage.push_back("'Enter' to close");
  }
  */

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
      Printer::Instance().AddMessage(messageToPrint);
    }
  }
  else
  {
    int posX = defender->PosX + Map::Instance().CurrentLevel->MapOffsetX;
    int posY = defender->PosY + Map::Instance().CurrentLevel->MapOffsetY;

    DrawAttackCursor(posX, posY, defender, cursorColor);

    Util::Sleep(delayMs);

    if (messageToPrint.length() != 0)
    {
      Printer::Instance().AddMessage(messageToPrint);
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
  if (cursorColor == Colors::None)
  {
    if (defender->FgColor != Colors::None
     && defender->BgColor != Colors::None)
    {
      Printer::Instance().PrintFB(x, y, defender->Image, defender->FgColor, defender->BgColor);
      Printer::Instance().Render();
    }
  }
  else
  {
    Printer::Instance().PrintFB(x, y, ' ', Colors::BlackColor, cursorColor);
    Printer::Instance().Render();
  }
}

// =============================================================================

void Application::WriteObituary(bool wasKilled)
{
  std::ofstream postMortem;
  std::stringstream ss;

  ss << "obituary.txt";

  postMortem.open(ss.str());

  ss.str(std::string());

  MapLevelBase* curLvl = Map::Instance().CurrentLevel;

  std::string playerEndCause = wasKilled
                            ? "has perished at "
                            : "has quit at ";

  //
  // Write part of the map with player.
  //
  SaveMapAroundPlayer(ss, wasKilled);

  ss << '\n';

  //
  // Form obituary.
  //

  ss << "********** OBITUARY **********\n\n";

  ss << "World seed was: 0x" << RNG::Instance().GetSeedAsHex()
     << " (" << RNG::Instance().GetSeedString().first << ")"
     << "\n\n";

  auto nameAndTitle = Util::StringFormat("%s the %s",
                                          PlayerInstance.Name.data(),
                                          PlayerInstance.GetClassName().data());

  ss << nameAndTitle << " of level " << PlayerInstance.Attrs.Lvl.Get() << '\n';
  ss << playerEndCause << curLvl->LevelName << "\n\n";

  ss << "He survived " << PlayerTurnsPassed << " turns\n\n";

  ss << "HP " << PlayerInstance.Attrs.HP.Min().Get()
     << " / " << PlayerInstance.Attrs.HP.Max().Get() << '\n';

  ss << "MP " << PlayerInstance.Attrs.MP.Min().Get()
     << " / " << PlayerInstance.Attrs.MP.Max().Get() << '\n';

  ss << '\n';

  SavePrettyAlignedStatInfo(ss);

  ss << '\n';
  ss << "********** POSSESSIONS **********\n\n";

  size_t stringResizeWidth = WritePossessions(ss);

  ss << '\n';
  ss << "**********    KILLS    **********\n\n";

  for (auto& kvp : PlayerInstance.TotalKills)
  {
    std::string name = kvp.first;
    name.resize(stringResizeWidth, ' ');

    ss << name << " " << kvp.second << '\n';
  }

  postMortem << ss.str();

  postMortem.close();
}

// =============================================================================

void Application::SaveGame()
{
  using C  = std::chrono::system_clock;
  using TP = std::chrono::time_point<C>;

  NRS save;

  DebugLog("saving game...");

  TP before = C::now();

  SaveBaseStuff(save);

  //
  // FIXME: one level for now
  //
  Map::Instance().CurrentLevel->Serialize(save);

  SavePlayer(save);

  if (!save.Save(Strings::SaveFileName))
  {
    ConsoleLog("Couldn't save at %s !", Strings::SaveFileName.data());
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

  root[SK::Gid].SetUInt(GID::Instance().GetCurrentGlobalId());
  {
    NRS& node = root[SK::Seed];

    node[SK::Name].SetString(RNG::Instance().GetSeedString().first);
    node[SK::Value].SetUInt(RNG::Instance().Seed);
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

size_t Application::WritePossessions(std::stringstream& ss)
{
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

    ss << name;

    if (ic->Data.IsStackable || ic->Data.IsChargeable)
    {
      ss << " (" << ic->Data.Amount << ")";
    }

    if (ic->Data.IsEquipped)
    {
      ss << " (E)";
    }

    ss << '\n';
  }

  return stringResizeWidth;
}

// =============================================================================

void Application::SaveMapAroundPlayer(std::stringstream& ss, bool wasKilled)
{
  MapLevelBase* curLvl = Map::Instance().CurrentLevel;

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
          auto gos = Map::Instance().GetGameObjectsAtPosition(x, y);
          if (!gos.empty())
          {
            ch = gos.back()->Image;

            if (ch == ' ')
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
            auto so = Map::Instance().GetStaticGameObjectAtPosition(x, y);
            if (so != nullptr)
            {
              ch = (so->Image == ' ') ? '#' : so->Image;
            }
          }

          //
          // If actor is standing on this cell, draw him instead.
          //
          auto actor = Map::Instance().GetActorAtPosition(x, y);
          if (actor != nullptr)
          {
            bool imageNonPrintable = (actor->Image < 33);
            ch = (imageNonPrintable ? '@' : actor->Image);
          }

          //
          // If character is not printable, replace it with 'x' character.
          //
          if (ch < 32)
          {
            ch = 'x';
          }
        }

        //
        // Draw player on top of everything.
        //
        if (isPlayer)
        {
          if (x == px && y == py)
          {
            ch = wasKilled ? '%' : '@';
          }
        }

        row.push_back(ch);
      }
    }

    map.push_back(row);
  }

  for (size_t x = 0; x < map.size(); x++)
  {
    for (size_t y = 0; y < map[x].size(); y++)
    {
      ss << map[x][y];
    }

    ss << '\n';
  }
}

// =============================================================================

void Application::SavePrettyAlignedStatInfo(std::stringstream& ss)
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

  size_t longestResultingStatLen = Util::FindLongestStringLength(resultingValuesStringList);
  for (auto& i : statInfoStrings)
  {
    ss << i;

    std::string res = std::to_string(statInfos[statInfoIndex].ResultingValue);
    size_t origLen = res.length();
    res.insert(0, longestResultingStatLen - origLen, ' ');

    ss << "= " << res << '\n';

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
  nodelay(stdscr, true);     // non-blocking getch()
  keypad(stdscr, true);      // enable numpad
  noecho();
  curs_set(false);

  start_color();

  LoadConfig();

  Printer::Instance().Init();

  return true;
}
#endif

// =============================================================================

#ifdef USE_SDL

#include <SDL2/SDL.h>

#include "base64-strings.h"

void Application::SetIcon()
{
  auto res = Util::Base64_Decode(Base64Strings::IconBase64);
  auto bytes = Util::ConvertStringToBytes(res);
  SDL_RWops* data = SDL_RWFromMem(bytes.data(), bytes.size());
  SDL_Surface* surf = SDL_LoadBMP_RW(data, 1);
  if (!surf)
  {
    auto str = Util::StringFormat("***** Could not load from memory: %s *****\n", SDL_GetError());
    LogPrint(str);
    ConsoleLog("%s\n", str.data());
    return;
  }

  SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0, 0xFF));
  SDL_SetWindowIcon(Window, surf);
  SDL_FreeSurface(surf);
}

// =============================================================================

bool Application::InitSDL()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    ConsoleLog("SDL_Init Error: %s\n", SDL_GetError());
    return false;
  }

  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

  LoadConfig();

  SDL_Rect rect = GetWindowSize(GameConfig.TileWidth,
                                GameConfig.TileHeight);

  _defaultWindowSize = { rect.w, rect.h };

  _resizedWindowSize = _defaultWindowSize;

  Window = SDL_CreateWindow("nrogue",
                            rect.x, rect.y,
                            rect.w, rect.h,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  if (Window == nullptr)
  {
    ConsoleLog("SDL_CreateWindow fail: %s", SDL_GetError());
    return false;
  }

  //
  // NOTE: it looks like "direct3d" is Direct3D 9 (which is kinda slow
  // and reports some weird error / warning in stdout after maximizing
  // the window).
  // "direct3d11" works OK, but produces another weird behaviour:
  // when monster attacks player, attack animation displays every other time
  // while player's attack animation works fine.
  // "direct3d12" seems to have no such issues, but WTF is this?!
  // What the hell is one supposed to choose?!
  //

#if defined(MSVC_COMPILER) || defined(__WIN64__) || defined(__WIN32__)
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d12");
#else
  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
#endif

  Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
  if (Renderer == nullptr)
  {
    ConsoleLog("SDL_CreateRenderer() fail: %s", SDL_GetError());
    ConsoleLog("Trying software mode...");

    Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE);
    if (Renderer == nullptr)
    {
      ConsoleLog("SDL_CreateRenderer() fail: %s", SDL_GetError());
      return false;
    }
  }

  SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

  SetIcon();

  Printer::Instance().Init();

  if (!Printer::Instance().IsReady())
  {
    ConsoleLog("Printer failed to initialize!");
    return false;
  }

  Printer::Instance().SetRenderDst({ 0, 0, _defaultWindowSize.first, _defaultWindowSize.second });

  Printer::TerminalWidth  = GlobalConstants::TerminalWidth;
  Printer::TerminalHeight = GlobalConstants::TerminalHeight;

  return true;
}

// =============================================================================

const std::pair<int, int>& Application::GetDefaultWindowSize()
{
  return _defaultWindowSize;
}

// =============================================================================

std::pair<int, int>& Application::GetResizedWindowSize()
{
  return _resizedWindowSize;
}

// =============================================================================

//
// Returns SDL_Rect with initial window position in x, y
// and window size in w, h
//
SDL_Rect Application::GetWindowSize(int tileWidth, int tileHeight)
{
  SDL_Rect res;

  int scaledW = (int)((double)tileWidth * GameConfig.ScaleFactor);
  int scaledH = (int)((double)tileHeight * GameConfig.ScaleFactor);

  // ---------------------------------------------------------------------------
  //
  // This is just a plain hack to fit
  // graphics tileset to screen at certain resolution
  //
  if (scaledW == 32)
  {
    GlobalConstants::TerminalWidth = 60;
  }

  if (tileWidth == tileHeight)
  {
    GlobalConstants::TerminalHeight = GlobalConstants::TerminalWidth / 2;
  }

  // ---------------------------------------------------------------------------

  int ww = GlobalConstants::TerminalWidth  * scaledW;
  int wh = GlobalConstants::TerminalHeight * scaledH;

  GameConfig.WindowWidth  = ww;
  GameConfig.WindowHeight = wh;

  res.w = ww;
  res.h = wh;

  SDL_DisplayMode dm;
  SDL_GetCurrentDisplayMode(0, &dm);

  //
  // Subtract current display size from created window size
  // to determine starting window position,
  // which should be relatively centered.
  //
  int wx = dm.w / 2 - ww / 2;
  int wy = dm.h / 2 - wh / 2;

  res.x = wx;
  res.y = wy;

  return res;
}
#endif

// =============================================================================

void Application::LoadConfig()
{
  GameConfig.TileWidth  = 8;
  GameConfig.TileHeight = 16;

  NRS::LoadResult res = _loadedConfig.Load("config.txt");
  switch (res)
  {
    case NRS::LoadResult::INVALID_FORMAT:
      ConsoleLog("Config format is invalid - check syntax! "
                 "Will assume default values for now.");
      break;

    case NRS::LoadResult::ERROR:
      ConsoleLog("Couldn't load config - check if file exists! "
                 "Will assume default values for now.");
      break;

    default:
    {
      DebugLog("Config loaded:\n%s\n", _loadedConfig.ToPrettyString().data());

      //
      // If tileset file can't be opened,
      // fallback to default '8x16' scaled window size.
      // Scale can still be read.
      //
      std::ifstream tileset(_loadedConfig[kConfigKeyTileset].GetString());
      if (tileset.is_open())
      {
        GameConfig.TilesetFilename = _loadedConfig[kConfigKeyTileset].GetString();
      }

      GameConfig.TileWidth           = std::stoi(_loadedConfig[kConfigKeyTileW].GetString());
      GameConfig.TileHeight          = std::stoi(_loadedConfig[kConfigKeyTileH].GetString());
      GameConfig.ScaleFactor         = std::stod(_loadedConfig[kConfigKeyScale].GetString());
      GameConfig.FastCombat          = (_loadedConfig[kConfigKeyFastCombat].GetString() != "0");
      GameConfig.FastMonsterMovement = (_loadedConfig[kConfigKeyFastMonsterMovement].GetString() != "0");
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
  Map::Instance().Cleanup();

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

  #ifdef DEBUG_BUILD
  RegisterState<DevConsole>(GameStates::DEV_CONSOLE);
  #endif

  for (auto& state : _gameStates)
  {
    GameState* st = state.second.get();
    st->Init();
  }
}
