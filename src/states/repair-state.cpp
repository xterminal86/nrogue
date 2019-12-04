#include "repair-state.h"

#include "application.h"
#include "map.h"
#include "printer.h"

void RepairState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;  
}

void RepairState::Prepare()
{
  _itemRefByChar.clear();
}

void RepairState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case 'q':
      Application::Instance().ChangeState(GameStates::INVENTORY_STATE);
      break;

    default:
    {
      if (_itemRefByChar.count(_keyPressed))
      {
        ItemComponent* ic = _itemRefByChar[_keyPressed];
        if (ic->Data.Durability.Min().Get() == ic->Data.Durability.Max().Get())
        {
          Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "This item looks undamaged" }, GlobalConstants::MessageBoxDefaultBorderColor);
        }
        else
        {
          RepairItem(ic);
        }
      }
    }
    break;
  }
}

void RepairState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    DrawHeader(" REPAIR ITEMS ");

    int itemIndex = 0;
    for (auto& i : _playerRef->Inventory.Contents)
    {
      ItemComponent* ic = i->GetComponent<ItemComponent>();
      if (ic->Data.IsEquipped && ic->Data.IsWeaponOrArmor())
      {
        std::string name = ic->Data.IsIdentified ? ic->Data.IdentifiedName : ic->Data.UnidentifiedName;

        char c = GlobalConstants::AlphabetLowercase[itemIndex];
        std::string str;

        if (ic->Data.IsIdentified)
        {
          str = Util::StringFormat("'%c' - %s (%i/%i)",
                                   c,
                                   name.data(),
                                   ic->Data.Durability.Min().Get(),
                                   ic->Data.Durability.Max().Get());
        }
        else
        {
          str = Util::StringFormat(R"('%c' - %s (??/??))", c, name.data());
        }

        Printer::Instance().PrintFB(0, 2 + itemIndex, str, Printer::kAlignLeft, "#FFFFFF");

        _itemRefByChar[c] = ic;

        itemIndex++;
      }
    }

    Printer::Instance().PrintFB(tw / 2, th - 1, "'q' - cancel", Printer::kAlignCenter, "#FFFFFF");

    Printer::Instance().Render();
  }
}

void RepairState::SetRepairKitRef(ItemComponent* item, int inventoryIndex)
{
  _repairKit = item;
  _inventoryIndex = inventoryIndex;
}

void RepairState::RepairItem(ItemComponent* itemToRepair)
{
  int maxDur = itemToRepair->Data.Durability.Max().Get();
  int currentDur = itemToRepair->Data.Durability.Min().Get();

  int repaired = 0;

  while (currentDur < maxDur && _repairKit->Data.Amount > 0)
  {
    if (_repairKit->Data.Prefix == ItemPrefix::BLESSED)
    {
      _repairKit->Data.Amount--;
      currentDur += 2;
      repaired += 2;
      itemToRepair->Data.Durability.AddMin(2);
    }
    else if (_repairKit->Data.Prefix == ItemPrefix::UNCURSED)
    {
      _repairKit->Data.Amount--;
      currentDur++;
      repaired++;
      itemToRepair->Data.Durability.AddMin(1);
    }
    else if (_repairKit->Data.Prefix == ItemPrefix::CURSED)
    {
      _repairKit->Data.Amount -= 2;
      currentDur++;
      repaired++;
      itemToRepair->Data.Durability.AddMin(1);
    }
  }

  if (currentDur > maxDur)
  {
    itemToRepair->Data.Durability.Restore();
  }

  if (_repairKit->Data.Amount < 0)
  {
    _repairKit->Data.Amount = 0;
  }

  auto str = Util::StringFormat("You've repaired %i durability", repaired);
  Printer::Instance().AddMessage(str);

  _playerRef->FinishTurn();

  Application::Instance().ChangeState(GameStates::MAIN_STATE);  
}

void RepairState::Cleanup()
{
  if (_repairKit->Data.Amount == 0)
  {
    auto it = _playerRef->Inventory.Contents.begin();
    _playerRef->Inventory.Contents.erase(it + _inventoryIndex);
    Printer::Instance().AddMessage("Repair kit has been used up!");
  }
}
