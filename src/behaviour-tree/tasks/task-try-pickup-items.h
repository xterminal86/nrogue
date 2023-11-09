#ifndef TASKTRYPICKUPITEMS_H
#define TASKTRYPICKUPITEMS_H

#include "behaviour-tree.h"

using Item   = std::pair<int, GameObject*>;
using Items  = std::vector<Item>;
using Filter = std::vector<ItemType>;

class ContainerComponent;

class TaskTryPickupItems : public Node
{
  public:
    TaskTryPickupItems(GameObject* objectToControl);

    BTResult Run() override;

  private:
    ContainerComponent* _inventoryRef = nullptr;

    bool PickupItems(const Items& items,
                     const Filter& filter);

    void Pickup(const Item& item);
};


#endif // TASKTRYPICKUPITEMS_H
