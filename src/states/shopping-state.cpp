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
    case NUMPAD_2:
      _inventoryItemIndex++;
      break;

    case NUMPAD_8:
      _inventoryItemIndex--;
      break;

    case NUMPAD_4:
    {
      if (!_playerRef->Inventory.IsEmpty())
      {
        _playerSide = true;
      }
    }
    break;

    case NUMPAD_6:
    {
      if (_shopOwner->Items.size() > 0)
      {
        _playerSide = false;
      }
    }
    break;

    case VK_ENTER:
      BuyOrSellItem();
      break;

    case 'q':
    {
      auto res = Application::Instance().GetGameStateRefByName(GameStates::NPC_INTERACT_STATE);
      NPCInteractState* nis = static_cast<NPCInteractState*>(res);
      nis->SetNPCRef(_shopOwner->NpcRef);
      Application::Instance().ChangeState(GameStates::NPC_INTERACT_STATE);
    }
    break;

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

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    DrawHeader(_shopTitle);

    for (int y = 1; y < th; y++)
    {
      #ifdef USE_SDL
      Printer::Instance().PrintFB(tw / 2, y, (int)NameCP437::VBAR_2, "#FFFFFF");
      #else
      Printer::Instance().PrintFB(tw / 2, y, '|', "#000000", "#FFFFFF");
      #endif
    }

    DisplayPlayerInventory();
    DisplayShopInventory();

    auto playerMoney = Util::StringFormat("You have: %i $", _playerRef->Money);
    Printer::Instance().PrintFB(0, th - 1, playerMoney, Printer::kAlignLeft, GlobalConstants::CoinsColor);

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

    Printer::Instance().PrintFB(extraInfoStringPosX, yPos + index, extraInfo, Printer::kAlignLeft, "#FFFFFF");

    int cost = GetCost(ic, true);

    costString = Util::StringFormat(" $ %i", cost);

    Printer::Instance().PrintFB(extraInfoStringPosX + itemStringTotalLen, yPos + index, costString, Printer::kAlignLeft, GlobalConstants::CoinsColor);

    std::string textColor = GetItemTextColor(ic);

    std::string idColor = (ic->Data.IsIdentified || ic->Data.IsPrefixDiscovered) ? textColor : "#FFFFFF";

    if (_playerSide && index == _inventoryItemIndex)
    {
      Printer::Instance().PrintFB(0, yPos + index, nameInInventory, Printer::kAlignLeft, "#000000", "#FFFFFF");
    }
    else
    {
      Printer::Instance().PrintFB(0, yPos + index, nameInInventory, Printer::kAlignLeft, idColor);
    }

    index++;

    itemsCount++;
  }

  for (int i = itemsCount; i < GlobalConstants::InventoryMaxNameLength; i++)
  {
    std::string stub(GlobalConstants::InventoryMaxNameLength, '-');
    Printer::Instance().PrintFB(0, yPos + index, stub, Printer::kAlignLeft, "#FFFFFF");
    yPos++;
  }
}

void ShoppingState::DisplayShopInventory()
{
  int itemsCount = 0;

  int yPos = 2;
  int index = 0;

  int tw = Printer::Instance().TerminalWidth;

  int xPos = tw - 1;

  int itemStringTotalLen = GetItemStringTotalLen(_shopOwner->Items);

  for (auto& item : _shopOwner->Items)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();

    std::string nameInInventory = ic->Data.IsIdentified ? item->ObjectName : ic->Data.UnidentifiedName;

    std::string tmpName = nameInInventory;

    nameInInventory.insert(0, GlobalConstants::InventoryMaxNameLength - tmpName.length(), ' ');
    nameInInventory.resize(GlobalConstants::InventoryMaxNameLength);

    std::string extraInfo = GetItemExtraInfo(ic);

    Printer::Instance().PrintFB(xPos - GlobalConstants::InventoryMaxNameLength - 1, yPos + index, extraInfo, Printer::kAlignRight, "#FFFFFF");

    int cost = GetCost(ic, false);

    std::string costString = Util::StringFormat("%i $ ", cost);

    int bonusStringPosX = xPos - GlobalConstants::InventoryMaxNameLength - 1;

    Printer::Instance().PrintFB(bonusStringPosX - itemStringTotalLen, yPos + index, costString, Printer::kAlignRight, GlobalConstants::CoinsColor);

    std::string textColor = GetItemTextColor(ic);

    std::string idColor = (ic->Data.IsIdentified || ic->Data.IsPrefixDiscovered) ? textColor : "#FFFFFF";

    if (!_playerSide && index == _inventoryItemIndex)
    {
      Printer::Instance().PrintFB(xPos, yPos + index, nameInInventory, Printer::kAlignRight, "#000000", "#FFFFFF");
    }
    else
    {
      Printer::Instance().PrintFB(xPos, yPos + index, nameInInventory, Printer::kAlignRight, idColor);
    }

    index++;

    itemsCount++;
  }

  for (int i = itemsCount; i < GlobalConstants::InventoryMaxNameLength; i++)
  {
    std::string stub(GlobalConstants::InventoryMaxNameLength, '-');
    Printer::Instance().PrintFB(xPos, yPos + index, stub, Printer::kAlignRight, "#FFFFFF");
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
    if (_shopOwner->Items.size() > 0)
    {
      invSize = _shopOwner->Items.size() - 1;
    }
  }

  _inventoryItemIndex = Util::Clamp(_inventoryItemIndex, 0, invSize);
}

int ShoppingState::GetItemStringTotalLen(std::vector<std::unique_ptr<GameObject>>& container)
{
  int maxLen = 0;

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

std::string ShoppingState::GetItemTextColor(ItemComponent* item)
{
  std::string textColor = "#FFFFFF";

  if (item->Data.Prefix == ItemPrefix::BLESSED)
  {
    textColor = GlobalConstants::ItemMagicColor;
  }
  else if (item->Data.Prefix == ItemPrefix::CURSED)
  {
    textColor = "#880000";
  }

  return textColor;
}

void ShoppingState::BuyOrSellItem()
{
  if (_playerRef->Inventory.IsEmpty()
   && _shopOwner->Items.size() == 0)
  {
    return;
  }

  if (_playerSide)
  {
    auto go = _playerRef->Inventory.Contents[_inventoryItemIndex].get();
    ItemComponent* ic = go->GetComponent<ItemComponent>();

    if (ic->Data.IsEquipped)
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "Unequip first!" }, GlobalConstants::MessageBoxRedBorderColor);
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
  else if (!_playerSide && _shopOwner->Items.size() == 0)
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

  if (!playerSide)
  {
    cost *= 2;

    if (_shopOwner->Type() == TraderRole::JUNKER)
    {
      cost = 50;
    }
  }
  else
  {
    cost /= _kPlayerSellRate;

    if (!ic->Data.IsIdentified
     && _shopOwner->Type() == TraderRole::JUNKER)
    {
      cost *= 2;
    }
  }

  return cost;
}
