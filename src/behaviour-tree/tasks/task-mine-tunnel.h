#ifndef TASKMINETUNNEL_H
#define TASKMINETUNNEL_H

#include "behaviour-tree.h"

class EquipmentComponent;

class TaskMineTunnel : public Node
{
  public:
    TaskMineTunnel(GameObject* objectToControl, bool ignorePickaxe);

    BTResult Run() override;

  private:
    EquipmentComponent* _equipment = nullptr;

    bool _ignorePickaxe = false;
};

#endif // TASKMINETUNNEL_H
