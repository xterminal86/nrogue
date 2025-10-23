#include "town-portal-component.h"
#include "map.h"
#include "application.h"
#include "printer.h"

// =============================================================================

void TownPortalComponent::Update()
{
  auto& playerRef = Game::gApp.PlayerInstance;
  if (playerRef.PosX == OwnerGameObject->PosX
   && playerRef.PosY == OwnerGameObject->PosY)
  {
    TeleportBack();
  }
}

// =============================================================================

void TownPortalComponent::SavePosition(MapType mapToReturn,
                                       const Position& posToReturn)
{
  _posToReturn.first  = mapToReturn;
  _posToReturn.second = posToReturn;
}

// =============================================================================

void TownPortalComponent::TeleportBack()
{
  Game::gPrnt.AddMessage("The blue portal disappears behind you!");

  OwnerGameObject->Destroy();

  Game::gMap.TeleportToExistingLevel(_posToReturn.first,
                                      _posToReturn.second);
}
