#include "inventory-state.h"

#include "application.h"
#include "printer.h"
#include "returner-state.h"
#include "repair-state.h"
#include "target-state.h"
#include "util.h"
#include "map.h"

void InventoryState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

// =============================================================================

void InventoryState::Prepare()
{
  _selectedIndex = 0;
}

// =============================================================================

void InventoryState::HandleInput()
{
  _keyPressed = GetKeyDown();

  bool isInventoryEmpty = (_playerRef->Inventory->IsEmpty());

  switch(_keyPressed)
  {
    case ALT_K8:
    case NUMPAD_8:
      _selectedIndex--;
      break;

    case ALT_K2:
    case NUMPAD_2:
      _selectedIndex++;
      break;

    case 'i':
    {
      if (isInventoryEmpty)
      {
        return;
      }

      ItemComponent* c = _playerRef->Inventory->Contents[_selectedIndex]->GetComponent<ItemComponent>();
      c->Inspect();
    }
    break;

    case 'd':
    {
      if (isInventoryEmpty)
      {
        return;
      }

      auto go = _playerRef->Inventory->Contents[_selectedIndex].get();
      ItemComponent* ic = go->GetComponent<ItemComponent>();

      if (ic->Data.IsEquipped)
      {
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                               Strings::MessageBoxEpicFailHeaderText,
                                               { Strings::MsgUnequipFirst },
                                               Colors::MessageBoxRedBorderColor);
        return;
      }

      if (ic->Data.IsImportant)
      {
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                               Strings::MessageBoxInformationHeaderText,
                                               { Strings::MsgLooksImportant },
                                               Colors::MessageBoxBlueBorderColor);
        return;
      }

      DropItem(ic);
      DestroyInventoryItem();
    }
    break;

    case 'u':
    {
      if (isInventoryEmpty)
      {
        return;
      }

      auto go = _playerRef->Inventory->Contents[_selectedIndex].get();
      ItemComponent* ic = go->GetComponent<ItemComponent>();

      UseResult r = ic->Use(_playerRef);

      if (r == UseResult::SUCCESS)
      {
        if (ic->Data.IsStackable)
        {
          ic->Data.Amount--;

          if (ic->Data.Amount == 0)
          {
            DestroyInventoryItem();
          }

          Application::Instance().ChangeState(GameStates::MAIN_STATE);
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
          Application::Instance().ChangeState(GameStates::MAIN_STATE);
        }

        //
        // Turn is finished in corresponding states.
        //
        if (ic->Data.ItemType_ != ItemType::REPAIR_KIT
         && ic->Data.ItemType_ != ItemType::RETURNER)
        {
          _playerRef->FinishTurn();
        }

        //
        // Check if player was killed
        // after using something (e.g. cursed potion, teleported into wall)
        //
        if (!_playerRef->HasNonZeroHP())
        {
          Application::Instance().ChangeState(GameStates::ENDGAME_STATE);
        }
      }
      else if (r == UseResult::UNUSABLE)
      {
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                               Strings::MessageBoxEpicFailHeaderText,
                                               { Strings::MsgCantBeUsed },
                                               Colors::MessageBoxRedBorderColor);
      }
    }
    break;

    case 'e':
    {
      auto go = _playerRef->Inventory->Contents[_selectedIndex].get();
      ItemComponent* ic = go->GetComponent<ItemComponent>();
      if (_playerRef->Equipment->Equip(ic))
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
    {
      if (isInventoryEmpty)
      {
        return;
      }

      auto go = _playerRef->Inventory->Contents[_selectedIndex].get();
      ItemComponent* ic = go->GetComponent<ItemComponent>();
      if (ic->Data.IsEquipped)
      {
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                               Strings::MessageBoxEpicFailHeaderText,
                                               { Strings::MsgUnequipFirst },
                                               Colors::MessageBoxRedBorderColor);
        return;
      }

      if (ic->Data.IsImportant)
      {
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                               Strings::MessageBoxInformationHeaderText,
                                               { Strings::MsgLooksImportant },
                                               Colors::MessageBoxBlueBorderColor);
        return;
      }

      auto s = Application::Instance().GetGameStateRefByName(GameStates::TARGET_STATE);
      TargetState* ts = static_cast<TargetState*>(s);
      ts->SetupForThrowing(ic, _selectedIndex);
      Application::Instance().ChangeState(GameStates::TARGET_STATE);
    }
    break;

    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }

  int inventorySize = _playerRef->Inventory->Contents.size();
  _selectedIndex = Util::Clamp(_selectedIndex, 0, inventorySize - 1);
}

