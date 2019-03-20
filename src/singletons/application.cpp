#include "application.h"

#include "mainstate.h"
#include "infostate.h"
#include "menustate.h"
#include "select-class-state.h"
#include "enter-name-state.h"
#include "intro-state.h"
#include "inventory-state.h"
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
#include "map.h"
#include "map-level-base.h"

#include <sstream>

void Application::Init()
{
  InitGraphics();

  _gameStates[GameStates::MAIN_STATE]               = std::unique_ptr<GameState>(new MainState());
  _gameStates[GameStates::INFO_STATE]               = std::unique_ptr<GameState>(new InfoState());
  _gameStates[GameStates::ATTACK_STATE]             = std::unique_ptr<GameState>(new AttackState());
  _gameStates[GameStates::MENU_STATE]               = std::unique_ptr<GameState>(new MenuState());
  _gameStates[GameStates::SELECT_CLASS_STATE]       = std::unique_ptr<GameState>(new SelectClassState());
  _gameStates[GameStates::ENTER_NAME_STATE]         = std::unique_ptr<GameState>(new EnterNameState());
  _gameStates[GameStates::INTRO_STATE]              = std::unique_ptr<GameState>(new IntroState());
  _gameStates[GameStates::INVENTORY_STATE]          = std::unique_ptr<GameState>(new InventoryState());
  _gameStates[GameStates::CONTAINER_INTERACT_STATE] = std::unique_ptr<GameState>(new ContainerInteractState());
  _gameStates[GameStates::SHOW_MESSAGES_STATE]      = std::unique_ptr<GameState>(new MessageLogState());
  _gameStates[GameStates::LOOK_INPUT_STATE]         = std::unique_ptr<GameState>(new LookInputState());
  _gameStates[GameStates::INTERACT_INPUT_STATE]     = std::unique_ptr<GameState>(new InteractInputState());
  _gameStates[GameStates::NPC_INTERACT_STATE]       = std::unique_ptr<GameState>(new NPCInteractState());
  _gameStates[GameStates::SHOPPING_STATE]           = std::unique_ptr<GameState>(new ShoppingState());
  _gameStates[GameStates::RETURNER_STATE]           = std::unique_ptr<GameState>(new ReturnerState());
  _gameStates[GameStates::REPAIR_STATE]             = std::unique_ptr<GameState>(new RepairState());
  _gameStates[GameStates::EXITING_STATE]            = std::unique_ptr<GameState>(new ExitingState());
  _gameStates[GameStates::MESSAGE_BOX_STATE]        = std::unique_ptr<GameState>(new MessageBoxState());
  _gameStates[GameStates::TARGET_STATE]             = std::unique_ptr<GameState>(new TargetState());
  _gameStates[GameStates::ENDGAME_STATE]            = std::unique_ptr<GameState>(new EndgameState());

  for (auto& state : _gameStates)
  {
    state.second.get()->Init();
  }

  _currentState = _gameStates[GameStates::MENU_STATE].get();

  Printer::Instance().AddMessage("You begin your quest");
}

void Application::Run()
{
  while (_currentState != nullptr)
  {
    // Since change state happens in HandleInput, if it's called before Update
    // to exit game (change state to nullptr) we get segfault because
    // _currentState->Update() gets called on nullptr.
    //
    // NOTE: Probably still a bad idea to just change order of methods call,
    // since we might get the same situation in Update().

    _currentState->Update();
    _currentState->HandleInput();    
  }
}

