#include "mainstate.h"

#include "application.h"
#include "map.h"
#include "printer.h"
#include "rng.h"
#include "item-component.h"
#include "stairs-component.h"
#include "map-level-base.h"

void MainState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;  
}

void MainState::HandleInput()
{
  _keyPressed = getch();

  if (_playerRef->Attrs.ActionMeter >= 100)
  {
    switch (_keyPressed)
    {
      case NUMPAD_7:
        if (_playerRef->TryToAttack(-1, -1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(-1, -1))
        {
          Map::Instance().CurrentLevel->MapOffsetY++;
          Map::Instance().CurrentLevel->MapOffsetX++;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_8:
        if (_playerRef->TryToAttack(0, -1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(0, -1))
        {
          Map::Instance().CurrentLevel->MapOffsetY++;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_9:
        if (_playerRef->TryToAttack(1, -1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(1, -1))
        {
          Map::Instance().CurrentLevel->MapOffsetY++;
          Map::Instance().CurrentLevel->MapOffsetX--;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_4:
        if (_playerRef->TryToAttack(-1, 0))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(-1, 0))
        {
          Map::Instance().CurrentLevel->MapOffsetX++;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_2:
        if (_playerRef->TryToAttack(0, 1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(0, 1))
        {
          Map::Instance().CurrentLevel->MapOffsetY--;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_6:
        if (_playerRef->TryToAttack(1, 0))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(1, 0))
        {
          Map::Instance().CurrentLevel->MapOffsetX--;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_1:
        if (_playerRef->TryToAttack(-1, 1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(-1, 1))
        {
          Map::Instance().CurrentLevel->MapOffsetY--;
          Map::Instance().CurrentLevel->MapOffsetX++;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_3:
        if (_playerRef->TryToAttack(1, 1))
        {
          _playerRef->FinishTurn();
        }
        else if (_playerRef->Move(1, 1))
        {
          Map::Instance().CurrentLevel->MapOffsetY--;
          Map::Instance().CurrentLevel->MapOffsetX--;

          _playerRef->FinishTurn();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      // wait
      case NUMPAD_5:
        Printer::Instance().AddMessage("You waited...");

        // If player waits, game objects take their turns,
        // while player action meter is not spent.
        //
        // FIXME: think on better approach

        //_playerRef->SubtractActionMeter();
        Map::Instance().UpdateGameObjects();
        break;

      case 'a':        
        Application::Instance().ChangeState(Application::GameStates::ATTACK_STATE);
        break;

      case 'e':
        Application::Instance().ChangeState(Application::GameStates::INVENTORY_STATE);
        break;

      case 'm':
        Application::Instance().ChangeState(Application::GameStates::SHOW_MESSAGES_STATE);
        break;

      case 'l':
        Application::Instance().ChangeState(Application::GameStates::LOOK_INPUT_STATE);
        break;

      case 'i':
        Application::Instance().ChangeState(Application::GameStates::INTERACT_INPUT_STATE);
        break;

      case 'g':
        TryToPickupItem();
        break;

      case '@':
        Application::Instance().ChangeState(Application::GameStates::INFO_STATE);
        break;

      case '?':        
        DisplayHelp();
        break;

      case 'q':
        Application::Instance().ChangeState(Application::GameStates::EXITING_STATE);
        break;

      case '>':
        CheckStairs('>');
        break;

      case '<':
        CheckStairs('<');
        break;

      // TODO: for debug, remove afterwards
      case 'z':
        _playerRef->LevelUp();
        break;

      default:
        break;
    }
  }

  // Update all game objects if player is not ready to act
  if (_playerRef->Attrs.ActionMeter < 100)
  {    
    Map::Instance().UpdateGameObjects();    
    _playerRef->WaitForTurn();
  }
}

void MainState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {    
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    DrawHPMP();

    if (Printer::Instance().ShowLastMessage)
    {
      DisplayGameLog();
    }
    else
    {
      Printer::Instance().ResetMessagesToDisplay();
    }

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth - 1, 0, Map::Instance().CurrentLevel->LevelName, Printer::kAlignRight, "#FFFFFF");

    _debugInfo = Util::StringFormat("World seed: %lu", RNG::Instance().Seed);
    Printer::Instance().PrintFB(0, 0, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

    // NOTE: Some debug info

    _debugInfo = Util::StringFormat("Act: %i Ofst: %i %i: Plr: [%i;%i] Key: %i",
                                    _playerRef->Attrs.ActionMeter,
                                    Map::Instance().CurrentLevel->MapOffsetX,
                                    Map::Instance().CurrentLevel->MapOffsetY,
                                    _playerRef->PosX,
                                    _playerRef->PosY,
                                    _keyPressed);

    Printer::Instance().PrintFB(0, 1, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

    // *****

    Printer::Instance().Render();
  }
}

void MainState::DisplayGameLog()
{
  int x = Printer::Instance().TerminalWidth - 1;
  int y = Printer::Instance().TerminalHeight - 1;

  //Printer::Instance().PrintFB(x, y, Printer::Instance().GetLastMessage(), Printer::kAlignRight, "#FFFFFF");

  int count = 0;
  auto msgs = Printer::Instance().GetLastMessages();
  for (auto& m : msgs)
  {
    Printer::Instance().PrintFB(x, y - count, m, Printer::kAlignRight, "#FFFFFF");
    count++;
  }
}

void MainState::TryToPickupItem()
{
  auto res = Map::Instance().GetGameObjectToPickup(_playerRef->PosX, _playerRef->PosY);
  Component* ic = nullptr;
  if (res.first != -1)
  {
    if (_playerRef->Inventory.Contents.size() == _playerRef->kInventorySize)
    {
      Printer::Instance().AddMessage("Inventory is full!");
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail", { "Inventory is full!" }, GlobalConstants::MessageBoxRedBorderColor);
      return;
    }

    ic = res.second->GetComponent<ItemComponent>();

    auto go = Map::Instance().CurrentLevel->GameObjects[res.first].release();

    _playerRef->Inventory.AddToInventory(go);

    std::string objName = ((ItemComponent*)ic)->Data.IsIdentified ? go->ObjectName : ((ItemComponent*)ic)->Data.UnidentifiedName;

    std::string message;
    if (((ItemComponent*)ic)->Data.IsStackable)
    {
      message = Util::StringFormat("Picked up: %i %s", ((ItemComponent*)ic)->Data.Amount, objName.data());
    }
    else
    {
      message = Util::StringFormat("Picked up: %s", objName.data());
    }    

    Printer::Instance().AddMessage(message);

    auto it = Map::Instance().CurrentLevel->GameObjects.begin();
    Map::Instance().CurrentLevel->GameObjects.erase(it + res.first);
  }
  else
  {
    Printer::Instance().AddMessage("Nothing of interest here");
  }
}

void MainState::DrawHPMP()
{
  int curHp = _playerRef->Attrs.HP.CurrentValue;
  int maxHp = _playerRef->Attrs.HP.OriginalValue;
  int curMp = _playerRef->Attrs.MP.CurrentValue;
  int maxMp = _playerRef->Attrs.MP.OriginalValue;

  int th = Printer::Instance().TerminalHeight;

  auto bar = UpdateBar(0, th - 2, _playerRef->Attrs.HP);

  auto str = Util::StringFormat("%i/%i", curHp, maxHp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2, th - 2, str, Printer::kAlignCenter, "#FFFFFF", "#880000");

  bar = UpdateBar(0, th - 1, _playerRef->Attrs.MP);

  str = Util::StringFormat("%i/%i", curMp, maxMp);
  Printer::Instance().PrintFB(GlobalConstants::HPMPBarLength / 2, th - 1, str, Printer::kAlignCenter, "#FFFFFF", "#000088");
}

std::string MainState::UpdateBar(int x, int y, Attribute attr)
{
  float ratio = ((float)attr.CurrentValue / (float)attr.OriginalValue);
  int len = ratio * GlobalConstants::HPMPBarLength;

  std::string bar = "[";
  for (int i = 0; i < GlobalConstants::HPMPBarLength; i++)
  {
    bar += (i < len) ? "=" : " ";
  }

  bar += "]";

  Printer::Instance().PrintFB(x, y, bar, Printer::kAlignLeft, "#FFFFFF");

  return bar;
}

void MainState::DisplayHelp()
{
  Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Help", _helpText);
}

void MainState::CheckStairs(chtype stairsSymbol)
{
  if (stairsSymbol == '>')
  {
    auto tile = Map::Instance().CurrentLevel->MapArray[_playerRef->PosX][_playerRef->PosY].get();
    if (tile->Image != '>')
    {
      Printer::Instance().AddMessage("There are no stairs leading down here");
      return;
    }

    Component* c = tile->GetComponent<StairsComponent>();
    StairsComponent* stairs = static_cast<StairsComponent*>(c);
    Map::Instance().ChangeLevel(stairs->LeadsTo, true);
  }
  else if (stairsSymbol == '<')
  {
    auto tile = Map::Instance().CurrentLevel->MapArray[_playerRef->PosX][_playerRef->PosY].get();
    if (tile->Image != '<')
    {
      Printer::Instance().AddMessage("There are no stairs leading up here");
      return;
    }

    Component* c = tile->GetComponent<StairsComponent>();
    StairsComponent* stairs = static_cast<StairsComponent*>(c);
    Map::Instance().ChangeLevel(stairs->LeadsTo, false);
  }
}
