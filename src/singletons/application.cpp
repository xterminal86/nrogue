#include "application.h"

#include "main-state.h"
#include "info-state.h"
#include "menu-state.h"
#include "select-class-state.h"
#include "custom-class-state.h"
#include "enter-name-state.h"
#include "intro-state.h"
#include "inventory-state.h"
#include "help-state.h"
#include "container-interact-state.h"
#include "message-log-state.h"
#include "look-input-state.h"
#include "interact-input-state.h"
#include "npc-interact-state.h"
#include "shopping-state.h"
#include "returner-state.h"
#include "repair-state.h"
#include "exiting-state.h"
#include "message-box-state.h"
#include "endgame-state.h"
#include "attack-state.h"
#include "target-state.h"
#include "service-state.h"
#include "map.h"
#include "map-level-base.h"
#include "printer.h"
#include "timer.h"
#include "logger.h"

#ifdef DEBUG_BUILD
#include "dev-console.h"
#endif

void Application::Init()
{
  if (_initialized)
  {
    return;
  }

  PrepareChars();
  InitGraphics();
  InitGameStates();

  _currentState = _gameStates[GameStates::MENU_STATE].get();

  // In SDL build GetKeyDown() will return -1 on application
  // start resulting in white screen during to not
  // redundantly drawing the "scene"
  // because no key has been pressed yet.
  _currentState->Update(true);

  PlayerInstance.Attrs.Indestructible = false;

  Printer::Instance().AddMessage("You begin your quest");
  Printer::Instance().AddMessage("Press 'h' for help");

  _initialized = true;
}

void Application::Run()
{
  while (_currentState != nullptr)
  {
    Timer::Instance().MeasureStart();

    if (PlayerInstance.CanMove() || !PlayerInstance.IsAlive())
    {
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
      // If player has levelled up, stop updating
      // everything until message box is closed,
      // or we can get attack animations on top of message box.
      if (CurrentStateIs(GameStates::MESSAGE_BOX_STATE))
      {
        _currentState->HandleInput();
      }
      else
      {
        Map::Instance().UpdateGameObjects();
        PlayerInstance.WaitForTurn();
      }
    }

    Timer::Instance().MeasureEnd();
  }
}

void Application::ChangeState(const GameStates& gameStateIndex)
{
  // Don't self-change
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
    Logger::Instance().Print(str);
    //DebugLog("%s\n", str.data());
  }
  else
  {
    auto str = Util::StringFormat("Changing state: %s [0x%X] => EXIT_GAME [0x0]", typeid(*_currentState).name(), _currentState);
    Logger::Instance().Print(str);
    //DebugLog("%s\n", str.data());
  }

  _currentState->Cleanup();

  _currentState = (gameStateIndex == GameStates::EXIT_GAME) ? nullptr : _gameStates[gameStateIndex].get();

  if (_currentState != nullptr)
  {
    _currentState->Prepare();

    // I don't know how it worked before without this line
    _currentState->Update(true);
  }
}

GameState* Application::GetGameStateRefByName(GameStates stateName)
{
  if (_gameStates.count(stateName) == 1)
  {
    return _gameStates[stateName].get();
  }

  return nullptr;
}

bool Application::CurrentStateIs(GameStates stateName)
{
  if (_gameStates.count(stateName) == 1)
  {
    return (_currentState == _gameStates[stateName].get());
  }

  return false;
}

void Application::ShowMessageBox(MessageBoxType type,
                                 const std::string& header,
                                 const std::vector<std::string>& message,
                                 const std::string& borderColor,
                                 const std::string& bgColor)
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

void Application::CloseMessageBox()
{
  _currentState = _previousState;
}

void Application::DisplayAttack(GameObject* defender,
                                int delayMs,
                                const std::string& messageToPrint,
                                const std::string& cursorColor)
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

void Application::DrawAttackCursor(int x, int y,
                                   GameObject* defender,
                                   const std::string& cursorColor)
{
  if (cursorColor.length() == 0)
  {
    Printer::Instance().PrintFB(x, y, defender->Image, defender->FgColor, defender->BgColor);
    Printer::Instance().Render();
  }
  else
  {
    Printer::Instance().PrintFB(x, y, ' ', "#000000", cursorColor);
    Printer::Instance().Render();
  }
}

