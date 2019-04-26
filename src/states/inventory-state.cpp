#include "inventory-state.h"

#include "item-component.h"
#include "application.h"
#include "printer.h"
#include "returner-state.h"
#include "repair-state.h"
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
  _keyPressed = GetKeyDown();

  bool isInventoryEmpty = (_playerRef->Inventory.IsEmpty());

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
        else if (ic->Data.IsChargeable)
        {
          if (ic->Data.ItemType_ == ItemType::RETURNER)
          {
            auto s = Application::Instance().GetGameStateRefByName(GameStates::RETURNER_STATE);
            ReturnerState* rs = static_cast<ReturnerState*>(s);
            rs->SetItemComponentRef(ic);
            Application::Instance().ChangeState(GameStates::RETURNER_STATE);
          }
          else if (ic->Data.ItemType_ == ItemType::REPAIR_KIT)
          {
            auto s = Application::Instance().GetGameStateRefByName(GameStates::REPAIR_STATE);
            RepairState* rs = static_cast<RepairState*>(s);
            rs->SetRepairKitRef(ic, _selectedIndex);
            Application::Instance().ChangeState(GameStates::REPAIR_STATE);
          }
        }
        else
        {
          DestroyInventoryItem();
        }

        _playerRef->FinishTurn();

        // Check if player was killed
        // after using something (e.g. cursed potion, teleported into wall)
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

    case 's':
      SortInventory();
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

    DrawHeader(" INVENTORY ");

    int itemsCount = 0;
    int yPos = 0;
    for (auto& item : _playerRef->Inventory.Contents)
    {
      auto c = item->GetComponent<ItemComponent>();
      ItemComponent* ic = static_cast<ItemComponent*>(c);

      std::string nameInInventory = ic->Data.IsIdentified ?
                                    item->ObjectName :
                                    ic->Data.UnidentifiedName;

      nameInInventory.resize(GlobalConstants::InventoryMaxNameLength, ' ');

      if (ic->Data.IsStackable || (ic->Data.IsChargeable && ic->Data.IsIdentified))
      {        
        auto stackAmount = Util::StringFormat("(%i)", ic->Data.Amount);
        if ((ic->Data.ItemType_ == ItemType::ARROWS
          || ic->Data.ItemType_ == ItemType::WAND)
            && ic->Data.IsEquipped)
        {
          stackAmount += " (E)";
        }

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

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::TORSO][0];
  DrawEquipmentField(tw + 10, yPos + 3, "Armor", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::BOOTS][0];
  DrawEquipmentField(tw + 10, yPos + 6, "Boots", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  DrawEquipmentField(tw - 4, yPos + 3, "Hand", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::SHIELD][0];
  DrawEquipmentField(tw - 4, yPos + 6, "Hand", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::RING][0];
  DrawEquipmentField(tw + 24, yPos + 3, "Accessory", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::RING][1];
  DrawEquipmentField(tw + 24, yPos + 6, "Accessory", eq);

  // TODO: too much slots, remove?
  /*
  eq = _playerRef->EquipmentByCategory[EquipmentCategory::HEAD][0];
  DrawEquipmentField(tw + 10, yPos, "Head", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::NECK][0];
  DrawEquipmentField(tw + 24, yPos, "Neck", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  DrawEquipmentField(tw - 4, yPos + 3, "Weapon", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::TORSO][0];
  DrawEquipmentField(tw + 10, yPos + 3, "Armor", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::SHIELD][0];
  DrawEquipmentField(tw + 24, yPos + 3, "Shield", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::LEGS][0];
  DrawEquipmentField(tw + 10, yPos + 6, "Legs", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::BOOTS][0];
  DrawEquipmentField(tw + 10, yPos + 9, "Boots", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::RING][0];
  DrawEquipmentField(tw + 24, yPos + 6, "Ring", eq);

  eq = _playerRef->EquipmentByCategory[EquipmentCategory::RING][1];
  DrawEquipmentField(tw + 24, yPos + 9, "Ring", eq);
  */
}

void InventoryState::DrawEquipmentField(int x, int y, const std::string& fieldName, ItemComponent* eq)
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

  int part = tw / 6;

  std::vector<std::string> footer =
  {
    "'i' - inspect",
    "'e' - equip",
    "'s' - sort",
    "'u' - use",
    "'d' - drop",
    "'t' - throw"
  };

  int additive = 0;

  int counter = 0;
  for (int i = 0; i < 3; i++)
  {
    std::string tmp = footer[i];
    tmp.resize(part, ' ');

    additive = (counter > 0) ? 3 : 0;

    Printer::Instance().PrintFB(counter * part + additive, th - 1, tmp, Printer::kAlignLeft, "#FFFFFF");
    counter++;
  }

  counter = 0;
  additive = 0;

  for (int i = 3; i < footer.size(); i++)
  {
    int d = part - footer[i].length();

    std::string tmp = footer[i];
    for (int j = 0; j < d; j++)
    {
      tmp.insert(0, " ");
    }

    additive = (counter < 2) ? 1 : 0;

    Printer::Instance().PrintFB(tw / 2 + 1 + counter * part - additive, th - 1, tmp, Printer::kAlignLeft, "#FFFFFF");
    counter++;
  }
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

  // Player may drop item on a different dungeon level,
  // so reference to the level where object was originally
  // created will become invalid and it will not be drawn.
  // Well, actually it will be drawn, but using original
  // _levelOwner->MapOffsetX and Y, which might be different
  // or out of bounds on current level.
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

void InventoryState::DrawSelectionBar(int yOffset, const std::string& text, const std::string& textColor)
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

void InventoryState::SortInventory()
{
  std::vector<std::unique_ptr<GameObject>>& inventory = _playerRef->Inventory.Contents;

  for (int i = 0; i < inventory.size(); i++)
  {
    auto go1 = inventory.at(i).get();
    ItemComponent* currentItem = go1->GetComponent<ItemComponent>();
    if (currentItem->Data.IsIdentified && currentItem->Data.IsStackable)
    {
      int from = i + 1;

      for (int j = from; j < inventory.size(); j++)
      {
        auto go2 = inventory.at(j).get();
        ItemComponent* nextItem = go2->GetComponent<ItemComponent>();
        if (nextItem->Data.IsIdentified
         && nextItem->Data.IsStackable
         && currentItem->Data.ItemTypeHash == nextItem->Data.ItemTypeHash)
        {
          currentItem->Data.Amount++;
          inventory.erase(inventory.begin() + j);
        }
      }
    }
  }
}
