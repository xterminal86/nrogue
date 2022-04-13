#include "task-try-pickup-items.h"

#include "container-component.h"
#include "item-component.h"
#include "map.h"

TaskTryPickupItems::TaskTryPickupItems(GameObject* objectToControl)
  : Node(objectToControl)
{
  _inventoryRef = _objectToControl->GetComponent<ContainerComponent>();
}

BTResult TaskTryPickupItems::Run()
{
  if (_inventoryRef->IsFull())
  {
    return BTResult::Failure;
  }

  auto items = Map::Instance().GetGameObjectsToPickup(_objectToControl->PosX,
                                                      _objectToControl->PosY);

  if (items.empty())
  {
    return BTResult::Failure;
  }

  auto topItem = items.back();

  bool succ = true;

  switch (_objectToControl->Type)
  {
    case GameObjectType::MAD_MINER:
      succ = FilterItem(topItem, { ItemType::GEM, ItemType::RETURNER });
      break;

    default:
      Pickup(topItem);
      break;
  }

  if (!succ)
  {
    return BTResult::Failure;
  }

  _objectToControl->FinishTurn();

  return BTResult::Success;
}

bool TaskTryPickupItems::FilterItem(const Item& item,
                                    const Filter& filter)
{
  bool succ = false;

  ItemComponent* ic = item.second->GetComponent<ItemComponent>();
  if (ic == nullptr)
  {
    DebugLog("[WAR] TaskTryPickupItems::FilterItem() no ItemComponent found on %s!", item.second->ObjectName.data());
    return succ;
  }

  auto found = std::find(filter.begin(), filter.end(), ic->Data.ItemType_);
  if (found != filter.end())
  {
    Pickup(item);
    succ = true;
  }

  return succ;
}

void TaskTryPickupItems::Pickup(const Item& item)
{
  auto go = Map::Instance().CurrentLevel->GameObjects[item.first].get();
  ItemComponent* ic = go->GetComponent<ItemComponent>();
  if (ic->Data.ItemType_ == ItemType::COINS)
  {
    _objectToControl->Money += ic->Data.Amount;
  }
  else
  {
    go = Map::Instance().CurrentLevel->GameObjects[item.first].release();
    _inventoryRef->Add(go);
  }

  auto it = Map::Instance().CurrentLevel->GameObjects.begin();
  Map::Instance().CurrentLevel->GameObjects.erase(it + item.first);
}
