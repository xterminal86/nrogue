#include "task-mine-block.h"

#include "game-object.h"
#include "position.h"
#include "map.h"
#include "behaviour-tree.h"
#include "equipment-component.h"
#include "item-component.h"

TaskMineBlock::TaskMineBlock(GameObject* objectToControl, bool ignorePickaxe)
  : Node(objectToControl)
{
  _equipment = _objectToControl->GetComponent<EquipmentComponent>();
  _ignorePickaxe = ignorePickaxe;
}

// =============================================================================

BTResult TaskMineBlock::Run()
{
  //DebugLog("[TaskMineBlock]\n");

  EquipmentCategory ec = EquipmentCategory::WEAPON;

  bool equipmentFail = (_equipment == nullptr
                     || _equipment->EquipmentByCategory[ec][0] == nullptr);

  if (!_ignorePickaxe && equipmentFail)
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

  std::vector<Position> candidates;
  for (auto& p : toCheck)
  {
    bool insideMap = Util::IsInsideMap(p,
                                       Map::Instance().CurrentLevel->MapSize);

    auto& so = Map::Instance().CurrentLevel->StaticMapObjects[p.X][p.Y];

    if (insideMap && so != nullptr && so->Type == GameObjectType::PICKAXEABLE)
    {
      candidates.push_back(p);
    }
  }

  if (candidates.empty())
  {
    return BTResult::Failure;
  }

  int ind = RNG::Instance().RandomRange(0, candidates.size());

  Position found = candidates[ind];

  if (!_ignorePickaxe)
  {
    Util::TryToDamageEquipment(_objectToControl, EquipmentCategory::WEAPON, -1);
  }

  MapLevelBase* curLvl = Map::Instance().CurrentLevel;

  curLvl->StaticMapObjects[found.X][found.Y]->Attrs.HP.SetMin(0);
  curLvl->StaticMapObjects[found.X][found.Y]->IsDestroyed = true;

  _objectToControl->FinishTurn();

  return BTResult::Success;
}
