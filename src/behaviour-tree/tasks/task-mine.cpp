#include "task-mine.h"

#include "game-object.h"
#include "map.h"
#include "blackboard.h"
#include "equipment-component.h"
#include "item-component.h"

TaskMine::TaskMine(GameObject* objectToControl)
  : Node(objectToControl)
{
  _equipment = _objectToControl->GetComponent<EquipmentComponent>();
}

BTResult TaskMine::Run()
{
  //DebugLog("[TaskMine]\n");

  int x = _objectToControl->PosX;
  int y = _objectToControl->PosY;

  int lx = _objectToControl->PosX - 1;
  int ly = _objectToControl->PosY - 1;
  int hx = _objectToControl->PosX + 1;
  int hy = _objectToControl->PosY + 1;

  std::vector<Position> toCheck =
  {
    { lx, y }, { hx, y }, { x, ly }, { x, hy }
  };

  // Searching for any kind of
  //
  // ##
  // .# <-
  // ##
  //
  // Meaning number of walls around actor should be 3
  // and number of walls around the wall to tunnel into
  // should be at least 2.
  //
  int countActor = Map::Instance().CountWallsOrthogonal(x, y);
  if (countActor < 3)
  {
    return BTResult::Failure;
  }

  //
  // Find that empty spot behide the character facing the wall
  // we need to tunnel into
  //
  //      .##
  //    ->.X#
  //      .##
  //
  Position found = { -1, -1 };
  for (auto& p : toCheck)
  {
    if (Map::Instance().CurrentLevel->StaticMapObjects[p.X][p.Y] == nullptr)
    {
      found = p;
      break;
    }
  }

  if (found.X == -1)
  {
    return BTResult::Failure;
  }

  //
  // Wall should be 2 units in the corresponding direction
  //
  if      (found.X > x) found.X -= 2;
  else if (found.X < x) found.X += 2;
  else if (found.Y > y) found.Y -= 2;
  else if (found.Y < y) found.Y += 2;

  auto& so = Map::Instance().CurrentLevel->StaticMapObjects[found.X][found.Y];

  if (!Util::IsInsideMap(found, Map::Instance().CurrentLevel->MapSize) || so == nullptr)
  {
    return BTResult::Failure;
  }

  if (so->Type != GameObjectType::PICKAXEABLE)
  {
    return BTResult::Failure;
  }

  //
  // We can reuse this task for some digging-type monster
  // who don't have any equipment. But if present, check accordingly.
  //
  // NOTE: this task is specialized to digging tunnels, but maybe
  // it's better to create base mining task that tries to dig one block
  // and any other specialized tasks separately?
  //
  if (_equipment != nullptr)
  {
    ItemComponent* pick = _equipment->EquipmentByCategory[EquipmentCategory::WEAPON][0];
    if (pick != nullptr && pick->Data.WeaponType_ == WeaponType::PICKAXE)
    {
      pick->Data.Durability.AddMin(-1);

      if (pick->Data.Durability.Min().Get() < 0)
      {
        pick->Break(_objectToControl);
      }
    }
  }

  Map::Instance().CurrentLevel->StaticMapObjects[found.X][found.Y]->Attrs.HP.SetMin(0);
  Map::Instance().CurrentLevel->StaticMapObjects[found.X][found.Y]->IsDestroyed = true;

  _objectToControl->FinishTurn();

  Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyLastMinedPosX, std::to_string(found.X) });
  Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyLastMinedPosY, std::to_string(found.Y) });

  return BTResult::Success;
}
