#include "inventory-state.h"

#include "item-component.h"
#include "application.h"
#include "printer.h"
#include "util.h"
#include "map.h"

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
    case NUMPAD_8:
      _selectedIndex--;
      break;

    case NUMPAD_2:
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
      ItemComponent* ic = go->GetComponent<ItemComponent>();

      if (ic->Data.IsEquipped)
      {
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "Unequip first!" }, GlobalConstants::MessageBoxRedBorderColor);
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
      ItemComponent* ic = go->GetComponent<ItemComponent>();

      if (ic->Use())
      {
        if (ic->Data.IsStackable)
        {
          ic->Data.Amount--;

          if (ic->Data.Amount == 0)
          {
            DestroyInventoryItem();
          }
        }
        else
        {
          DestroyInventoryItem();
        }

        _playerRef->FinishTurn();

        // Check if player was killed
        // after using something (e.g. cursed potion)
        if (!_playerRef->IsAlive(go))
        {
          Application::Instance().ChangeState(GameStates::ENDGAME_STATE);
        }        
      }      
    }
    break;

    case 'e':
    {
      auto go = _playerRef->Inventory.Contents[_selectedIndex].get();
      ItemComponent* ic = go->GetComponent<ItemComponent>();
      if (ic->Equip())
      {
        _playerRef->FinishTurn();
        Application::Instance().ChangeState(GameStates::MAIN_STATE);
      }
    }
    break;

    case 't':
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "Not implemented yet!" });
      break;

    case 'q':
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
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

    Printer::Instance().PrintFB(tw / 2, 0, "========== INVENTORY ==========", Printer::kAlignCenter, "#FFFFFF");

    int itemsCount = 0;
    int yPos = 0;
    for (auto& item : _playerRef->Inventory.Contents)
    {
      auto c = item->GetComponent<ItemComponent>();
      ItemComponent* ic = static_cast<ItemComponent*>(c);

      std::string nameInInventory = ic->Data.IsIdentified ? item->ObjectName : ic->Data.UnidentifiedName;
      nameInInventory.resize(GlobalConstants::InventoryMaxNameLength, ' ');

      if (ic->Data.IsStackable)
      {        
        auto stackAmount = Util::StringFormat("(%i)", ic->Data.Amount);
        Printer::Instance().PrintFB(GlobalConstants::InventoryMaxNameLength + 1, 2 + yPos, stackAmount, Printer::kAlignLeft, "#FFFFFF");
      }
      else if (ic->Data.IsEquipped)
      {
        auto equipStatus = Util::StringFormat("E", ic->Data.Amount);
        Printer::Instance().PrintFB(GlobalConstants::InventoryMaxNameLength + 1, 2 + yPos, equipStatus, Printer::kAlignLeft, "#FFFFFF");
      }

      std::string textColor = "#FFFFFF";

      if (ic->Data.Prefix == ItemPrefix::BLESSED)
      {
        textColor = GlobalConstants::ItemMagicColor;
      }
      else if (ic->Data.Prefix == ItemPrefix::CURSED)
      {
        textColor = "#880000";
      }

      std::string idColor = (ic->Data.IsIdentified || ic->Data.IsPrefixDiscovered) ? textColor : "#FFFFFF";
      DrawSelectionBar(yPos, nameInInventory, idColor);

      yPos++;

      itemsCount++;
    }

    for (int i = itemsCount; i < GlobalConstants::InventoryMaxSize; i++)
    {
      std::string stub(GlobalConstants::InventoryMaxNameLength, '-');
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
    stub = eq->Data.IsIdentified ? eq->OwnerGameObject->ObjectName : eq->Data.UnidentifiedName;
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
  ItemComponent* ic = go->GetComponent<ItemComponent>();

  // Player can drop items on different dungeon level,
  // so reference to level where object was originally
  // created will become invalid and it will not be drawn.
  // (well, actually it will be drawn, but using original
  // _levelOwner->MapOffsetX and Y which might be different)
  ic->OwnerGameObject->SetLevelOwner(Map::Instance().CurrentLevel);

  ic->Transfer();

  std::string objName = ic->Data.IsIdentified ? go->ObjectName : ic->Data.UnidentifiedName;

  std::string message;
  if (ic->Data.IsStackable)
  {
    message = Util::StringFormat("Dropped: %i %s", ic->Data.Amount, objName.data());
  }
  else
  {
    message = Util::StringFormat("Dropped: %s", objName.data());
  }

  // !!! OwnerGameObject should not be destroyed here !!!

  Printer::Instance().AddMessage(message);
}

void InventoryState::DrawSelectionBar(int yOffset, std::string& text, std::string& textColor)
{
  if (yOffset == _selectedIndex)
  {
    Printer::Instance().PrintFB(0, 2 + yOffset, text, Printer::kAlignLeft, "#000000", "#FFFFFF");
  }
  else
  {
    Printer::Instance().PrintFB(0, 2 + yOffset, text, Printer::kAlignLeft, textColor);
  }
}