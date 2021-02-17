#include "shopping-state.h"

#include "printer.h"
#include "application.h"
#include "util.h"
#include "npc-interact-state.h"
#include "trader-component.h"
#include "ai-npc.h"
#include "player.h"

void ShoppingState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;

  _tw = Printer::TerminalWidth;
  _th = Printer::TerminalHeight;
}

void ShoppingState::Prepare()
{
  _inventoryItemIndex = 0;
  _playerSide = (!_playerRef->Inventory.IsEmpty());
}

void ShoppingState::PassShopOwner(TraderComponent* tc)
{
  _shopOwner = tc;

  _shopTitle = tc->ShopTitle;

  tc->NpcRef->Data.IsAquainted = true;
}

void ShoppingState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case ALT_K2:
    case NUMPAD_2:
      _inventoryItemIndex++;
      break;

    case ALT_K8:
    case NUMPAD_8:
      _inventoryItemIndex--;
      break;

    case ALT_K4:
    case NUMPAD_4:
    {
      if (!_playerRef->Inventory.IsEmpty())
      {
        _playerSide = true;
      }
    }
    break;

    case ALT_K6:
    case NUMPAD_6:
    {
      if (!_shopOwner->Items.empty())
      {
        _playerSide = false;
      }
    }
    break;

    case VK_ENTER:
      BuyOrSellItem();
      break;

    case 'i':
      ShowItemInfo();
      break;

    case VK_CANCEL:
    {
      auto res = Application::Instance().GetGameStateRefByName(GameStates::NPC_INTERACT_STATE);
      NPCInteractState* nis = static_cast<NPCInteractState*>(res);
      nis->SetNPCRef(_shopOwner->NpcRef);
      Application::Instance().ChangeState(GameStates::NPC_INTERACT_STATE);
    }
    break;

    #ifdef DEBUG_BUILD
    case 'r':
      _shopOwner->RefreshStock();
      break;
    #endif

    default:
      break;
  }

  CheckIndexLimits();
}

void ShoppingState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    DrawHeader(_shopTitle);

    for (int y = 1; y < _th; y++)
    {
      #ifdef USE_SDL
      Printer::Instance().PrintFB(_tw / 2, y, (int)NameCP437::VBAR_2, GlobalConstants::WhiteColor);
      #else
      Printer::Instance().PrintFB(_tw / 2, y, '|', GlobalConstants::BlackColor, GlobalConstants::WhiteColor);
      #endif
    }

    DisplayPlayerInventory();
    DisplayShopInventory();

    auto playerMoney = Util::StringFormat("You have: %i $", _playerRef->Money);
    Printer::Instance().PrintFB(1, _th - 1, playerMoney, Printer::kAlignLeft, GlobalConstants::CoinsColor);

    Printer::Instance().PrintFB(_tw, _th - 1, "'i' - inspect ", Printer::kAlignRight, GlobalConstants::WhiteColor);

    Printer::Instance().Render();
  }
}

void ShoppingState::DisplayPlayerInventory()
{
  int itemsCount = 0;

  int yPos = 2;
  int index = 0;

  std::string costString;

  int itemStringTotalLen = GetItemStringTotalLen(_playerRef->Inventory.Contents);

  for (auto& item : _playerRef->Inventory.Contents)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();

    std::string nameInInventory = ic->Data.IsIdentified ? item->ObjectName : ic->Data.UnidentifiedName;
    nameInInventory.resize(GlobalConstants::InventoryMaxNameLength, ' ');

    int extraInfoStringPosX = GlobalConstants::InventoryMaxNameLength + 1;

    std::string extraInfo = GetItemExtraInfo(ic);

    Printer::Instance().PrintFB(extraInfoStringPosX + 1,
                                 yPos + index,
                                 extraInfo,
                                 Printer::kAlignLeft,
                                 GlobalConstants::WhiteColor);

    int cost = GetCost(ic, true);

    costString = Util::StringFormat(" $ %i", cost);

    Printer::Instance().PrintFB(extraInfoStringPosX + itemStringTotalLen + 1,
                                 yPos + index,
                                 costString,
                                 Printer::kAlignLeft,
                                 GlobalConstants::CoinsColor);

    std::string textColor = Util::GetItemInventoryColor(ic->Data);

    if (_playerSide && index == _inventoryItemIndex)
    {
      Printer::Instance().PrintFB(1,
                                   yPos + index,
                                   nameInInventory,
                                   Printer::kAlignLeft,
                                   GlobalConstants::BlackColor,
                                   GlobalConstants::WhiteColor);
    }
    else
    {
      Printer::Instance().PrintFB(1,
                                   yPos + index,
                                   nameInInventory,
                                   Printer::kAlignLeft,
                                   textColor);
    }

    index++;

    itemsCount++;
  }

  for (size_t i = itemsCount; i < GlobalConstants::InventoryMaxNameLength; i++)
  {
    std::string stub(GlobalConstants::InventoryMaxNameLength, '-');
    Printer::Instance().PrintFB(1,
                                 yPos + index,
                                 stub,
                                 Printer::kAlignLeft,
                                 GlobalConstants::WhiteColor);
    yPos++;
  }
}

