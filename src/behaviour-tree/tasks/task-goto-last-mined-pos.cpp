#include "task-goto-last-mined-pos.h"

#include "blackboard.h"
#include "game-object.h"

BTResult TaskGotoLastMinedPos::Run()
{
  //DebugLog("[TaskGotoLastMinedPos]\n");

  auto sX = Blackboard::Instance().Get(_objectToControl->ObjectId(), Strings::BlackboardKeyLastMinedPosX);
  auto sY = Blackboard::Instance().Get(_objectToControl->ObjectId(), Strings::BlackboardKeyLastMinedPosY);

  if (sX.empty() || sY.empty())
  {
    return BTResult::Failure;
  }

  int mX = std::stoi(sX);
  int mY = std::stoi(sY);

  if (_objectToControl->PosX == mX
   && _objectToControl->PosY == mY)
  {
    Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyLastMinedPosX, "" });
    Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyLastMinedPosY, "" });

    return BTResult::Failure;
  }

  _objectToControl->MoveTo({ mX, mY });
  _objectToControl->FinishTurn();

  Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyLastMinedPosX, "" });
  Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyLastMinedPosY, "" });

  return BTResult::Success;
}
