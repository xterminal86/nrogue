#include "service-state.h"
#include "printer.h"
#include "application.h"
#include "npc-interact-state.h"
#include "trader-component.h"
#include "ai-npc.h"

void ServiceState::Prepare()
{
  _playerRef = &Application::Instance().PlayerInstance;
  _headerText = _serviceNameByType.at(_shopOwner->NpcRef->Data.ProvidesService);
}

// =============================================================================

void ServiceState::ProcessInput()
{
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

// =============================================================================

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

// =============================================================================

void ServiceState::ProcessRepair(int key)
{
  ServiceInfo& si = _serviceInfoByChar[key];
  if (_playerRef->Money < si.ServiceCost)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                           Strings::MessageBoxEpicFailHeaderText,
                                           { Strings::MsgNoMoney },
                                           Colors::MessageBoxRedBorderColor);
  }
  else
  {
    _playerRef->Money -= si.ServiceCost;
    si.ItemComponentRef->Data.Durability.Restore();
    FillItemsForRepair();
  }
}

// =============================================================================

void ServiceState::ProcessIdentify(int key)
{
  ServiceInfo& si = _serviceInfoByChar[key];
  if (_playerRef->Money < si.ServiceCost)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                           Strings::MessageBoxEpicFailHeaderText,
                                           { Strings::MsgNotEnoughMoney },
                                           Colors::MessageBoxRedBorderColor);
  }
  else
  {
    _playerRef->Money -= si.ServiceCost;
    si.ItemComponentRef->Data.IsIdentified = true;
    FillItemsForIdentify();
  }
}

// =============================================================================

void ServiceState::ProcessBlessing(int key)
{
  ServiceInfo& si = _serviceInfoByChar[key];
  if (_playerRef->Money < si.ServiceCost)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                           "Damn Nation!",
                                           { "No donation - no salvation!" },
                                           Colors::MessageBoxRedBorderColor);
  }
  else
  {
    _playerRef->Money -= si.ServiceCost;

    //
    // If item wasn't identified but was actually already blessed,
    // nothing happens. Money is forfeit tho.
    //
    if (si.ItemComponentRef->Data.Prefix != ItemPrefix::BLESSED)
    {
      Util::UpdateItemPrefix(si.ItemComponentRef, ItemPrefix::BLESSED);

      BlessItem(si);

      //
      // Now we need to replace header BUC status name
      // that was generated during object creation.
      //
      si.ItemComponentRef->Data.IdentifiedName = Util::ReplaceItemPrefix(si.ItemComponentRef->Data.IdentifiedName, { "Cursed" , "Uncursed" }, "Blessed");
    }

    si.ItemComponentRef->Data.IsPrefixDiscovered = true;

    FillItemsForBlessing();
  }
}

// =============================================================================

void ServiceState::BlessItem(const ServiceInfo& si)
{
  //
  // Blessed / cursed item effects (like potions)
  // are handled in item use handler,
  // so no need to do anything special here.
  //
  if (!si.ItemComponentRef->Data.Bonuses.empty())
  {
    Util::BlessItem(si.ItemComponentRef);
  }
  else
  {
    if (si.ItemComponentRef->Data.ItemType_ == ItemType::WAND)
    {
      Util::RecalculateWandStats(si.ItemComponentRef);
    }
  }

  ItemComponent* equippedItem = nullptr;

  if (si.ItemComponentRef->Data.IsEquipped)
  {
    auto& cat = si.ItemComponentRef->Data.EqCategory;

    //
    // There can be two rings.
    //
    if (cat == EquipmentCategory::RING)
    {
      auto leftHand  = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::RING][0];
      auto rightHand = _playerRef->Equipment->EquipmentByCategory[EquipmentCategory::RING][1];

      if (leftHand == si.ItemComponentRef
       || rightHand == si.ItemComponentRef)
      {
        equippedItem = si.ItemComponentRef;
      }
    }
    else if (cat != EquipmentCategory::NOT_EQUIPPABLE)
    {
      equippedItem = _playerRef->Equipment->EquipmentByCategory[cat][0];
    }

    //
    // NOTE: old code used double Equip() call to reapply bonuses.
    // Think on why, maybe there is some special case I forgot about.
    //
    if (equippedItem == si.ItemComponentRef)
    {
      Util::ReapplyBonuses(_playerRef, equippedItem);
    }
  }
}

// =============================================================================

void ServiceState::DrawSpecific()
{
  DisplayItems();

  std::string youHaveStr = "You have: ";
  auto playerMoney = Util::StringFormat("$ %i", _playerRef->Money);

  Printer::Instance().PrintFB(1,
                              _th - 1,
                              youHaveStr,
                              Printer::kAlignLeft,
                              Colors::WhiteColor);

  Printer::Instance().PrintFB(1 + youHaveStr.length(),
                              _th - 1,
                              playerMoney,
                              Printer::kAlignLeft,
                              Colors::CoinsColor);
}

// =============================================================================

