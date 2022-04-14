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
  //
  // No separate money filter for AI, because I'm lazy
  //
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

  bool succ = true;

  switch (_objectToControl->Type)
  {
    case GameObjectType::MAD_MINER:
      succ = PickupItems(items, { ItemType::GEM, ItemType::RETURNER });
      break;

    default:
      PickupItems(items, Filter());
      break;
  }

  if (!succ)
  {
    return BTResult::Failure;
  }

  _objectToControl->FinishTurn();

  return BTResult::Success;
}

bool TaskTryPickupItems::PickupItems(const Items& items,
                                     const Filter& filter)
{
  if (filter.empty())
  {
    auto topItem = items.back();
    Pickup(topItem);
    return true;
  }

  bool found = false;
  for (auto& i : items)
  {
    ItemComponent* ic = i.second->GetComponent<ItemComponent>();
    if (ic == nullptr)
    {
      DebugLog("[WAR] TaskTryPickupItems::FilterItem() no ItemComponent found on %s!", i.second->ObjectName.data());
      continue;
    }

    auto res = std::find(filter.begin(), filter.end(), ic->Data.ItemType_);
    if (res != filter.end())
    {
      Pickup(i);
      found = true;
      break;
    }
  }

  return found;
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
