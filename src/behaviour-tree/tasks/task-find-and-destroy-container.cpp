#include "task-find-and-destroy-container.h"

#include "game-object.h"
#include "container-component.h"
#include "equipment-component.h"
#include "blackboard.h"
#include "map.h"

BTResult TaskFindAndDestroyContainer::Run()
{
  std::string objId = Blackboard::Instance().Get(_objectToControl->ObjectId(), Strings::BlackboardKeyObjectId);
  if (!objId.empty())
  {
    uint64_t objIdInt = std::stoull(objId);
    GameObject* object = Map::Instance().FindGameObjectById(objIdInt);
    if (object == nullptr)
    {
      return BTResult::Failure;
    }

    // TODO: finish

    if (Util::IsObjectInRange(_objectToControl,
                               object,
                               1))
    {
      ItemComponent* weapon = nullptr;
      EquipmentComponent* ec = _objectToControl->GetComponent<EquipmentComponent>();
      if (ec != nullptr)
      {
        weapon = ec->EquipmentByCategory[EquipmentCategory::WEAPON][0];
      }

      int dmg = Util::CalculateDamageValue(_objectToControl,
                                             object,
                                             weapon,
                                             false);

      Util::TryToDamageEquipment(_objectToControl, weapon, -1);

      object->ReceiveDamage(_objectToControl, dmg, false, false, true);
    }
  }

  _objectToControl->FinishTurn();

  return BTResult::Success;
}

GameObject* TaskFindAndDestroyContainer::FindContainer()
{
  GameObject* res = nullptr;

  auto& collection = Map::Instance().CurrentLevel->GameObjects;
  int r = _objectToControl->VisibilityRadius.Get();

  auto items = Util::FindObjectsInRange(_objectToControl,
                                          collection,
                                          r);

  std::vector<int> itemsIndices;
  for (size_t i = 0; i < items.size(); i++)
  {
    ContainerComponent* cc = items[i]->GetComponent<ContainerComponent>();
    if (cc != nullptr)
    {
      itemsIndices.push_back(i);
    }
  }

  if (!itemsIndices.empty())
  {
    int index = RNG::Instance().RandomRange(0, itemsIndices.size());
    res = items[index];
  }

  return res;
}
