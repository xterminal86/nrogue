#include "mainstate.h"

#include "application.h"
#include "map.h"
#include "printer.h"
#include "rng.h"
#include "item-component.h"

void MainState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;  
}

void MainState::HandleInput()
{
  _keyPressed = getch();

  if (_playerRef->ActionMeter >= 100)
  {
    switch (_keyPressed)
    {
      case NUMPAD_7:
        if (_playerRef->Move(-1, -1))
        {
          Map::Instance().MapOffsetY++;
          Map::Instance().MapOffsetX++;

          _playerRef->SubtractActionMeter();
        }
        break;

      case NUMPAD_8:
        if (_playerRef->Move(0, -1))
        {
          Map::Instance().MapOffsetY++;

          _playerRef->SubtractActionMeter();
        }
        break;

      case NUMPAD_9:
        if (_playerRef->Move(1, -1))
        {
          Map::Instance().MapOffsetY++;
          Map::Instance().MapOffsetX--;

          _playerRef->SubtractActionMeter();
        }
        break;

      case NUMPAD_4:
        if (_playerRef->Move(-1, 0))
        {
          Map::Instance().MapOffsetX++;

          _playerRef->SubtractActionMeter();
        }
        break;

      case NUMPAD_2:
        if (_playerRef->Move(0, 1))
        {
          Map::Instance().MapOffsetY--;

          _playerRef->SubtractActionMeter();
        }
        break;

      case NUMPAD_6:
        if (_playerRef->Move(1, 0))
        {
          Map::Instance().MapOffsetX--;

          _playerRef->SubtractActionMeter();
        }
        break;

      case NUMPAD_1:
        if (_playerRef->Move(-1, 1))
        {
          Map::Instance().MapOffsetY--;
          Map::Instance().MapOffsetX++;

          _playerRef->SubtractActionMeter();
        }
        break;

      case NUMPAD_3:
        if (_playerRef->Move(1, 1))
        {
          Map::Instance().MapOffsetY--;
          Map::Instance().MapOffsetX--;

          _playerRef->SubtractActionMeter();
        }
        break;

      // wait
      case NUMPAD_5:
        Printer::Instance().AddMessage("You waited...");

        // TODO: think
        //_playerRef->SubtractActionMeter();
        Map::Instance().UpdateGameObjects();
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
        Application::Instance().ChangeState(Application::GameStates::SHOW_HELP_STATE);
        break;

      case 'q':
        Application::Instance().ChangeState(Application::GameStates::EXITING_STATE);
        break;

      default:
        break;
    }
  }

  // If player's turn finished, update all game objects' components
  if (_playerRef->ActionMeter < 100)
  {    
    Map::Instance().UpdateGameObjects();
    Update(true);
    _playerRef->ActionMeter += _playerRef->Actor.Attrs.Spd.CurrentValue;    
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

    // Some debug info    
    _debugInfo = Util::StringFormat("Act: %i Ofst: %i %i: Plr: [%i;%i] Key: %i",
                                    _playerRef->ActionMeter,
                                    Map::Instance().MapOffsetX,
                                    Map::Instance().MapOffsetY,
                                    _playerRef->PosX,
                                    _playerRef->PosY,
                                    _keyPressed);

    Printer::Instance().PrintFB(0, Printer::Instance().TerminalHeight - 1, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

    _debugInfo = Util::StringFormat("World seed: %lu", RNG::Instance().Seed);
    Printer::Instance().PrintFB(0, 0, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

    if (Printer::Instance().ShowLastMessage)
    {
      DisplayGameLog();
      Printer::Instance().ShowLastMessage = false;
    }

    Printer::Instance().Render();
  }
}

void MainState::DisplayGameLog()
{
  int x = Printer::Instance().TerminalWidth - 1;
  int y = Printer::Instance().TerminalHeight - 1;

  Printer::Instance().PrintFB(x, y, Printer::Instance().GetLastMessage(), Printer::kAlignRight, "#FFFFFF");
}

void MainState::TryToPickupItem()
{
  auto res = Map::Instance().GetTopGameObjectAtPosition(_playerRef->PosX, _playerRef->PosY);
  if (res.second != nullptr)
  {
    auto go = Map::Instance().GameObjects[res.first].release();
    auto c = go->GetComponent<ItemComponent>();
    if (((ItemComponent*)c)->IsStackable)
    {
      bool isInInventory = false;

      for (auto& i : _playerRef->Inventory.Contents)
      {
        auto ic = i->GetComponent<ItemComponent>();
        if (((ItemComponent*)ic)->TypeOfObject == ((ItemComponent*)c)->TypeOfObject)
        {
          ((ItemComponent*)ic)->Amount += ((ItemComponent*)c)->Amount;
          isInInventory = true;
          break;
        }
      }

      if (!isInInventory)
      {
        _playerRef->Inventory.AddToInventory(go);
      }
    }
    else
    {
      _playerRef->Inventory.AddToInventory(go);
    }

    Map::Instance().GameObjects.erase(Map::Instance().GameObjects.begin() + res.first);

    std::string message;
    if (((ItemComponent*)c)->IsStackable)
    {
      message = Util::StringFormat("Picked up: %i %s", ((ItemComponent*)c)->Amount, go->ObjectName.data());
    }
    else
    {
      message = Util::StringFormat("Picked up: %s", go->ObjectName.data());
    }

    Printer::Instance().AddMessage(message);
  }
  else
  {
    Printer::Instance().AddMessage("There's nothing here");
  }
}
