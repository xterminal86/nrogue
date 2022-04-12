#include "task-remember-player-pos.h"

#include "player.h"
#include "blackboard.h"

BTResult TaskRememberPlayerPos::Run()
{
  //DebugLog("[TaskRememberPlayerPos]\n");

  std::string plX = std::to_string(_playerRef->PosX);
  std::string plY = std::to_string(_playerRef->PosY);

  auto plPos = Util::StringFormat("%i,%i", _playerRef->PosX, _playerRef->PosY);

  Blackboard::Instance().Set(_objectToControl->ObjectId(), { Strings::BlackboardKeyPlayerPos, plPos });

  return BTResult::Success;
}
