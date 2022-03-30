#ifndef TASKMINE_H
#define TASKMINE_H

#include "behaviour-tree.h"

class EquipmentComponent;

class TaskMine : public Node
{
  public:
    TaskMine(GameObject* objectToControl);

    BTResult Run() override;

  private:
    EquipmentComponent* _equipment = nullptr;
};

#endif // TASKMINE_H
