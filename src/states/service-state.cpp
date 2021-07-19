#include "service-state.h"
#include "printer.h"
#include "application.h"
#include "npc-interact-state.h"
#include "trader-component.h"
#include "ai-npc.h"

void ServiceState::Prepare()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void ServiceState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_CANCEL:
    {
      auto res = Application::Instance().GetGameStateRefByName(GameStates::NPC_INTERACT_STATE);
      NPCInteractState* nis = static_cast<NPCInteractState*>(res);
      nis->SetNPCRef(_shopOwner->NpcRef);
      Application::Instance().ChangeState(GameStates::NPC_INTERACT_STATE);
    }
    break;

    default:
    {
      if (_serviceInfoByChar.count(_keyPressed))
      {
        ProcessItem(_keyPressed);
      }
    }
    break;
  }
}

void ServiceState::ProcessItem(int key)
{
  switch (_shopOwner->NpcRef->Data.ProvidesService)
  {
    case ServiceType::REPAIR:
      ProcessRepair(key);
      break;

    case ServiceType::IDENTIFY:
      ProcessIdentify(key);
      break;

    case ServiceType::BLESS:
      ProcessBlessing(key);
      break;
  }
}

void ServiceState::ProcessRepair(int key)
{
  ServiceInfo& si = _serviceInfoByChar[key];
  if (_playerRef->Money < si.ServiceCost)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail!", { "You have no money, ha ha ha!" }, GlobalConstants::MessageBoxRedBorderColor);
  }
  else
  {
    _playerRef->Money -= si.ServiceCost;
    si.ItemComponentRef->Data.Durability.Restore();
    FillItemsForRepair();
  }
}

void ServiceState::ProcessIdentify(int key)
{
  ServiceInfo& si = _serviceInfoByChar[key];
  if (_playerRef->Money < si.ServiceCost)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail!", { "Not enough money!" }, GlobalConstants::MessageBoxRedBorderColor);
  }
  else
  {
    _playerRef->Money -= si.ServiceCost;
    si.ItemComponentRef->Data.IsIdentified = true;
    FillItemsForIdentify();
  }
}

void ServiceState::ProcessBlessing(int key)
{
  ServiceInfo& si = _serviceInfoByChar[key];
  if (_playerRef->Money < si.ServiceCost)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Damn Nation!", { "No donation - no salvation!" }, GlobalConstants::MessageBoxRedBorderColor);
  }
  else
  {
    _playerRef->Money -= si.ServiceCost;

    // If item wasn't identified but was actually already blessed,
    // nothing happens. Money is forfeit tho.
    if (si.ItemComponentRef->Data.Prefix != ItemPrefix::BLESSED)
    {
      si.ItemComponentRef->Data.Prefix = ItemPrefix::BLESSED;

      bool isWeaponOrArmor = (si.ItemComponentRef->Data.ItemType_ == ItemType::WEAPON ||si.ItemComponentRef->Data.ItemType_ == ItemType::ARMOR);
      if (isWeaponOrArmor)
      {
        // BUC adjust is private inside GameObjectsFactory, so we have to
        // kinda repeat it here.
        //
        // Blessed / cursed item effects are handled in item use handler,
        // so no need to do anything special here.
        //
        for (auto& kvp : _playerRef->_attributesRefsByBonus)
        {
         ItemBonusStruct* ibs = si.ItemComponentRef->Data.GetBonus(kvp.first);
         if (ibs != nullptr && ibs->FromItem)
         {
           // Only +1 bonus from blessing and even then
           // I'm not sure whether it will be OP or not
           ibs->BonusValue += 1;

           // Now we need to replace header BUC status name
           // that was generated during object creation.
           std::string header = si.ItemComponentRef->Data.IdentifiedName;

           std::vector<std::string> toFind =
           {
             "Cursed", "Uncursed"
           };

           for (auto& s : toFind)
           {
             size_t pos = header.find(s);
             if (pos != std::string::npos)
             {
               si.ItemComponentRef->Data.IdentifiedName = header.replace(pos, s.length(), "Blessed");
               break;
             }
           }

           // If item to be blessed is actually equipped, do the shuffle below.
           auto equippedItem = _playerRef->EquipmentByCategory[si.ItemComponentRef->Data.EqCategory][0];
           if (equippedItem == si.ItemComponentRef)
           {
             // Unequip / equip to reapply stat bonuses
             equippedItem->Equip();
             equippedItem->Equip();

             Printer::Instance().Messages().erase(Printer::Instance().Messages().begin());
             Printer::Instance().Messages().erase(Printer::Instance().Messages().begin());
           }
         }
        }
      }
    }

    si.ItemComponentRef->Data.IsPrefixDiscovered = true;

    FillItemsForBlessing();
  }
}

void ServiceState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawHeader(_serviceNameByType.at(_shopOwner->NpcRef->Data.ProvidesService));

    DisplayItems();

    std::string youHaveStr = "You have: ";
    auto playerMoney = Util::StringFormat("$ %i", _playerRef->Money);

    Printer::Instance().PrintFB(1, _th - 1, youHaveStr, Printer::kAlignLeft, GlobalConstants::WhiteColor);
    Printer::Instance().PrintFB(1 + youHaveStr.length(), _th - 1, playerMoney, Printer::kAlignLeft, GlobalConstants::CoinsColor);

    Printer::Instance().PrintFB(_tw / 2, _th - 1, "'q' - exit ", Printer::kAlignCenter, GlobalConstants::WhiteColor);

    Printer::Instance().Render();
  }
}