void Application::WriteObituary(bool wasKilled)
{
  std::ofstream postMortem;
  std::stringstream ss;

  ss << "obituary.txt";

  postMortem.open(ss.str());

  ss.str(std::string());

  MapLevelBase* curLvl = Map::Instance().CurrentLevel;

  std::string playerEndCause = wasKilled ? "has perished at " : "has quit at ";

  // Write part of the map with player

  SaveMapAroundPlayer(ss, wasKilled);

  ss << '\n';

  // Form obituary

  ss << "********** OBITUARY **********\n\n";

  ss << "World seed was: 0x" << RNG::Instance().GetSeedAsHex() << "\n\n";

  std::string nameAndTitle = PlayerInstance.Name + " the " + PlayerInstance.GetClassName();

  ss << nameAndTitle << " of level " << PlayerInstance.Attrs.Lvl.Get() << '\n';
  ss << playerEndCause << curLvl->LevelName << "\n\n";

  ss << "He survived " << TurnsPassed << " turns\n\n";

  ss << "HP " << PlayerInstance.Attrs.HP.Min().Get() << " / " << PlayerInstance.Attrs.HP.Max().Get() << '\n';
  ss << "MP " << PlayerInstance.Attrs.MP.Min().Get() << " / " << PlayerInstance.Attrs.MP.Max().Get() << '\n';

  ss << '\n';

  SavePrettyAlignedStatInfo(ss);

  ss << '\n';
  ss << "********** POSSESSIONS **********\n\n";

  size_t stringResizeWidth = SavePossessions(ss);

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

size_t Application::SavePossessions(std::stringstream& ss)
{
  size_t stringResizeWidth = 0;
  for (auto& i : PlayerInstance.Inventory.Contents)
  {
    ItemComponent* ic = i->GetComponent<ItemComponent>();
    std::string name = ic->Data.IdentifiedName;
    if (name.length() > stringResizeWidth)
    {
      stringResizeWidth = name.length();
    }
  }

  for (auto& i : PlayerInstance.Inventory.Contents)
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

  std::vector<std::vector<char>> map;

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

          // If walls are ' ', display them as '#'
          if (curLvl->MapArray[x][y]->Blocking
           && curLvl->MapArray[x][y]->BlocksSight
           && ch == ' ')
          {
            ch = '#';
          }

          // Check items first
          auto gos = Map::Instance().GetGameObjectsAtPosition(x, y);
          if (!gos.empty())
          {
            ch = gos.back()->Image;

            if (ch == ' ')
            {
              ch = 'o';
            }
          }

          // If there are no objects lying above static game object,
          // draw static game object
          if (gos.empty())
          {
            auto so = Map::Instance().GetStaticGameObjectAtPosition(x, y);
            if (so != nullptr)
            {
              ch = (so->Image == ' ') ? '#' : so->Image;
            }
          }

          // if actor is standing on this cell, draw him instead.
          auto actor = Map::Instance().GetActorAtPosition(x, y);
          if (actor != nullptr)
          {
            bool imageNonPrintable = (actor->Image < 33);
            ch = (imageNonPrintable ? '@' : actor->Image);
          }

          // If character is not printable, replace it with x
          if (ch < 32)
          {
            ch = 'x';
          }
        }

        // Draw player on top of everything
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

void Application::SavePrettyAlignedStatInfo(std::stringstream& ss)
{
  const std::vector<std::string> statNames =
  {
    "STR", "DEF", "MAG", "RES", "SKL", "SPD"
  };

  std::vector<std::string> statInfoStrings;
  std::vector<StatInfo> statInfos;

  for (auto& i : statNames)
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

Application::StatInfo Application::GetStatInfo(const std::string& attrName)
{
  std::map<std::string, Attribute&> attrsByName =
  {
    { "STR", PlayerInstance.Attrs.Str },
    { "DEF", PlayerInstance.Attrs.Def },
    { "MAG", PlayerInstance.Attrs.Mag },
    { "RES", PlayerInstance.Attrs.Res },
    { "SKL", PlayerInstance.Attrs.Skl },
    { "SPD", PlayerInstance.Attrs.Spd }
  };

  StatInfo res;

  for (auto& i : attrsByName)
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

void Application::InitGraphics()
{
#ifdef USE_SDL
  InitSDL();
#else
  InitCurses();
#endif
}

#ifndef USE_SDL
void Application::InitCurses()
{
  initscr();
  nodelay(stdscr, true);     // non-blocking getch()
  keypad(stdscr, true);      // enable numpad
  noecho();
  curs_set(false);

  start_color();

  Printer::Instance().Init();
}
#endif

#ifdef USE_SDL

#include <SDL2/SDL.h>

void Application::ParseConfig()
{
  std::ifstream configFile("config.txt");
  if (configFile.is_open())
  {
    std::string line;
    while (std::getline(configFile, line))
    {
      auto res = Util::StringSplit(line, '=');
      if (res.size() > 1)
      {
        _config.emplace(res[0], res[1]);
      }
    }

    configFile.close();
  }

  /*
  for (auto& kvp : _config)
  {
    DebugLog("%s = %s\n", kvp.first.data(), kvp.second.data());
  }
  */
}

void Application::ProcessConfig()
{
  // Trying to get values from config map
  if (_config.count("FILE") != 0)
  {
    TilesetFilename = _config["FILE"];
  }

  if (_config.count("TILE_W") != 0
   && _config.count("TILE_H") != 0)
  {
    TileWidth = std::stoi(_config["TILE_W"]);
    TileHeight = std::stoi(_config["TILE_H"]);
  }

  if (_config.count("SCALE") != 0)
  {
    ScaleFactor = std::stof(_config["SCALE"]);
  }

  // If tileset file can't be opened,
  // fallback to default '8x16' scaled window size.
  // Scale can still be read.
  std::ifstream tileset(_config["FILE"]);
  if (!tileset.is_open())
  {
    TileWidth = 8;
    TileHeight = 16;
  }

  tileset.close();
}

void Application::SetIcon()
{
  auto res = Util::Base64_Decode(Strings::IconBase64);
  auto bytes = Util::ConvertStringToBytes(res);
  SDL_RWops* data = SDL_RWFromMem(bytes.data(), bytes.size());
  SDL_Surface* surf = SDL_LoadBMP_RW(data, 1);
  if (!surf)
  {
    auto str = Util::StringFormat("***** Could not load from memory: %s *****\n", SDL_GetError());
    Logger::Instance().Print(str);
    DebugLog("%s\n", str.data());
    return;
  }

  SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, 0xFF, 0, 0xFF));
  SDL_SetWindowIcon(Window, surf);
  SDL_FreeSurface(surf);
}

void Application::InitSDL()
{
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    DebugLog("SDL_Init Error: %s\n", SDL_GetError());
    return;
  }

  SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

  ParseConfig();
  ProcessConfig();

  SDL_Rect rect = GetWindowSize(TileWidth, TileHeight);

  _defaultWindowSize = { rect.w, rect.h };

  Window = SDL_CreateWindow("nrogue",
                            rect.x, rect.y,
                            rect.w, rect.h,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

  int drivers = SDL_GetNumRenderDrivers();
  for (int i = 0; i < drivers; i++)
  {
    SDL_RendererInfo info;
    SDL_GetRenderDriverInfo(i, &info);

    //
    // Create double buffer by searching for SDL_RENDERER_TARGETTEXTURE
    //
    if (info.flags & SDL_RENDERER_TARGETTEXTURE)
    {
      Renderer = SDL_CreateRenderer(Window, i, info.flags);
      if (Renderer != nullptr)
      {
        break;
      }
    }
  }

  SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);

  SetIcon();

  Printer::Instance().Init();

  Printer::Instance().SetRenderDst({ 0, 0, _defaultWindowSize.first, _defaultWindowSize.second });

  Printer::TerminalWidth  = GlobalConstants::TerminalWidth;
  Printer::TerminalHeight = GlobalConstants::TerminalHeight;
}