void Application::ChangeState(const GameStates& gameStateIndex)
{
  if (gameStateIndex != GameStates::EXIT_GAME)
  {
    auto str = Util::StringFormat("Changing state: %s [0x%X] => %s [0x%X]",
                                  typeid(*_currentState).name(), _currentState,
                                  typeid(*_gameStates[gameStateIndex].get()).name(),
                                  _gameStates[gameStateIndex].get());
    Logger::Instance().Print(str);
  }
  else
  {
    auto str = Util::StringFormat("Changing state: %s [0x%X] => EXIT_GAME", typeid(*_currentState).name(), _currentState);
    Logger::Instance().Print(str);
  }

  _currentState->Cleanup();

  _currentState = (gameStateIndex == GameStates::EXIT_GAME) ? nullptr : _gameStates[gameStateIndex].get();

  if (_currentState != nullptr)
  {
    _currentState->Prepare();    
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

void Application::ShowMessageBox(MessageBoxType type, std::string header, std::vector<std::string> message,
                                 std::string borderColor, std::string bgColor)
{
  _previousState = _currentState;

  auto ptr = _gameStates[GameStates::MESSAGE_BOX_STATE].get();
  MessageBoxState* mbs = static_cast<MessageBoxState*>(ptr);
  mbs->SetMessage(type, header, message, borderColor, bgColor);

  _currentState = ptr;
}

void Application::CloseMessageBox()
{
  _currentState = _previousState;
}

void Application::DisplayAttack(GameObject* defender, int delayMs, const std::string& messageToPrint, const std::string& cursorColor)
{
  int posX = defender->PosX + Map::Instance().CurrentLevel->MapOffsetX;
  int posY = defender->PosY + Map::Instance().CurrentLevel->MapOffsetY;

  DrawAttackCursor(posX, posY, defender, cursorColor);
  Util::Sleep(delayMs);

  Printer::Instance().AddMessage(messageToPrint);

  DrawAttackCursor(posX, posY, defender);
  Util::Sleep(delayMs);
}

void Application::DrawAttackCursor(int x, int y, GameObject* defender, const std::string& cursorColor)
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
  Player* playerRef = &PlayerInstance;

  std::ofstream postMortem;
  std::stringstream ss;

  ss << "obituary.txt";

  postMortem.open(ss.str());

  ss.str(std::string());

  MapLevelBase* curLvl = Map::Instance().CurrentLevel;

  std::string playerEndCause = wasKilled ? "has perished at " : "has quit on ";

  // Write part of the map with player

  int px = playerRef->PosX;
  int py = playerRef->PosY;

  curLvl->MapArray[px][py]->Image = wasKilled ? '%' : '@';

  int range = 10;

  int lx = playerRef->PosX - range;
  int ly = playerRef->PosY - range;
  int hx = playerRef->PosX + range;
  int hy = playerRef->PosY + range;

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

        if (curLvl->MapArray[x][y]->Visible
         || curLvl->MapArray[x][y]->Revealed)
        {
          ch = curLvl->MapArray[x][y]->Image;

          // If walls are ' ', display them as '#'
          if (curLvl->MapArray[x][y]->Blocking
           && curLvl->MapArray[x][y]->BlocksSight
           && ch == ' ')
          {
            ch = '#';
          }
        }

        auto gos = Map::Instance().GetGameObjectsAtPosition(x, y);
        if (gos.size() != 0
         && (curLvl->MapArray[x][y]->Visible
          || curLvl->MapArray[x][y]->Revealed))
        {
          ch = gos.back()->Image;
        }

        auto actor = Map::Instance().GetActorAtPosition(x, y);
        if (actor != nullptr
         && (curLvl->MapArray[x][y]->Visible
          || curLvl->MapArray[x][y]->Revealed))
        {
          ch = actor->Image;
        }

        row.push_back(ch);
      }
    }

    map.push_back(row);
  }

  for (int x = 0; x < map.size(); x++)
  {
    for (int y = 0; y < map[x].size(); y++)
    {
      ss << map[x][y];
    }

    ss << '\n';
  }

  ss << '\n';

  // Form obituary

  ss << "********** OBITUARY **********\n\n";

  std::string nameAndTitle = playerRef->Name + " the " + playerRef->GetClassName();

  ss << nameAndTitle << " of level " << playerRef->Attrs.Lvl.CurrentValue << '\n';
  ss << playerEndCause << curLvl->LevelName << "\n\n";

  ss << "STR " << playerRef->Attrs.Str.CurrentValue << '\n';
  ss << "DEF " << playerRef->Attrs.Def.CurrentValue << '\n';
  ss << "MAG " << playerRef->Attrs.Mag.CurrentValue << '\n';
  ss << "RES " << playerRef->Attrs.Res.CurrentValue << '\n';
  ss << "SKL " << playerRef->Attrs.Skl.CurrentValue << '\n';
  ss << "SPD " << playerRef->Attrs.Spd.CurrentValue << '\n';

  ss << '\n';
  ss << "********** POSSESSIONS **********\n\n";

  int stringResizeWidth = 40;

  for (auto& i : playerRef->Inventory.Contents)
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

  ss << '\n';
  ss << "********** KILLS **********\n\n";

  for (auto& kvp : playerRef->TotalKills)
  {
    std::string name = kvp.first;
    name.resize(stringResizeWidth, ' ');

    ss << name << " " << kvp.second << '\n';
  }

  postMortem << ss.str();

  postMortem.close();
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
#include <SDL2/SDL_image.h>

void Application::InitSDL()
{  
  int tilesetWidth = 0;
  int tilesetHeight = 0;

  const int kTerminalWidth = 80;
  const int kTerminalHeight = 24;

  if (SDL_Init(SDL_INIT_VIDEO) != 0)
  {
    printf("SDL_Init Error: %s\n", SDL_GetError());
    return;
  }

  std::ifstream config("config.txt");
  if (config.is_open())
  {
    std::string line;
    while (std::getline(config, line))
    {
      auto res = Util::StringSplit(line, '=');
      if (res.size() > 1)
      {
        _config.emplace(res[0], res[1]);
      }
    }
  }

  config.close();

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
  else
  {
    TileWidth = 8;
    TileHeight = 16;
  }

  if (_config.count("SCALE") != 0)
  {
    ScaleFactor = std::stof(_config["SCALE"]);
  }

  int scaledW = (int)((float)TileWidth * ScaleFactor);
  int scaledH = (int)((float)TileHeight * ScaleFactor);

  int ww = kTerminalWidth * scaledW;
  int wh = kTerminalHeight * scaledH;

  WindowWidth = ww;
  WindowHeight = wh;

  Window = SDL_CreateWindow("nrogue",
                            50, 50,
                            ww, wh,
                            SDL_WINDOW_SHOWN);

  int drivers = SDL_GetNumRenderDrivers();

  for (int i = 0; i < drivers; i++)
  {
    SDL_RendererInfo info;
    SDL_GetRenderDriverInfo(i, &info);

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

  IMG_Init(IMG_INIT_PNG);

  Printer::Instance().Init();

  Printer::Instance().TerminalWidth = kTerminalWidth;
  Printer::Instance().TerminalHeight = kTerminalHeight;
}
#endif

void Application::Cleanup()
{
#ifdef USE_SDL  
  IMG_Quit();
  SDL_Quit();
#else
  endwin();
#endif

  Logger::Instance().Print("Application::Cleanup()");

  printf("Goodbye!\n");
}
