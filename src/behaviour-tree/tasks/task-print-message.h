#ifndef TASKPRINTMESSAGE_H
#define TASKPRINTMESSAGE_H

#include "behaviour-tree.h"

class TaskPrintMessage : public Node
{
  public:
    TaskPrintMessage(GameObject* objectToControl,
                     const std::string& debugMessage);

    BTResult Run() override;

  private:
    std::string _message;
};

#endif // TASKPRINTMESSAGE_H
