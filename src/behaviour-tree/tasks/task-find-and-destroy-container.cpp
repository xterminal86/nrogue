#include "task-find-and-destroy-container.h"

#include "game-object.h"
#include "container-component.h"
#include "equipment-component.h"
#include "blackboard.h"
#include "pathfinder.h"
#include "map.h"

BTResult TaskFindAndDestroyContainer::Run()
{
  BTResult res = BTResult::Failure;

  std::string objId = Blackboard::Instance().Get(_objectToControl->ObjectId(), Strings::BlackboardKeyObjectId);
  if (!objId.empty())
  {
    uint64_t objIdInt = std::stoull(objId);
    //
    // If our saved object no longer exists, erase it from blackboard
    // so that new container could be found on next iteration
    //
    GameObject* object = Map::Instance().FindGameObjectById(objIdInt, GameObjectCollectionType::STATIC_OBJECTS);
    if (object == nullptr)
    {
      Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyObjectId, std::string() });
    }
    else
    {
      res = ProcessExistingObject(object);
    }
  }
  else
  {
    GameObject* container = FindContainer();
    if (container != nullptr)
    {
      Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyObjectId, std::to_string(container->ObjectId()) });
      res = ProcessExistingObject(container);
    }
  }

  if (res == BTResult::Success)
  {
    _objectToControl->FinishTurn();
  }

  return res;
}

// =============================================================================

GameObject* TaskFindAndDestroyContainer::FindContainer()
{
  GameObject* res = nullptr;

  int r = _objectToControl->VisibilityRadius.Get();

  Position objPos = _objectToControl->GetPosition();

  auto containersFound = Util::GetContainersInRange(_objectToControl, r);
  if (!containersFound.empty())
  {
    std::vector<int> indices;
    for (size_t i = 0; i < containersFound.size(); i++)
    {
      if (Map::Instance().IsObjectVisible(objPos,
                                          containersFound[i]->GetPosition(),
                                          true))
      {
        indices.push_back(i);
      }
    }

    if (!indices.empty())
    {
      int index = RNG::Instance().RandomRange(0, indices.size());
      res = containersFound[index];
    }
  }

  return res;
}

// =============================================================================

BTResult TaskFindAndDestroyContainer::ProcessExistingObject(GameObject* container)
{
  if (Util::IsObjectInRange(_objectToControl,
                            container,
                            1))
  {
    ItemComponent* weapon = nullptr;
    EquipmentComponent* ec = _objectToControl->GetComponent<EquipmentComponent>();
    if (ec != nullptr)
    {
      weapon = ec->EquipmentByCategory[EquipmentCategory::WEAPON][0];
    }

    int dmg = Util::CalculateDamageValue(_objectToControl,
                                         container,
                                         weapon,
                                         false);

    Util::TryToDamageEquipment(_objectToControl, weapon, -1);

    container->ReceiveDamage(_objectToControl, dmg, false, false, false, true);
  }
  else
  {
    Pathfinder pf;

    auto path = pf.BuildRoad(Map::Instance().CurrentLevel,
                             _objectToControl->GetPosition(),
                             container->GetPosition(),
                             std::vector<char>(),
                             false,
                             true);

    if (path.empty())
    {
      return BTResult::Failure;
    }

    auto moveTo = path.top();
    if (!_objectToControl->MoveTo(moveTo))
    {
      return BTResult::Failure;
    }
  }

  return BTResult::Success;
}
