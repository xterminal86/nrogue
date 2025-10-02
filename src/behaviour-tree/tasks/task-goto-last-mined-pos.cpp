#include "task-goto-last-mined-pos.h"

#include "blackboard.h"
#include "game-object.h"

BTResult TaskGotoLastMinedPos::Run()
{
  //DebugLog("[TaskGotoLastMinedPos]\n");

  auto minedPos = Game::gBB.Get(_objectToControl->ObjectId(),
                                Strings::BlackboardKeyLastMinedPos);

  if (minedPos.empty())
  {
    return BTResult::Failure;
  }

  auto split = Util::StringSplit(minedPos, ',');

  int mX = std::stoi(split[0]);
  int mY = std::stoi(split[1]);

  if (_objectToControl->PosX == mX
   && _objectToControl->PosY == mY)
  {
    Game::gBB.Set(_objectToControl->ObjectId(),
                  {
                    Strings::BlackboardKeyLastMinedPos,
                    std::string()
                  });

    return BTResult::Failure;
  }

  _objectToControl->MoveTo({ mX, mY });
  _objectToControl->FinishTurn();

  Game::gBB.Set(_objectToControl->ObjectId(),
                {
                  Strings::BlackboardKeyLastMinedPos,
                  std::string()
                });

  return BTResult::Success;
}
