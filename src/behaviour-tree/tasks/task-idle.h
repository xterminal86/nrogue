#ifndef TASKIDLE_H
#define TASKIDLE_H

#include "behaviour-tree.h"

class TaskIdle : public Node
{
  using Node::Node;

  public:
    bool Run() override
    {
      if (_objectToControl->Attrs.ActionMeter < GlobalConstants::TurnReadyValue)
      {
        _objectToControl->WaitForTurn();
      }
      else
      {
        _objectToControl->FinishTurn();
      }

      return true;
    }
};

#endif // TASKIDLE_H