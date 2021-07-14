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

    si.ItemComponentRef->Data.Prefix = ItemPrefix::BLESSED;
    si.ItemComponentRef->Data.IsPrefixDiscovered = true;

    // TODO: recalculate blessed parameters for weapons and armor
    // since they're created inside factory right now.
    // Ignore already existing blessing for unidentified items.

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

      Printer::Instance().PrintFB(1, 2 + itemIndex, ri.NameToDisplay, Printer::kAlignLeft, GlobalConstants::WhiteColor);
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

    bool alreadyBlessed = (ic->Data.IsPrefixDiscovered
                        && ic->Data.Prefix == ItemPrefix::BLESSED);

    bool isValid        = (ic->Data.ItemType_ != ItemType::DUMMY);

    if (alreadyBlessed || !isValid)
    {
      continue;
    }

    char c = GlobalConstants::AlphabetLowercase[itemIndex];

    std::string str = Util::StringFormat("'%c' - %s", c, ic->Data.UnidentifiedName.data());

    if (_maxStrLen < str.length())
    {
      _maxStrLen = str.length();
    }

    _serviceInfoByChar[c] = { str, ic, 200 };

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

    std::string str = Util::StringFormat("'%c' - %s", c, ic->Data.UnidentifiedName.data());

    if (_maxStrLen < str.length())
    {
      _maxStrLen = str.length();
    }

    _serviceInfoByChar[c] = { str, ic, 100 };

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

    str = Util::StringFormat("'%c' - %s %s", c, name.data(), dur.data());

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

    _serviceInfoByChar[c] = { str, ic, toRepair };

    itemIndex++;
  }
}
