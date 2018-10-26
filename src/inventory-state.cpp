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

      auto go = _playerRef->Inventory.Contents[_selectedIndex].get();
      auto c = go->GetComponent<ItemComponent>();
      ItemComponent* ic = static_cast<ItemComponent*>(c);

      if (ic->IsEquipped)
      {
        Application::Instance().ShowMessageBox("Information", { "Unequip first!" });
        return;
      }

      DropItem();
      DestroyInventoryItem();
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
      ItemComponent* ic = static_cast<ItemComponent*>(c);

      if (ic->Use())
      {
        if (ic->IsStackable)
        {
          ic->Amount--;

          if (ic->Amount == 0)
          {
            DestroyInventoryItem();
          }
        }
        else
        {
          DestroyInventoryItem();
        }

        _playerRef->FinishTurn();

        Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      }      
    }
    break;

    case 'e':
    {
      auto go = _playerRef->Inventory.Contents[_selectedIndex].get();
      auto c = go->GetComponent<ItemComponent>();
      ItemComponent* ic = static_cast<ItemComponent*>(c);
      if (ic->Equip())
      {
        _playerRef->FinishTurn();
        Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      }
    }
    break;

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

    Printer::Instance().PrintFB(tw / 2, 0, "========== INVENTORY ==========", Printer::kAlignCenter, "#FFFFFF");

    int itemsCount = 0;
    int yPos = 0;
    for (auto& item : _playerRef->Inventory.Contents)
    {      
      std::string nameInInventory = item->ObjectName;      
      nameInInventory.resize(kInventoryMaxNameLength, ' ');

      auto c = item->GetComponent<ItemComponent>();
      if (((ItemComponent*)c)->IsStackable)
      {        
        auto stackAmount = Util::StringFormat("(%i)", ((ItemComponent*)c)->Amount);
        Printer::Instance().PrintFB(kInventoryMaxNameLength + 1, 2 + yPos, stackAmount, Printer::kAlignLeft, "#FFFFFF");
      }
      else if (((ItemComponent*)c)->IsEquipped)
      {
        auto equipStatus = Util::StringFormat("E", ((ItemComponent*)c)->Amount);
        Printer::Instance().PrintFB(kInventoryMaxNameLength + 1, 2 + yPos, equipStatus, Printer::kAlignLeft, "#FFFFFF");
      }

      DrawSelectionBar(yPos, nameInInventory);

      yPos++;

      itemsCount++;
    }

    for (int i = itemsCount; i < _playerRef->kInventorySize; i++)
    {
      std::string stub(kInventoryMaxNameLength, '-');
      Printer::Instance().PrintFB(0, 2 + yPos, stub, Printer::kAlignLeft, "#FFFFFF");
      yPos++;
    }

    DisplayEquipment();

    PrintFooter();

    Printer::Instance().Render();
  }
}

void InventoryState::DisplayEquipment()
{
  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  tw /= 2;

  int yPos = 4;

  ItemComponent* eq = nullptr;

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::HEAD][0];
  DrawEquipmentField(tw + 10, yPos, "Head", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::NECK][0];
  DrawEquipmentField(tw + 24, yPos, "Neck", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  DrawEquipmentField(tw - 4, yPos + 3, "Weapon", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::TORSO][0];
  DrawEquipmentField(tw + 10, yPos + 3, "Torso", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::LEGS][0];
  DrawEquipmentField(tw + 10, yPos + 6, "Legs", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::BOOTS][0];
  DrawEquipmentField(tw + 10, yPos + 9, "Boots", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::RING][0];
  DrawEquipmentField(tw + 24, yPos + 6, "Ring", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::RING][1];
  DrawEquipmentField(tw + 24, yPos + 9, "Ring", eq);
}

void InventoryState::DrawEquipmentField(int x, int y, std::string fieldName, ItemComponent* eq)
{
  std::string stub(kEquipmentMaxNameLength, '-');

  Printer::Instance().PrintFB(x, y, fieldName, Printer::kAlignCenter, "#FFFFFF");

  if (eq != nullptr)
  {
    stub = ((GameObject*)eq->OwnerGameObject)->ObjectName;
    stub.resize(kEquipmentMaxNameLength, ' ');
  }

  Printer::Instance().PrintFB(x, y + 1, stub, Printer::kAlignCenter, "#FFFFFF");
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

void InventoryState::DestroyInventoryItem()
{
  auto it = _playerRef->Inventory.Contents.begin();
  _playerRef->Inventory.Contents.erase(it + _selectedIndex);
}

void InventoryState::DropItem()
{
  auto go = _playerRef->Inventory.Contents[_selectedIndex].release();
  auto c = go->GetComponent<ItemComponent>();
  ItemComponent* ic = static_cast<ItemComponent*>(c);
  if (ic->IsEquipped)
  {
    ic->Equip();
  }

  ic->Transfer();

  std::string message;
  if (ic->IsStackable)
  {
    message = Util::StringFormat("Dropped: %i %s", ic->Amount, go->ObjectName.data());
  }
  else
  {
    message = Util::StringFormat("Dropped: %s", go->ObjectName.data());
  }

  // !!! Destruction is done in other method !!!

  Printer::Instance().AddMessage(message);
}

void InventoryState::DrawSelectionBar(int yOffset, std::string& text)
{
  if (yOffset == _selectedIndex)
  {
    Printer::Instance().PrintFB(0, 2 + yOffset, text, Printer::kAlignLeft, "#000000", "#FFFFFF");
  }
  else
  {
    Printer::Instance().PrintFB(0, 2 + yOffset, text, Printer::kAlignLeft, "#FFFFFF");
  }
}
