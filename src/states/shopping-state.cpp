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

void ShoppingState::PassShopOwner(TraderComponent* tc)
{
  _shopOwner = tc;

  _shopTitle = tc->ShopTitle;

  tc->NpcRef->Data.IsAquainted = true;
}

void ShoppingState::HandleInput()
{
  _keyPressed = getch();

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

    case 'q':
      auto res = Application::Instance().GetGameStateRefByName(GameStates::NPC_INTERACT_STATE);
      NPCInteractState* nis = static_cast<NPCInteractState*>(res);
      nis->SetNPCRef(_shopOwner->NpcRef);
      Application::Instance().ChangeState(GameStates::NPC_INTERACT_STATE);
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

    for (int x = 0; x < tw; x++)
    {
      Printer::Instance().PrintFB(x, 0, ' ', "#000000", "#FFFFFF");
    }

    for (int y = 1; y < th; y++)
    {
      Printer::Instance().PrintFB(tw / 2, y, ' ', "#000000", "#FFFFFF");
    }

    Printer::Instance().PrintFB(tw / 2, 0, _shopTitle, Printer::kAlignCenter, "#000000", "#FFFFFF");

    DisplayPlayerInventory();
    DisplayShopInventory();

    Printer::Instance().Render();
  }
}

void ShoppingState::DisplayPlayerInventory()
{
  int itemsCount = 0;

  int yPos = 2;
  int index = 0;

  std::string costString;

  int bonusStringMaxLen = 0;
  GetBonusStringMaxLen(bonusStringMaxLen);

  for (auto& item : _playerRef->Inventory.Contents)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();

    std::string nameInInventory = ic->Data.IsIdentified ? item->ObjectName : ic->Data.UnidentifiedName;
    nameInInventory.resize(GlobalConstants::InventoryMaxNameLength, ' ');

    std::string bonusInfo;

    if (ic->Data.IsStackable)
    {
      bonusInfo = Util::StringFormat("(%i)", ic->Data.Amount);
    }
    else if (ic->Data.IsEquipped)
    {
      bonusInfo = Util::StringFormat("E", ic->Data.Amount);
    }

    int cost = ic->Data.GetCost();

    cost = _playerSide ? cost / _kPlayerSellRate : cost;

    if (cost == 0)
    {
      cost = 1;
    }

    costString = Util::StringFormat(" $ %i", cost);

    int bonusStringPosX = GlobalConstants::InventoryMaxNameLength + 1;

    Printer::Instance().PrintFB(bonusStringPosX, yPos + index, bonusInfo, Printer::kAlignLeft, "#FFFFFF");
    Printer::Instance().PrintFB(bonusStringPosX + bonusStringMaxLen, yPos + index, costString, Printer::kAlignLeft, GlobalConstants::CoinsColor);

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

  for (auto& item : _shopOwner->Items)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();

    std::string nameInInventory = ic->Data.IsIdentified ? item->ObjectName : ic->Data.UnidentifiedName;

    std::string tmpName = nameInInventory;
    nameInInventory.insert(0, GlobalConstants::InventoryMaxNameLength - tmpName.length(), ' ');

    nameInInventory.resize(GlobalConstants::InventoryMaxNameLength);

    if (ic->Data.IsStackable)
    {
      auto stackAmount = Util::StringFormat("(%i)", ic->Data.Amount);
      Printer::Instance().PrintFB(xPos - GlobalConstants::InventoryMaxNameLength - 1, yPos + index, stackAmount, Printer::kAlignRight, "#FFFFFF");
    }
    else if (ic->Data.IsEquipped)
    {
      auto equipStatus = Util::StringFormat("E", ic->Data.Amount);
      Printer::Instance().PrintFB(xPos - GlobalConstants::InventoryMaxNameLength - 1, yPos + index, equipStatus, Printer::kAlignRight, "#FFFFFF");
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

void ShoppingState::GetBonusStringMaxLen(int& maxLen)
{
  for (auto& item : _playerRef->Inventory.Contents)
  {
    ItemComponent* ic = item->GetComponent<ItemComponent>();

    std::string bonusInfo;

    if (ic->Data.IsStackable)
    {
      bonusInfo = Util::StringFormat("(%i)", ic->Data.Amount);
    }
    else if (ic->Data.IsEquipped)
    {
      bonusInfo = Util::StringFormat("E", ic->Data.Amount);
    }

    if (bonusInfo.length() > maxLen)
    {
      maxLen = bonusInfo.length();
    }
  }
}
