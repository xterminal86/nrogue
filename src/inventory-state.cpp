#include "inventory-state.h"

#include "item-component.h"
#include "application.h"
#include "printer.h"
#include "util.h"

void InventoryState::Prepare()
{
  _selectedIndex = 0;
}

void InventoryState::HandleInput()
{  
  _keyPressed = getch();

  auto& playerRef = Application::Instance().PlayerInstance;

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
      if (playerRef.Inventory.Contents.size() == 0)
      {
        break;
      }

      auto c = playerRef.Inventory.Contents[_selectedIndex]->GetComponent<ItemComponent>();
      ((ItemComponent*)c)->Inspect();
      break;
    }

    case 'd':
    {
      if (playerRef.Inventory.Contents.size() == 0)
      {
        break;
      }

      auto go = playerRef.Inventory.Contents[_selectedIndex].release();
      auto c = go->GetComponent<ItemComponent>();
      ((ItemComponent*)c)->Transfer();
      playerRef.Inventory.Contents.erase(playerRef.Inventory.Contents.begin() + _selectedIndex);
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

      break;
    }

    case 'e':
    case 'u':
    case 't':
      Application::Instance().ShowMessageBox("Information", { "Not implemented yet!" });
      break;

    case 'q':
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      break;
  }

  int inventorySize = Application::Instance().PlayerInstance.Inventory.Contents.size();
  _selectedIndex = Util::Clamp(_selectedIndex, 0, inventorySize - 1);
}

void InventoryState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    auto& playerRef = Application::Instance().PlayerInstance;

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    Printer::Instance().PrintFB(tw / 2, 0, "INVENTORY", Printer::kAlignCenter, "#FFFFFF");
    Printer::Instance().PrintFB(tw / 2, 1, "=========", Printer::kAlignCenter, "#FFFFFF");

    int yPos = 0;
    for (auto& item : playerRef.Inventory.Contents)
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