const std::pair<int, int>& Application::GetDefaultWindowSize()
{
  return _defaultWindowSize;
}

///
/// Returns SDL_Rect with initial window position in x, y
/// and window size in w, h
///
SDL_Rect Application::GetWindowSize(int tileWidth, int tileHeight)
{
  SDL_Rect res;

  int scaledW = (int)((float)tileWidth * ScaleFactor);
  int scaledH = (int)((float)tileHeight * ScaleFactor);

  // ===========================================================================
  //
  // This is just a plain hack to fit
  // graphics tileset to screen at certain resolution
  //
  if (scaledW == 32)
  {
    GlobalConstants::TerminalWidth = 59;
  }

  if (tileWidth == tileHeight)
  {
    GlobalConstants::TerminalHeight = GlobalConstants::TerminalWidth / 2 + 2;
  }

  // ===========================================================================

  int ww = GlobalConstants::TerminalWidth * scaledW;
  int wh = GlobalConstants::TerminalHeight * scaledH;

  WindowWidth = ww;
  WindowHeight = wh;

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

  Logger::Instance().Print("Application::Cleanup()");

  DebugLog("Goodbye!\n");
}

void Application::ForceDrawMainState()
{
  if (CurrentStateIs(GameStates::MAIN_STATE))// && PlayerInstance.AreEnemiesInRange())
  {
    _currentState->Update(true);
  }
}

