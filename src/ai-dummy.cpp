#include "ai-dummy.h"

#include "game-object.h"
#include "rng.h"

void AIDummy::Update()
{
  int dx = RNG::Instance().Random() % 2;
  int dy = RNG::Instance().Random() % 2;

  int signX = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;
  int signY = (RNG::Instance().Random() % 2) == 0 ? -1 : 1;

  dx *= signX;
  dy *= signY;
  
  // See component.h  
  ((GameObject*)OwnerGameObject)->Move(dx, dy);
}
