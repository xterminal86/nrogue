#include "pickup-item-state.h"

#include "application.h"
#include "printer.h"
#include "map.h"

void PickupItemState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;

  _headerText = " PICKUP ITEMS ";
}

void PickupItemState::Prepare()
{
  RebuildDisplayList();
}

void PickupItemState::Setup(const Items& items)
{
  _itemsList = items;
}

void PickupItemState::ProcessInput()
{
  switch (_keyPressed)
  {
    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MAIN_STATE);
      break;

    default:
    {
      if (!_itemsListIndexByChar.empty())
      {
        if (_itemsListIndexByChar.count(_keyPressed))
        {
          int index  = _itemsListIndexByChar[_keyPressed];
          auto& item = _itemsList[index];

          bool ok = PickupItem(item);
          if (ok)
          {
            //
            // NOTE: may be possible items shuffle
            // due to reaquiring list of items
            // in the pile in case of a big pile and
            // separate pickup from it.
            //
            _itemsList = Map::Instance().GetGameObjectsToPickup(_playerRef->PosX, _playerRef->PosY);
            RebuildDisplayList();
          }
        }
      }
    }
    break;
  }
}

bool PickupItemState::PickupItem(const Item& item)
{
  ItemComponent* ic = item.second->GetComponent<ItemComponent>();
  if (ic->Data.ItemType_ == ItemType::COINS)
  {
    auto message = Util::StringFormat(Strings::FmtPickedUpIS, ic->Data.Amount, ic->OwnerGameObject->ObjectName.data());
    Printer::Instance().AddMessage(message);

    _playerRef->Money += ic->Data.Amount;

    auto it = Map::Instance().CurrentLevel->GameObjects.begin();
    Map::Instance().CurrentLevel->GameObjects.erase(it + item.first);

    return true;
  }
  else
  {
    if (_playerRef->Inventory->IsFull())
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                             Strings::MessageBoxEpicFailHeaderText,
                                             { Strings::MsgInventoryFull },
                                             Colors::MessageBoxRedBorderColor);

      return false;
    }

    auto go = Map::Instance().CurrentLevel->GameObjects[item.first].release();

    _playerRef->Inventory->Add(go);

    std::string objName = ic->Data.IsIdentified
                        ? go->ObjectName
                        : ic->Data.UnidentifiedName;

    std::string message;
    if (ic->Data.IsStackable)
    {
      message = Util::StringFormat(Strings::FmtPickedUpIS,
                                   ic->Data.Amount,
                                   objName.data());
    }
    else
    {
      message = Util::StringFormat(Strings::FmtPickedUpS, objName.data());
    }

    Printer::Instance().AddMessage(message);

    auto it = Map::Instance().CurrentLevel->GameObjects.begin();
    Map::Instance().CurrentLevel->GameObjects.erase(it + item.first);

    return true;
  }

  return false;
}

void PickupItemState::DrawSpecific()
{
  if (_displayLines.empty())
  {
    Printer::Instance().PrintFB(_twHalf,
                                _thHalf,
                                "No items",
                                Printer::kAlignCenter,
                                Colors::WhiteColor);
  }
  else
  {
    int lineIndex = 0;
    for (auto& line : _displayLines)
    {
      Printer::Instance().PrintFB(1,
                                  2 + lineIndex,
                                  line,
                                  Printer::kAlignLeft,
                                  Colors::WhiteColor);
      lineIndex++;
    }
  }
}

void PickupItemState::RebuildDisplayList()
{
  _itemsListIndexByChar.clear();
  _displayLines.clear();

  int lineIndex = 0;
  for (auto& i : _itemsList)
  {
    ItemComponent* ic = i.second->GetComponent<ItemComponent>();

    std::string objName = ic->Data.IsIdentified
                        ? i.second->ObjectName
                        : ic->Data.UnidentifiedName;

    char c = Strings::AlphabetLowercase[lineIndex];

    std::string str = Util::StringFormat("'%c' - %s", c, objName.data());
    _displayLines.push_back(str);

    _itemsListIndexByChar[c] = lineIndex;
    lineIndex++;
  }
}
