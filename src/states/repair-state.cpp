#include "repair-state.h"

#include "application.h"
#include "map.h"
#include "printer.h"

void RepairState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;

  _headerText = " REPAIR ITEMS ";

  _itemRefByChar.reserve(Strings::AlphabetLowercase.length());
}

// =============================================================================

void RepairState::Prepare()
{
  _itemRefByChar.clear();

  int itemIndex = 0;
  for (auto& i : _playerRef->Inventory->Contents)
  {
    ItemComponent* ic = i->GetComponent<ItemComponent>();
    if (ic->Data.IsEquipped && ic->Data.IsWeaponOrArmor())
    {
      _itemRefByChar.push_back(ic);
      itemIndex++;
    }
  }
}

// =============================================================================

void RepairState::ProcessInput()
{
  switch (_keyPressed)
  {
    case VK_CANCEL:
      Application::Instance().RecordAction(_keyPressed);
      Application::Instance().ChangeState(GameStates::INVENTORY_STATE);
      break;

    default:
    {
      int index = (_keyPressed - 'a');
      if (index >= 0 && index < (int)_itemRefByChar.size())
      {
        ItemComponent* ic = _itemRefByChar[index];
        if (ic->Data.Durability.Min().Get() == ic->Data.Durability.Max().Get())
        {
          Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                                 Strings::MessageBoxInformationHeaderText,
                                                 { Strings::MsgItemUndamaged },
                                                 Colors::ShadesOfGrey::Six);
        }
        else
        {
          Application::Instance().RecordAction(_keyPressed);
          RepairItem(ic);
        }
      }
    }
    break;
  }
}

// =============================================================================

void RepairState::DrawSpecific()
{
  for (size_t i = 0; i < _itemRefByChar.size(); i++)
  {
    char c = 'a' + i;
    ItemComponent* ic = _itemRefByChar[i];

    std::string name = ic->Data.IsIdentified ?
                       ic->Data.IdentifiedName :
                       ic->Data.UnidentifiedName;

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

    Printer::Instance().PrintFB(1,
                                2 + i,
                                str,
                                Printer::kAlignLeft,
                                Colors::WhiteColor);
  }
}

// =============================================================================

void RepairState::SetRepairKitRef(ItemComponent* item, int inventoryIndex)
{
  _repairKit = item;
  _inventoryIndex = inventoryIndex;
}

// =============================================================================

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

// =============================================================================

void RepairState::Cleanup()
{
  if (_repairKit->Data.Amount == 0)
  {
    auto it = _playerRef->Inventory->Contents.begin();
    _playerRef->Inventory->Contents.erase(it + _inventoryIndex);
    Printer::Instance().AddMessage("Repair kit has been used up!");
  }
}
