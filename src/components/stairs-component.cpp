#include "stairs-component.h"

StairsComponent::StairsComponent()
{
}

// =============================================================================

void StairsComponent::Update()
{
  //
  // Stairs are part of MapArray, i.e. map floor tiles,
  // so they are not participating in global Update().
  //
}
