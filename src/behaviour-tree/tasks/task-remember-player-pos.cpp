#include "task-remember-player-pos.h"

#include "player.h"
#include "blackboard.h"

BTResult TaskRememberPlayerPos::Run()
{
  //DebugLog("[TaskRememberPlayerPos]\n");

  std::string plX = std::to_string(_playerRef->PosX);
  std::string plY = std::to_string(_playerRef->PosY);

  Blackboard::Instance().Set(_objectToControl->ObjectId(), { GlobalConstants::BlackboardKeyPlayerPosX, plX });
  Blackboard::Instance().Set(_objectToControl->ObjectId(), { GlobalConstants::BlackboardKeyPlayerPosY, plY });

  return BTResult::Success;
}
