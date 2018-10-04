#include "ai-dummy.h"

#include "game-object.h"

void AIDummy::Update()
{
  int dx = _rng() % 2;
  int dy = _rng() % 2;

  int signX = (_rng() % 2) == 0 ? -1 : 1;
  int signY = (_rng() % 2) == 0 ? -1 : 1;

  dx *= signX;
  dy *= signY;
  
  // See component.h  
  ((GameObject*)Owner)->Move(dx, dy);
}
