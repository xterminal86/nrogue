#ifndef TASKFINDANDDESTROYCONTAINER_H
#define TASKFINDANDDESTROYCONTAINER_H

#include "behaviour-tree.h"

class TaskFindAndDestroyContainer : public Node
{
  using Node::Node;

  public:
    BTResult Run() override;

  private:
    GameObject* FindContainer();

    BTResult ProcessExistingObject(GameObject* container);
};

#endif // TASKFINDANDDESTROYCONTAINER_H