void ShoppingState::DisplayShopInventory()
{
  int itemsCount = 0;

  int yPos = 2;
  int index = 0;

  int tw = Printer::TerminalWidth;

  int xPos = tw - 1;

  int itemStringTotalLen = GetItemStringTotalLen(_shopOwner->Items);

  for (auto& item : _shopOwner->Items)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();

    std::string nameInInventory = ic->Data.IsIdentified ? item->ObjectName : ic->Data.UnidentifiedName;

    std::string tmpName = nameInInventory;

    if (tmpName.length() > GlobalConstants::InventoryMaxNameLength)
    {
      tmpName.resize(GlobalConstants::InventoryMaxNameLength);
    }

    nameInInventory.insert(0, GlobalConstants::InventoryMaxNameLength - tmpName.length(), ' ');
    nameInInventory.resize(GlobalConstants::InventoryMaxNameLength);

    std::string extraInfo = GetItemExtraInfo(ic);

    Printer::Instance().PrintFB(xPos - GlobalConstants::InventoryMaxNameLength - 1,
                                 yPos + index,
                                 extraInfo,
                                 Printer::kAlignRight,
                                 GlobalConstants::WhiteColor);

    int cost = GetCost(ic, false);

    std::string costString = Util::StringFormat("%i $ ", cost);

    int bonusStringPosX = xPos - GlobalConstants::InventoryMaxNameLength - 1;

    Printer::Instance().PrintFB(bonusStringPosX - itemStringTotalLen, yPos + index, costString, Printer::kAlignRight, GlobalConstants::CoinsColor);

    std::string textColor = Util::GetItemInventoryColor(ic->Data);

    if (!_playerSide && index == _inventoryItemIndex)
    {
      Printer::Instance().PrintFB(xPos,
                                   yPos + index,
                                   nameInInventory,
                                   Printer::kAlignRight,
                                   GlobalConstants::BlackColor,
                                   GlobalConstants::WhiteColor);
    }
    else
    {
      Printer::Instance().PrintFB(xPos,
                                   yPos + index,
                                   nameInInventory,
                                   Printer::kAlignRight,
                                   textColor);
    }

    index++;

    itemsCount++;
  }

  for (size_t i = itemsCount; i < GlobalConstants::InventoryMaxNameLength; i++)
  {
    std::string stub(GlobalConstants::InventoryMaxNameLength, '-');
    Printer::Instance().PrintFB(xPos,
                                 yPos + index,
                                 stub,
                                 Printer::kAlignRight,
                                 GlobalConstants::WhiteColor);
    yPos++;
  }
}

void ShoppingState::CheckIndexLimits()
{
  int invSize = 0;

  if (_playerSide)
  {
    invSize = _playerRef->Inventory.Contents.size() - 1;
  }
  else
  {
    if (!_shopOwner->Items.empty())
    {
      invSize = _shopOwner->Items.size() - 1;
    }
  }

  _inventoryItemIndex = Util::Clamp(_inventoryItemIndex, 0, invSize);
}

size_t ShoppingState::GetItemStringTotalLen(std::vector<std::unique_ptr<GameObject>>& container)
{
  size_t maxLen = 0;

  for (auto& item : container)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();

    std::string bonusInfo = GetItemExtraInfo(ic);

    if (bonusInfo.length() > maxLen)
    {
      maxLen = bonusInfo.length();
    }
  }

  return maxLen;
}

