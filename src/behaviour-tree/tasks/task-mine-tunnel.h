#ifndef TASKMINETUNNEL_H
#define TASKMINETUNNEL_H

#include "behaviour-tree.h"

class EquipmentComponent;

class TaskMineTunnel : public Node
{
  public:
    TaskMineTunnel(GameObject* objectToControl);

    BTResult Run() override;

  private:
    EquipmentComponent* _equipment = nullptr;
};

#endif // TASKMINETUNNEL_H