void Application::ForceDrawCurrentState()
{
  if (_currentState != nullptr)
  {
    _currentState->Update(true);
  }
}

void Application::InitGameStates()
{
  RegisterState<MainState>             (GameStates::MAIN_STATE);
  RegisterState<InfoState>             (GameStates::INFO_STATE);
  RegisterState<AttackState>           (GameStates::ATTACK_STATE);
  RegisterState<MenuState>             (GameStates::MENU_STATE);
  RegisterState<SelectClassState>      (GameStates::SELECT_CLASS_STATE);
  RegisterState<CustomClassState>      (GameStates::CUSTOM_CLASS_STATE);
  RegisterState<EnterNameState>        (GameStates::ENTER_NAME_STATE);
  RegisterState<IntroState>            (GameStates::INTRO_STATE);
  RegisterState<InventoryState>        (GameStates::INVENTORY_STATE);
  RegisterState<HelpState>             (GameStates::HELP_STATE);
  RegisterState<ContainerInteractState>(GameStates::CONTAINER_INTERACT_STATE);
  RegisterState<MessageLogState>       (GameStates::SHOW_MESSAGES_STATE);
  RegisterState<LookInputState>        (GameStates::LOOK_INPUT_STATE);
  RegisterState<InteractInputState>    (GameStates::INTERACT_INPUT_STATE);
  RegisterState<NPCInteractState>      (GameStates::NPC_INTERACT_STATE);
  RegisterState<ShoppingState>         (GameStates::SHOPPING_STATE);
  RegisterState<ReturnerState>         (GameStates::RETURNER_STATE);
  RegisterState<RepairState>           (GameStates::REPAIR_STATE);
  RegisterState<ExitingState>          (GameStates::EXITING_STATE);
  RegisterState<MessageBoxState>       (GameStates::MESSAGE_BOX_STATE);
  RegisterState<ServiceState>          (GameStates::SERVICE_STATE);
  RegisterState<TargetState>           (GameStates::TARGET_STATE);
  RegisterState<EndgameState>          (GameStates::ENDGAME_STATE);

  #ifdef DEBUG_BUILD
  RegisterState<DevConsole>(GameStates::DEV_CONSOLE);
  #endif

  for (auto& state : _gameStates)
  {
    state.second->Init();
  }
}

uint64_t Application::GetNewId()
{
  static uint64_t globalId = 1;
  return globalId++;
}

void Application::PrepareChars()
{
  for (int i = 0; i < 128; i++)
  {
    _charByCharIndex[i] = i;
  }
}

const char& Application::CharByCharIndex(uint8_t index)
{
  return _charByCharIndex[index];
}
