#include "task-mine-tunnel.h"

#include "game-object.h"
#include "map.h"
#include "blackboard.h"
#include "equipment-component.h"
#include "item-component.h"

TaskMineTunnel::TaskMineTunnel(GameObject* objectToControl, bool ignorePickaxe)
  : Node(objectToControl)
{
  _equipment = _objectToControl->GetComponent<EquipmentComponent>();
  _ignorePickaxe = ignorePickaxe;
}

BTResult TaskMineTunnel::Run()
{
  //DebugLog("[TaskMine]\n");

  bool equipFail = (_equipment == nullptr
                 || _equipment->EquipmentByCategory[EquipmentCategory::WEAPON][0] == nullptr);

  if (!_ignorePickaxe && equipFail)
  {
    return BTResult::Failure;
  }

  int x = _objectToControl->PosX;
  int y = _objectToControl->PosY;

  int lx = x - 1;
  int ly = y - 1;
  int hx = x + 1;
  int hy = y + 1;

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
  // Find that empty spot behind the character facing the wall
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

  if (!_ignorePickaxe)
  {
    Util::TryToDamageEquipment(_objectToControl, EquipmentCategory::WEAPON, -1);
  }

  Map::Instance().CurrentLevel->StaticMapObjects[found.X][found.Y]->Attrs.HP.SetMin(0);
  Map::Instance().CurrentLevel->StaticMapObjects[found.X][found.Y]->IsDestroyed = true;

  _objectToControl->FinishTurn();

  auto minedPos = Util::StringFormat("%i,%i", found.X, found.Y);

  Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyLastMinedPos, minedPos });

  return BTResult::Success;
}
