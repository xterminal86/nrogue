#ifndef TASKMINEBLOCK_H
#define TASKMINEBLOCK_H

#include "behaviour-tree.h"

class EquipmentComponent;

class TaskMineBlock : public Node
{
  public:
    TaskMineBlock(GameObject* objectToControl, bool ignorePickaxe);

    BTResult Run() override;

  private:
    EquipmentComponent* _equipment = nullptr;

    bool _ignorePickaxe = false;
};

#endif // TASKMINEBLOCK_H
