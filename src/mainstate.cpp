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

  if (_playerRef->Attrs.ActionMeter >= 100)
  {
    switch (_keyPressed)
    {
      case NUMPAD_7:
        if (_playerRef->TryToAttack(-1, -1))
        {
          _playerRef->SubtractActionMeter();
        }
        else if (_playerRef->Move(-1, -1))
        {
          Map::Instance().MapOffsetY++;
          Map::Instance().MapOffsetX++;

          _playerRef->SubtractActionMeter();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_8:
        if (_playerRef->TryToAttack(0, -1))
        {
          _playerRef->SubtractActionMeter();
        }
        else if (_playerRef->Move(0, -1))
        {
          Map::Instance().MapOffsetY++;

          _playerRef->SubtractActionMeter();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_9:
        if (_playerRef->TryToAttack(1, -1))
        {
          _playerRef->SubtractActionMeter();
        }
        else if (_playerRef->Move(1, -1))
        {
          Map::Instance().MapOffsetY++;
          Map::Instance().MapOffsetX--;

          _playerRef->SubtractActionMeter();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_4:
        if (_playerRef->TryToAttack(-1, 0))
        {
          _playerRef->SubtractActionMeter();
        }
        else if (_playerRef->Move(-1, 0))
        {
          Map::Instance().MapOffsetX++;

          _playerRef->SubtractActionMeter();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_2:
        if (_playerRef->TryToAttack(0, 1))
        {
          _playerRef->SubtractActionMeter();
        }
        else if (_playerRef->Move(0, 1))
        {
          Map::Instance().MapOffsetY--;

          _playerRef->SubtractActionMeter();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_6:
        if (_playerRef->TryToAttack(1, 0))
        {
          _playerRef->SubtractActionMeter();
        }
        else if (_playerRef->Move(1, 0))
        {
          Map::Instance().MapOffsetX--;

          _playerRef->SubtractActionMeter();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_1:
        if (_playerRef->TryToAttack(-1, 1))
        {
          _playerRef->SubtractActionMeter();
        }
        else if (_playerRef->Move(-1, 1))
        {
          Map::Instance().MapOffsetY--;
          Map::Instance().MapOffsetX++;

          _playerRef->SubtractActionMeter();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      case NUMPAD_3:
        if (_playerRef->TryToAttack(1, 1))
        {
          _playerRef->SubtractActionMeter();
        }
        else if (_playerRef->Move(1, 1))
        {
          Map::Instance().MapOffsetY--;
          Map::Instance().MapOffsetX--;

          _playerRef->SubtractActionMeter();

          Printer::Instance().ShowLastMessage = false;
        }
        break;

      // wait
      case NUMPAD_5:
        Printer::Instance().AddMessage("You waited...");

        // TODO: think
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
  if (_playerRef->Attrs.ActionMeter < 100)
  {    
    Map::Instance().UpdateGameObjects();
    Update(true);
    _playerRef->Attrs.ActionMeter += _playerRef->Attrs.Spd.CurrentValue;
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

    // NOTE: Some debug info

    _debugInfo = Util::StringFormat("Act: %i Ofst: %i %i: Plr: [%i;%i] Key: %i",
                                    _playerRef->Attrs.ActionMeter,
                                    Map::Instance().MapOffsetX,
                                    Map::Instance().MapOffsetY,
                                    _playerRef->PosX,
                                    _playerRef->PosY,
                                    _keyPressed);

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth - 1, 0, _debugInfo, Printer::kAlignRight, "#FFFFFF");

    _debugInfo = Util::StringFormat("World seed: %lu", RNG::Instance().Seed);
    Printer::Instance().PrintFB(0, 0, _debugInfo, Printer::kAlignLeft, "#FFFFFF");

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
      Application::Instance().ShowMessageBox("Epic Fail", { "Inventory is full!" });
      return;
    }

    ic = res.second->GetComponent<ItemComponent>();

    auto go = Map::Instance().GameObjects[res.first].release();

    _playerRef->Inventory.AddToInventory(go);

    std::string message;
    if (((ItemComponent*)ic)->IsStackable)
    {
      message = Util::StringFormat("Picked up: %i %s", ((ItemComponent*)ic)->Amount, go->ObjectName.data());
    }
    else
    {
      message = Util::StringFormat("Picked up: %s", go->ObjectName.data());
    }    

    Printer::Instance().AddMessage(message);

    auto it = Map::Instance().GameObjects.begin();
    Map::Instance().GameObjects.erase(it + res.first);
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
