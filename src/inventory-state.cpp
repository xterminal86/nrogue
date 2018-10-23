#include "inventory-state.h"

#include "item-component.h"
#include "application.h"
#include "printer.h"
#include "util.h"

void InventoryState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void InventoryState::Prepare()
{
  _selectedIndex = 0;
}

void InventoryState::HandleInput()
{
  _keyPressed = getch();    

  bool isInventoryEmpty = (_playerRef->Inventory.Contents.size() == 0);

  switch(_keyPressed)
  {
    case KEY_UP:
      _selectedIndex--;
      break;

    case KEY_DOWN:
      _selectedIndex++;
      break;

    case 'i':
    {      
      if (isInventoryEmpty)
      {
        return;
      }

      auto c = _playerRef->Inventory.Contents[_selectedIndex]->GetComponent<ItemComponent>();
      ((ItemComponent*)c)->Inspect();            
    }
    break;

    case 'd':
    {
      if (isInventoryEmpty)
      {
        return;
      }

      DropItem();
      DestroyItem();
    }
    break;

    case 'u':
    {
      if (isInventoryEmpty)
      {
        return;
      }

      auto go = _playerRef->Inventory.Contents[_selectedIndex].get();
      auto c = go->GetComponent<ItemComponent>();

      if (((ItemComponent*)c)->Use())
      {
        if (((ItemComponent*)c)->IsStackable)
        {
          ((ItemComponent*)c)->Amount--;

          if (((ItemComponent*)c)->Amount == 0)
          {
            DestroyItem();
          }
        }
        else
        {
          DestroyItem();
        }

        _playerRef->SubtractActionMeter();
      }

      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
    }
    break;

    case 'e':
    case 't':
      Application::Instance().ShowMessageBox("Information", { "Not implemented yet!" });
      break;

    case 'q':
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      break;
  }

  int inventorySize = _playerRef->Inventory.Contents.size();
  _selectedIndex = Util::Clamp(_selectedIndex, 0, inventorySize - 1);
}

void InventoryState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    Printer::Instance().PrintFB(tw / 2, 0, "INVENTORY", Printer::kAlignCenter, "#FFFFFF");
    Printer::Instance().PrintFB(tw / 2, 1, "=========", Printer::kAlignCenter, "#FFFFFF");

    int itemsCount = 0;
    int yPos = 0;
    for (auto& item : _playerRef->Inventory.Contents)
    {
      std::string nameInInventory;

      auto c = item->GetComponent<ItemComponent>();
      if (((ItemComponent*)c)->IsStackable)
      {
        nameInInventory = Util::StringFormat("%s (%i)", item->ObjectName.data(), ((ItemComponent*)c)->Amount);
      }
      else
      {
        nameInInventory = item->ObjectName;
      }

      if (yPos == _selectedIndex)
      {
        Printer::Instance().PrintFB(0, 2 + yPos, nameInInventory, Printer::kAlignLeft, "#000000", "#FFFFFF");
      }
      else
      {
        Printer::Instance().PrintFB(0, 2 + yPos, nameInInventory, Printer::kAlignLeft, "#FFFFFF");
      }

      yPos++;

      itemsCount++;
    }

    for (int i = itemsCount; i < _playerRef->kInventorySize; i++)
    {
      Printer::Instance().PrintFB(0, 2 + yPos, "----------", Printer::kAlignLeft, "#FFFFFF");
      yPos++;
    }

    PrintFooter();

    Printer::Instance().Render();
  }
}

void InventoryState::PrintFooter()
{
  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  int part = tw / 5;

  Printer::Instance().PrintFB(tw / 2 - part * 2, th - 1, "'i' - inspect", Printer::kAlignCenter, "#FFFFFF");
  Printer::Instance().PrintFB(tw / 2 - part, th - 1, "'e' - equip", Printer::kAlignCenter, "#FFFFFF");
  Printer::Instance().PrintFB(tw / 2, th - 1, "'u' - use", Printer::kAlignCenter, "#FFFFFF");
  Printer::Instance().PrintFB(tw / 2 + part, th - 1, "'d' - drop", Printer::kAlignCenter, "#FFFFFF");
  Printer::Instance().PrintFB(tw / 2 + part * 2, th - 1, "'t' - throw", Printer::kAlignCenter, "#FFFFFF");
}

void InventoryState::DestroyItem()
{
  _playerRef->Inventory.Contents.erase(_playerRef->Inventory.Contents.begin() + _selectedIndex);
}

void InventoryState::DropItem()
{
  auto go = _playerRef->Inventory.Contents[_selectedIndex].release();
  auto c = go->GetComponent<ItemComponent>();
  ((ItemComponent*)c)->Transfer();

  std::string message;
  if (((ItemComponent*)c)->IsStackable)
  {
    message = Util::StringFormat("Dropped: %i %s", ((ItemComponent*)c)->Amount, go->ObjectName.data());
  }
  else
  {
    message = Util::StringFormat("Dropped: %s", go->ObjectName.data());
  }

  Printer::Instance().AddMessage(message);
}
