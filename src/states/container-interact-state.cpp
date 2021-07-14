#include "container-interact-state.h"
#include "printer.h"
#include "application.h"
#include "player.h"
#include "util.h"

void ContainerInteractState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;    
}

void ContainerInteractState::Cleanup()
{
  _containerToInteractWith = nullptr;
}

void ContainerInteractState::Prepare()
{
  _inventoryItemIndex = 0;
  _playerSide = (!_playerRef->Inventory.IsEmpty());
}

void ContainerInteractState::SetContainerRef(ContainerComponent* c)
{
  _containerToInteractWith = c;
}

void ContainerInteractState::HandleInput()
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
      if (!_containerToInteractWith->IsEmpty())
      {
        _playerSide = false;
      }
    }
    break;

    case VK_ENTER:
      TryToTransferItem();
      break;

    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
      break;
  }

  CheckIndexLimits();
}

void ContainerInteractState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    for (size_t y = 0; y < Printer::TerminalHeight; y++)
    {      
      Printer::Instance().PrintFB(_twHalf, y, ' ', "#000000", "#FFFFFF");
    }

    auto containerName = _containerToInteractWith->OwnerGameObject->ObjectName;

    Printer::Instance().PrintFB(_twQuarter, 0, "Player", Printer::kAlignCenter, "#FFFFFF");
    Printer::Instance().PrintFB(Printer::TerminalWidth - _twQuarter - 1, 0, containerName, Printer::kAlignCenter, "#FFFFFF");

    Printer::Instance().PrintFB(1, Printer::TerminalHeight - 1, "'Enter' - exchange", Printer::kAlignLeft, "#FFFFFF");

    DisplayPlayerInventory();
    DisplayContainerInventory();

    Printer::Instance().Render();
  }
}

void ContainerInteractState::DisplayPlayerInventory()
{
  int itemsCount = 0;

  int yPos = 2;
  int index = 0;

  for (auto& item : _playerRef->Inventory.Contents)
  {
    auto c = item->GetComponent<ItemComponent>();
    ItemComponent* ic = static_cast<ItemComponent*>(c);

    std::string nameInInventory = ic->Data.IsIdentified ? item->ObjectName : ic->Data.UnidentifiedName;
    nameInInventory.resize(GlobalConstants::InventoryMaxNameLength, ' ');

    if (ic->Data.IsStackable)
    {
      auto stackAmount = Util::StringFormat("(%i)", ic->Data.Amount);
      Printer::Instance().PrintFB(GlobalConstants::InventoryMaxNameLength + 1, yPos + index, stackAmount, Printer::kAlignLeft, "#FFFFFF");
    }
    else if (ic->Data.IsEquipped)
    {
      auto equipStatus = Util::StringFormat("E", ic->Data.Amount);
      Printer::Instance().PrintFB(GlobalConstants::InventoryMaxNameLength + 1, yPos + index, equipStatus, Printer::kAlignLeft, "#FFFFFF");
    }

    std::string textColor = Util::GetItemInventoryColor(ic->Data);

    if (_playerSide && index == _inventoryItemIndex)
    {
      Printer::Instance().PrintFB(1, yPos + index, nameInInventory, Printer::kAlignLeft, "#000000", "#FFFFFF");
    }
    else
    {      
      Printer::Instance().PrintFB(1, yPos + index, nameInInventory, Printer::kAlignLeft, textColor);
    }

    index++;

    itemsCount++;
  }

  for (size_t i = itemsCount; i < GlobalConstants::InventoryMaxNameLength; i++)
  {
    std::string stub(GlobalConstants::InventoryMaxNameLength, '.');
    Printer::Instance().PrintFB(1,
                                yPos + index,
                                stub,
                                Printer::kAlignLeft,
                                GlobalConstants::InventoryEmptySlotColor);
    yPos++;
  }
}

void ContainerInteractState::DisplayContainerInventory()
{
  int itemsCount = 0;

  int yPos = 2;
  int index = 0;

  int tw = Printer::TerminalWidth;

  int xPos = tw - 1;

  for (auto& item : _containerToInteractWith->Contents)
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

    std::string textColor = Util::GetItemInventoryColor(ic->Data);

    if (!_playerSide && index == _inventoryItemIndex)
    {
      Printer::Instance().PrintFB(xPos, yPos + index, nameInInventory, Printer::kAlignRight, "#000000", "#FFFFFF");
    }
    else
    {
      Printer::Instance().PrintFB(xPos, yPos + index, nameInInventory, Printer::kAlignRight, textColor);
    }

    index++;

    itemsCount++;
  }

  for (size_t i = itemsCount; i < GlobalConstants::InventoryMaxNameLength; i++)
  {
    std::string stub(GlobalConstants::InventoryMaxNameLength, '.');
    Printer::Instance().PrintFB(xPos,
                                yPos + index,
                                stub,
                                Printer::kAlignRight,
                                GlobalConstants::InventoryEmptySlotColor);
    yPos++;
  }
}


void ContainerInteractState::CheckIndexLimits()
{
  int invSize = 0;

  if (_playerSide)
  {    
    invSize = _playerRef->Inventory.Contents.size() - 1;
  }
  else
  {
    if (_containerToInteractWith != nullptr)
    {
      invSize = _containerToInteractWith->Contents.size() - 1;
    }
  }

  _inventoryItemIndex = Util::Clamp(_inventoryItemIndex, 0, invSize);
}

void ContainerInteractState::TryToTransferItem()
{
  ContainerComponent* src = _playerSide ? &_playerRef->Inventory : _containerToInteractWith;
  ContainerComponent* dst = _playerSide ? _containerToInteractWith : &_playerRef->Inventory;

  if (dst->IsFull())
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Epic Fail!", { "No room in inventory!" }, GlobalConstants::MessageBoxRedBorderColor);
    return;
  }

  GameObject* go = src->Contents[_inventoryItemIndex].get();
  ItemComponent* ic = go->GetComponent<ItemComponent>();

  if (ic->Data.IsEquipped)
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, "Information", { "Unqeuip first!" }, GlobalConstants::MessageBoxRedBorderColor);
    return;
  }

  go = src->Contents[_inventoryItemIndex].release();
  ic = go->GetComponent<ItemComponent>();

  ic->Transfer(dst);

  auto it = src->Contents.begin();
  src->Contents.erase(it + _inventoryItemIndex);

  if (_playerSide && _playerRef->Inventory.IsEmpty())
  {
    _playerSide = false;
  }
  else if (!_playerSide && _containerToInteractWith->IsEmpty())
  {
    _playerSide = true;
  }
}
