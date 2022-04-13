#ifndef TASKTRYPICKUPITEMS_H
#define TASKTRYPICKUPITEMS_H

#include "behaviour-tree.h"

class ContainerComponent;

class TaskTryPickupItems : public Node
{
  public:
    TaskTryPickupItems(GameObject* objectToControl);

    BTResult Run() override;

  private:
    ContainerComponent* _inventoryRef;
};


#endif // TASKTRYPICKUPITEMS_H