// =============================================================================

void InventoryState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawHeader(" INVENTORY ");

    int itemsCount = 0;
    int yPos = 0;
    for (auto& item : _playerRef->Inventory->Contents)
    {
      auto c = item->GetComponent<ItemComponent>();
      ItemComponent* ic = static_cast<ItemComponent*>(c);

      std::string nameInInventory = ic->Data.IsIdentified ?
                                    item->ObjectName :
                                    ic->Data.UnidentifiedName;

      if (ic->Data.IsIdentified && ic->Data.ItemType_ == ItemType::GEM)
      {
        ItemQuality q = ic->Data.ItemQuality_;
        nameInInventory.append(GlobalConstants::GemRatingByQuality.at(q));
      }

      nameInInventory.resize(GlobalConstants::InventoryMaxNameLength, ' ');

      if (ic->Data.IsStackable || (ic->Data.IsChargeable && ic->Data.IsIdentified))
      {
        std::string stackAmount = Util::StringFormat("(%i)", ic->Data.Amount);
        if ((ic->Data.ItemType_ == ItemType::ARROWS
          || ic->Data.ItemType_ == ItemType::WAND)
            && ic->Data.IsEquipped)
        {
          stackAmount.append(" (E)");
        }

        Printer::Instance().PrintFB(GlobalConstants::InventoryMaxNameLength + 2,
                                    2 + yPos,
                                    stackAmount,
                                    Printer::kAlignLeft,
                                    Colors::WhiteColor);
      }
      else if (ic->Data.IsEquipped)
      {
        auto equipStatus = Util::StringFormat("E", ic->Data.Amount);
        Printer::Instance().PrintFB(GlobalConstants::InventoryMaxNameLength + 2,
                                    2 + yPos,
                                    equipStatus,
                                    Printer::kAlignLeft,
                                    Colors::WhiteColor);
      }

      uint32_t textColor = Util::GetItemInventoryColor(ic->Data);

      DrawSelectionBar(yPos, nameInInventory, textColor);

      yPos++;

      itemsCount++;
    }

    for (int i = itemsCount; i < GlobalConstants::InventoryMaxSize; i++)
    {
      std::string stub(GlobalConstants::InventoryMaxNameLength,
                       Strings::InventoryEmptySlotChar);

      Printer::Instance().PrintFB(1,
                                  2 + yPos,
                                  stub,
                                  Printer::kAlignLeft,
                                  Colors::ShadesOfGrey::Six);
      yPos++;
    }

    DisplayEquipment();

    PrintFooter();

    Printer::Instance().Render();
  }
}

// =============================================================================

void InventoryState::DisplayEquipment()
{
  int tw = Printer::TerminalWidth;

  tw /= 2;

  int yPos = 4;

  ItemComponent* eq = nullptr;

  eq = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::HEAD][0];
  DrawEquipmentField(tw + 10, yPos, "[Head]", eq);

  eq = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::NECK][0];
  DrawEquipmentField(tw + 24, yPos, "[Neck]", eq);

  eq = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::TORSO][0];
  DrawEquipmentField(tw + 10, yPos + 3, "[Armor]", eq);

  eq = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::BOOTS][0];
  DrawEquipmentField(tw + 10, yPos + 6, "[Boots]", eq);

  eq = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::WEAPON][0];
  DrawEquipmentField(tw - 4, yPos + 3, "[Hand]", eq);

  eq = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::SHIELD][0];
  DrawEquipmentField(tw - 4, yPos + 6, "[Hand]", eq);

  eq = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::RING][0];
  DrawEquipmentField(tw + 24, yPos + 3, "[Accessory]", eq);

  eq = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::RING][1];
  DrawEquipmentField(tw + 24, yPos + 6, "[Accessory]", eq);
}

// =============================================================================

void InventoryState::DrawEquipmentField(int x, int y, const std::string& fieldName, ItemComponent* eq)
{
  std::string stub(kEquipmentMaxNameLength, Strings::InventoryEmptySlotChar);

  Printer::Instance().PrintFB(x,
                              y,
                              fieldName,
                              Printer::kAlignCenter,
                              Colors::ShadesOfGrey::Ten);

  uint32_t itemColor = Colors::WhiteColor;

  if (eq != nullptr)
  {
    stub = eq->Data.IsIdentified
         ? eq->OwnerGameObject->ObjectName
         : eq->Data.UnidentifiedName;
    stub.resize(kEquipmentMaxNameLength, ' ');
    itemColor = Util::GetItemInventoryColor(eq->Data);
  }

  Printer::Instance().PrintFB(x,
                              y + 1,
                              stub,
                              Printer::kAlignCenter,
                              (eq != nullptr)
                            ? itemColor
                            : Colors::ShadesOfGrey::Six);
}

