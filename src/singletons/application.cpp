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
#include "map.h"
#include "map-level-base.h"

#include <sstream>

void Application::Init()
{
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

void Application::DisplayAttack(GameObject* defender, int delayMs, std::string cursorColor)
{
  int posX = defender->PosX + Map::Instance().CurrentLevel->MapOffsetX;
  int posY = defender->PosY + Map::Instance().CurrentLevel->MapOffsetY;

  if (cursorColor.length() == 0)
  {
    Printer::Instance().PrintFB(posX, posY, defender->Image, defender->FgColor, defender->BgColor);
    Printer::Instance().Render();
  }
  else
  {
    Printer::Instance().PrintFB(posX, posY, ' ', "#000000", cursorColor);
    Printer::Instance().Render();
  }

  Util::Sleep(delayMs);
}

void Application::WriteObituary(bool wasKilled)
{
  Player* playerRef = &PlayerInstance;

  std::ofstream postMortem;
  std::stringstream ss;

  time_t now = std::time(nullptr);
  tm *ltm = std::localtime(&now);

  ss << "obituary_";
  ss << (ltm->tm_year + 1900) << "_";
  ss << (ltm->tm_mon + 1) << "_";
  ss << ltm->tm_mday << "_";
  ss << ltm->tm_hour << "_";
  ss << ltm->tm_min << "_";
  ss << ltm->tm_sec << ".txt";

  postMortem.open(ss.str());

  ss.str(std::string());

  MapLevelBase* curLvl = Map::Instance().CurrentLevel;

  std::string playerEndCause = wasKilled ? "has perished at " : "has quit on ";

  // Write part of the map with player

  int px = playerRef->PosX;
  int py = playerRef->PosY;

  curLvl->MapArray[px][py]->Image = '%';

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
        if (gos.size() != 0)
        {
          ch = gos.back()->Image;
        }

        auto actor = Map::Instance().GetActorAtPosition(x, y);
        if (actor != nullptr)
        {
          ch = actor->Image;
        }

        if (x == px && y == py)
        {
          ch = wasKilled ? '%' : '@';
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

  ss << nameAndTitle << " level " << playerRef->Attrs.Lvl.CurrentValue << '\n';
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
