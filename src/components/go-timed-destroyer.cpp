#include "go-timed-destroyer.h"

#include "game-object.h"
#include "map.h"

TimedDestroyerComponent::TimedDestroyerComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void TimedDestroyerComponent::Update()
{
  Time--;

  if (Time <= 0)
  {
    OwnerGameObject->IsDestroyed = true;
  }

  // FIXME: doesn't show, check RemoveDestroyed() calls
  //
  //
  // Show remains on dangerous tile
  // for 1 update for visual cue in case of knock back to lava.
  //
  bool danger = Map::Instance().IsTileDangerous({ OwnerGameObject->PosX, OwnerGameObject->PosY });
  if (danger)
  {
    if (!_dangerFlag)
    {
      _dangerFlag = true;
    }
    else
    {
      OwnerGameObject->IsDestroyed = true;
    }
  }
}