// =============================================================================

void InventoryState::PrintFooter()
{
  int tw = Printer::TerminalWidth;
  int th = Printer::TerminalHeight;

  int part = tw / 6;

  int additive = 0;
  int counter = 0;

  //
  // Left part.
  //
  for (int i = 0; i < 3; i++)
  {
    std::string tmp = _footer[i];
    tmp.resize(part, ' ');

    additive = (counter > 0) ? 3 : 1;

    Printer::Instance().PrintFB(counter * part + additive,
                                th - 1,
                                tmp,
                                Printer::kAlignLeft,
                                Colors::WhiteColor);
    counter++;
  }

  counter = 0;
  additive = 0;

  //
  // Right part.
  //
  for (size_t i = 3; i < _footer.size(); i++)
  {
    int d = part - _footer[i].length();

    std::string tmp = _footer[i];
    for (int j = 0; j < d; j++)
    {
      tmp.insert(0, " ");
    }

    additive = 1;

    Printer::Instance().PrintFB(tw / 2 + 1 + counter * part - additive,
                                th - 1,
                                tmp,
                                Printer::kAlignLeft,
                                Colors::WhiteColor);
    counter++;
  }
}

// =============================================================================

void InventoryState::DestroyInventoryItem()
{
  auto it = _playerRef->Inventory->Contents.begin();
  _playerRef->Inventory->Contents.erase(it + _selectedIndex);
}

// =============================================================================

void InventoryState::DropItem(ItemComponent* ic)
{
  auto go = _playerRef->Inventory->Contents[_selectedIndex].release();

  //
  // Player may drop item on a different dungeon level,
  // so reference to the level where object was originally
  // created will become invalid and it will not be drawn.
  // Well, actually it will be drawn, but using original
  // _levelOwner->MapOffsetX and Y, which might be different
  // or out of bounds on current level.
  //
  ic->OwnerGameObject->SetLevelOwner(Map::Instance().CurrentLevel);

  ic->Transfer();

  std::string objName = ic->Data.IsIdentified ? go->ObjectName : ic->Data.UnidentifiedName;

  std::string message;
  if (ic->Data.IsStackable)
  {
    message = Util::StringFormat(Strings::FmtDroppedIS, ic->Data.Amount, objName.data());
  }
  else
  {
    message = Util::StringFormat(Strings::FmtDroppedS, objName.data());
  }

  // !!! OwnerGameObject should not be destroyed here !!!

  Printer::Instance().AddMessage(message);
}

// =============================================================================

void InventoryState::DrawSelectionBar(int yOffset, const std::string& text, const uint32_t& textColor)
{
  if (yOffset == _selectedIndex)
  {
    Printer::Instance().PrintFB(1,
                                2 + yOffset,
                                text,
                                Printer::kAlignLeft,
                                textColor,
                                Colors::ShadesOfGrey::Four);
  }
  else
  {
    Printer::Instance().PrintFB(1,
                                2 + yOffset,
                                text,
                                Printer::kAlignLeft,
                                textColor);
  }
}

// =============================================================================

void InventoryState::SortInventory()
{
  auto& inventory = _playerRef->Inventory->Contents;

  bool shouldCleanup = false;

  for (size_t i = 0; i < inventory.size(); i++)
  {
    auto go1 = inventory.at(i).get();
    ItemComponent* currentItem = go1->GetComponent<ItemComponent>();
    if (currentItem->Data.IsIdentified && currentItem->Data.IsStackable)
    {
      int from = i + 1;

      for (size_t j = from; j < inventory.size(); j++)
      {
        auto go2 = inventory.at(j).get();
        ItemComponent* nextItem = go2->GetComponent<ItemComponent>();
        if (nextItem->Data.IsIdentified
         && nextItem->Data.IsStackable
         && currentItem->Data.ItemTypeHash == nextItem->Data.ItemTypeHash)
        {
          currentItem->Data.Amount += nextItem->Data.Amount;
          go2->IsDestroyed = true;
          shouldCleanup = true;
        }
      }
    }
  }

  if (shouldCleanup)
  {
    auto newBegin = std::remove_if(inventory.begin(),
                                   inventory.end(),
    [this](const std::unique_ptr<GameObject>& go)
    {
      if (go != nullptr && go->IsDestroyed)
      {
        return true;
      }

      return false;
    });

    inventory.erase(newBegin, inventory.end());
  }
}
