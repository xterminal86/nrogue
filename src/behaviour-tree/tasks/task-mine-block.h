#ifndef TASKMINEBLOCK_H
#define TASKMINEBLOCK_H

#include "behaviour-tree.h"

class EquipmentComponent;

class TaskMineBlock : public Node
{
  public:
    TaskMineBlock(GameObject* objectToControl);

    BTResult Run() override;

  private:
    EquipmentComponent* _equipment = nullptr;
};

#endif // TASKMINEBLOCK_H