void ServiceState::DisplayItems()
{
  if (_serviceInfoByChar.empty())
  {
    Printer::Instance().PrintFB(_twHalf, 2, _displayOnEmptyItems.at(_shopOwner->NpcRef->Data.ProvidesService), Printer::kAlignCenter, GlobalConstants::WhiteColor);
  }
  else
  {
    int itemIndex = 0;
    for (auto& kvp : _serviceInfoByChar)
    {
      ServiceInfo& ri = kvp.second;

      std::string cost = Util::StringFormat("$%i", ri.ServiceCost);

      Printer::Instance().PrintFB(1, 2 + itemIndex, ri.NameToDisplay, Printer::kAlignLeft, ri.Color);

      // Replace letter and dash with white color
      // in case item is blessed or cursed.
      Printer::Instance().PrintFB(1, 2 + itemIndex, ri.Letter + " - ", Printer::kAlignLeft, GlobalConstants::WhiteColor);

      Printer::Instance().PrintFB(1 + _maxStrLen + 1, 2 + itemIndex, cost, Printer::kAlignLeft, GlobalConstants::CoinsColor);

      itemIndex++;
    }
  }
}

void ServiceState::Setup(TraderComponent* shopOwner)
{
  _playerRef = &Application::Instance().PlayerInstance;

  _shopOwner = shopOwner;

  switch (_shopOwner->NpcRef->Data.ProvidesService)
  {
    case ServiceType::REPAIR:
      FillItemsForRepair();
      break;

    case ServiceType::IDENTIFY:
      FillItemsForIdentify();
      break;

    case ServiceType::BLESS:
      FillItemsForBlessing();
      break;
  }
}

void ServiceState::FillItemsForBlessing()
{
  _maxStrLen = 0;

  _serviceInfoByChar.clear();

  int itemIndex = 0;
  for (auto& item : _playerRef->Inventory.Contents)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();

    bool alreadyBlessed = ((ic->Data.IsIdentified || ic->Data.IsPrefixDiscovered)
                        && (ic->Data.Prefix == ItemPrefix::BLESSED));

    bool isValid        = (ic->Data.ItemType_ != ItemType::DUMMY);

    if (alreadyBlessed || !isValid)
    {
      continue;
    }

    char c = GlobalConstants::AlphabetLowercase[itemIndex];

    std::string nameToDisplay = (ic->Data.IsIdentified ? item->ObjectName : ic->Data.UnidentifiedName);
    std::string charStr = Util::StringFormat("'%c'", c);
    std::string str = Util::StringFormat("%s - %s", charStr.data(), nameToDisplay.data());

    if (_maxStrLen < str.length())
    {
      _maxStrLen = str.length();
    }

    auto color = Util::GetItemInventoryColor(ic->Data);

    int serviceCost = ic->Data.GetCost() * 5;

    _serviceInfoByChar[c] = { charStr, str, color, ic, serviceCost };

    itemIndex++;
  }
}

void ServiceState::FillItemsForIdentify()
{
  _maxStrLen = 0;

  _serviceInfoByChar.clear();

  int itemIndex = 0;
  for (auto& item : _playerRef->Inventory.Contents)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();
    if (ic->Data.IsIdentified)
    {
      continue;
    }

    char c = GlobalConstants::AlphabetLowercase[itemIndex];

    std::string charStr = Util::StringFormat("'%c'", c);
    std::string str = Util::StringFormat("%s - %s", charStr.data(), ic->Data.UnidentifiedName.data());

    if (_maxStrLen < str.length())
    {
      _maxStrLen = str.length();
    }

    _serviceInfoByChar[c] = { charStr, str, "#FFFFFF", ic, 100 };

    itemIndex++;
  }
}

void ServiceState::FillItemsForRepair()
{
  _maxStrLen = 0;

  _serviceInfoByChar.clear();

  int itemIndex = 0;
  for (auto& item : _playerRef->Inventory.Contents)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();
    if (!ic->Data.IsWeaponOrArmor() || !ic->Data.RepairNeeded())
    {
      continue;
    }

    std::string name = ic->Data.IsIdentified ?
                       item->ObjectName :
                       ic->Data.UnidentifiedName;

    char c = GlobalConstants::AlphabetLowercase[itemIndex];
    std::string str;
    std::string dur;

    if (ic->Data.IsIdentified)
    {
      dur = Util::StringFormat("(%i/%i)", ic->Data.Durability.Min().Get(), ic->Data.Durability.Max().Get());
    }
    else
    {
      dur = R"((??/??))";
    }

    std::string charStr = Util::StringFormat("'%c'", c);
    str = Util::StringFormat("%s - %s %s", charStr.data(), name.data(), dur.data());

    if (_maxStrLen < str.length())
    {
      _maxStrLen = str.length();
    }

    int toRepair = ic->Data.Durability.Max().Get() - ic->Data.Durability.Min().Get();
    if (ic->Data.Prefix == ItemPrefix::BLESSED)
    {
      toRepair *= 2;
    }
    else if (ic->Data.Prefix == ItemPrefix::UNCURSED)
    {
      toRepair *= 1;
    }
    else if (ic->Data.Prefix == ItemPrefix::CURSED)
    {
      toRepair *= 0.8f;
    }

    _serviceInfoByChar[c] = { charStr, str, "#FFFFFF", ic, toRepair };

    itemIndex++;
  }
}
