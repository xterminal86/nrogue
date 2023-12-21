#include "town-portal-component.h"
#include "map.h"
#include "application.h"
#include "printer.h"

TownPortalComponent::TownPortalComponent()
{
}

// =============================================================================

void TownPortalComponent::Update()
{
  auto& playerRef = Application::Instance().PlayerInstance;
  if (playerRef.PosX == OwnerGameObject->PosX
   && playerRef.PosY == OwnerGameObject->PosY)
  {
    TeleportBack();
  }
}

// =============================================================================

void TownPortalComponent::SavePosition(MapType mapToReturn, const Position& posToReturn)
{
  _posToReturn.first  = mapToReturn;
  _posToReturn.second = posToReturn;
}

// =============================================================================

void TownPortalComponent::TeleportBack()
{
  Printer::Instance().AddMessage("The blue portal disappears behind you!");

  OwnerGameObject->IsDestroyed = true;

  Map::Instance().TeleportToExistingLevel(_posToReturn.first, _posToReturn.second);
}
