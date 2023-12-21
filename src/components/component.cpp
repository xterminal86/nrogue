#include "component.h"

Component::Component()
{
}

// =============================================================================

void Component::Prepare(GameObject* owner)
{
  OwnerGameObject = owner;

  PrepareAdditional();
}

// =============================================================================

void Component::PrepareAdditional()
{
}
