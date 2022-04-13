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

  //
  // TODO: should preferred items for picking
  // be driven by GameObjectType of _objectToControl?
  //
  auto res = Map::Instance().GetGameObjectToPickup(_objectToControl->PosX, _objectToControl->PosY);
  if (res.first == -1)
  {
    return BTResult::Failure;
  }

  //
  // Picking up everything for now
  //
  auto go = Map::Instance().CurrentLevel->GameObjects[res.first].release();

  _inventoryRef->Add(go);

  auto it = Map::Instance().CurrentLevel->GameObjects.begin();
  Map::Instance().CurrentLevel->GameObjects.erase(it + res.first);

  _objectToControl->FinishTurn();

  return BTResult::Success;
}