void ServiceState::DisplayItems()
{
  if (_serviceInfoByChar.empty())
  {
    Printer::Instance().PrintFB(_twHalf,
                                _thHalf,
                                _displayOnEmptyItems.at(_shopOwner->NpcRef->Data.ProvidesService),
                                Printer::kAlignCenter,
                                Colors::WhiteColor);
  }
  else
  {
    int itemIndex = 0;
    for (auto& kvp : _serviceInfoByChar)
    {
      ServiceInfo& ri = kvp.second;

      std::string cost = Util::StringFormat("$%i", ri.ServiceCost);

      Printer::Instance().PrintFB(1,
                                  2 + itemIndex,
                                  ri.NameToDisplay,
                                  Printer::kAlignLeft,
                                  ri.Color);

      //
      // Replace letter and dash with white color
      // in case item is blessed or cursed.
      //
      Printer::Instance().PrintFB(1,
                                  2 + itemIndex,
                                  ri.Letter + " - ",
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor);

      Printer::Instance().PrintFB(1 + _maxStrLen + 1,
                                  2 + itemIndex,
                                  cost,
                                  Printer::kAlignLeft,
                                  Colors::CoinsColor);

      itemIndex++;
    }
  }
}

// =============================================================================

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

// =============================================================================

void ServiceState::FillItemsForBlessing()
{
  _maxStrLen = 0;

  _serviceInfoByChar.clear();

  int itemIndex = 0;
  for (auto& item : _playerRef->Inventory->Contents)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();

    bool alreadyKnown   = (ic->Data.IsIdentified || ic->Data.IsPrefixDiscovered);
    bool alreadyBlessed = (alreadyKnown && ic->Data.Prefix == ItemPrefix::BLESSED);
    bool isUnique       = (ic->Data.Rarity == ItemRarity::UNIQUE);
    bool cantBeBlessed  = (ic->Data.ItemType_ == ItemType::GEM
                        || ic->Data.ItemType_ == ItemType::RETURNER);

    if (cantBeBlessed || alreadyBlessed || isUnique)
    {
      continue;
    }

    int validBonuses = GetValidBonusesCount(ic);

    bool noValidBonuses = (validBonuses == 0);
    bool isDummy        = (ic->Data.ItemType_ == ItemType::DUMMY);
    bool isUncursed     = (ic->Data.Prefix == ItemPrefix::UNCURSED);
    bool isEquippable   = (ic->Data.EqCategory != EquipmentCategory::NOT_EQUIPPABLE);

    bool canBeEquippedButInvalid = (isEquippable
                                 && isUncursed
                                 && noValidBonuses);

    //
    // If it's an uncursed item that can be equipped, but it
    // contains invalid bonuses (like uncursed ring of reflection),
    // ignore it. Also ignore dummy items like notes.
    //
    if (alreadyKnown && (isDummy || canBeEquippedButInvalid))
    {
      continue;
    }

    char c = Strings::AlphabetLowercase[itemIndex];

    std::string nameToDisplay = (ic->Data.IsIdentified ? item->ObjectName : ic->Data.UnidentifiedName);
    std::string charStr = Util::StringFormat("'%c'", c);
    std::string str = Util::StringFormat("%s - %s", charStr.data(), nameToDisplay.data());

    if (_maxStrLen < str.length())
    {
      _maxStrLen = str.length();
    }

    auto color = Util::GetItemInventoryColor(ic->Data);

    int totalCost = ic->Data.GetCost();
    for (auto& b : ic->Data.Bonuses)
    {
      auto found = std::find(_invalidBonusTypes.begin(),
                             _invalidBonusTypes.end(),
                             b.Type);

      bool bonusIsInvalid = (found != _invalidBonusTypes.end());

      totalCost += (bonusIsInvalid
                 ? (b.MoneyCostIncrease / 2)
                 : b.MoneyCostIncrease);
    }

    int serviceCost = totalCost;

    _serviceInfoByChar[c] = { charStr, str, color, ic, serviceCost };

    itemIndex++;
  }
}

// =============================================================================

int ServiceState::GetValidBonusesCount(ItemComponent* ic)
{
  int count = 0;
  for (auto& i : ic->Data.Bonuses)
  {
    auto found = std::find(_invalidBonusTypes.begin(),
                           _invalidBonusTypes.end(),
                           i.Type);

    if (found == _invalidBonusTypes.end())
    {
      count++;
    }
  }

  return count;
}

// =============================================================================

void ServiceState::FillItemsForIdentify()
{
  _maxStrLen = 0;

  _serviceInfoByChar.clear();

  int itemIndex = 0;
  for (auto& item : _playerRef->Inventory->Contents)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();
    if (ic->Data.IsIdentified)
    {
      continue;
    }

    char c = Strings::AlphabetLowercase[itemIndex];

    std::string charStr = Util::StringFormat("'%c'", c);
    std::string str = Util::StringFormat("%s - %s", charStr.data(), ic->Data.UnidentifiedName.data());

    if (_maxStrLen < str.length())
    {
      _maxStrLen = str.length();
    }

    _serviceInfoByChar[c] = { charStr, str, Colors::WhiteColor, ic, 100 };

    itemIndex++;
  }
}

// =============================================================================

void ServiceState::FillItemsForRepair()
{
  _maxStrLen = 0;

  _serviceInfoByChar.clear();

  int itemIndex = 0;
  for (auto& item : _playerRef->Inventory->Contents)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();
    if (!ic->Data.IsWeaponOrArmor() || !ic->Data.RepairNeeded())
    {
      continue;
    }

    std::string name = ic->Data.IsIdentified ?
                       item->ObjectName :
                       ic->Data.UnidentifiedName;

    char c = Strings::AlphabetLowercase[itemIndex];
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
      toRepair *= 0.8;
    }

    _serviceInfoByChar[c] = { charStr, str, Colors::WhiteColor, ic, toRepair };

    itemIndex++;
  }
}
