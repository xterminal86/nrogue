#ifndef TASKDEBUG_H
#define TASKDEBUG_H

#include "behaviour-tree.h"

class TaskDebug : public Node
{
  public:
    TaskDebug(GameObject* objectToControl, const std::string& debugMessage);

    BTResult Run() override;

  private:
    std::string _message;
};

#endif // TASKDEBUG_H