std::string ShoppingState::GetItemExtraInfo(ItemComponent* item)
{
  std::string extraInfo;

  bool stackCond = (item->Data.IsStackable && item->Data.Amount > 1);
  if (item->Data.IsIdentified
  && (stackCond || item->Data.IsChargeable))
  {
    extraInfo = Util::StringFormat("(%i)", item->Data.Amount);
  }
  else if (item->Data.IsEquipped)
  {
    extraInfo = "E";
  }

  return extraInfo;
}

void ShoppingState::BuyOrSellItem()
{
  if (_playerRef->Inventory.IsEmpty()
   && _shopOwner->Items.empty())
  {
    return;
  }

  if (_playerSide)
  {
    auto go = _playerRef->Inventory.Contents[_inventoryItemIndex].get();
    ItemComponent* ic = go->GetComponent<ItemComponent>();

    if (ic->Data.IsEquipped)
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                              "Information",
                                              { "Unequip first!" },
                                              GlobalConstants::MessageBoxRedBorderColor);
      return;
    }

    if (ic->Data.IsImportant)
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                              "Information",
                                              { "This looks important - better hold on to it" },
                                              GlobalConstants::MessageBoxBlueBorderColor);
      return;
    }

    int cost = GetCost(ic, true);

    _playerRef->Money += cost;

    auto it = _playerRef->Inventory.Contents.begin();
    _playerRef->Inventory.Contents.erase(it + _inventoryItemIndex);
  }
  else
  {
    auto go = _shopOwner->Items[_inventoryItemIndex].get();
    ItemComponent* ic = go->GetComponent<ItemComponent>();

    if (!CanBeBought(ic))
    {
      return;
    }

    int cost = GetCost(ic, false);

    _playerRef->Money -= cost;

    go = _shopOwner->Items[_inventoryItemIndex].release();
    ic = go->GetComponent<ItemComponent>();

    if (_shopOwner->Type() == TraderRole::JUNKER)
    {
      ic->Data.IsIdentified = true;
    }

    ic->Transfer(&_playerRef->Inventory);

    auto it = _shopOwner->Items.begin();
    _shopOwner->Items.erase(it + _inventoryItemIndex);
  }

  CheckSide();
}

void ShoppingState::CheckSide()
{
  if (_playerSide && _playerRef->Inventory.IsEmpty())
  {
    _playerSide = false;
  }
  else if (!_playerSide && _shopOwner->Items.empty())
  {
    _playerSide = true;
  }
}

bool ShoppingState::CanBeBought(ItemComponent *ic)
{
  int cost = GetCost(ic, false);

  if (_playerRef->Money < cost)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail!", { "Not enough money!" }, GlobalConstants::MessageBoxRedBorderColor);
    return false;
  }
  else if (_playerRef->Inventory.IsFull())
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail!", { "No room in inventory!" }, GlobalConstants::MessageBoxRedBorderColor);
    return false;
  }

  return true;
}

int ShoppingState::GetCost(ItemComponent* ic, bool playerSide)
{
  int cost = ic->Data.GetCost();

  for (auto& i : ic->Data.Bonuses)
  {
    cost += i.MoneyCostIncrease;
  }

  if (!playerSide)
  {
    cost *= 2;

    if (_shopOwner->Type() == TraderRole::JUNKER)
    {
      cost = _kJunkerBuyCost;
    }
  }
  else
  {
    float tmp = (float)cost / _kPlayerSellRate;
    cost = (int)tmp;

    if (!ic->Data.IsIdentified
     && _shopOwner->Type() == TraderRole::JUNKER)
    {
      cost *= 2;
    }
  }

  return cost;
}

void ShoppingState::ShowItemInfo()
{
  if (_playerRef->Inventory.IsEmpty()
   && _shopOwner->Items.empty())
  {
    return;
  }

  ItemComponent* ic = nullptr;

  if (_playerSide)
  {
    auto go = _playerRef->Inventory.Contents[_inventoryItemIndex].get();
    ic = go->GetComponent<ItemComponent>();
  }
  else
  {
    auto go = _shopOwner->Items[_inventoryItemIndex].get();
    ic = go->GetComponent<ItemComponent>();
  }

  ic->Inspect();
}
