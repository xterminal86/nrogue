#include "town-portal-component.h"
#include "map.h"
#include "application.h"

TownPortalComponent::TownPortalComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void TownPortalComponent::Update()
{
  auto& playerRef = Application::Instance().PlayerInstance;
  if (playerRef.PosX == OwnerGameObject->PosX
   && playerRef.PosY == OwnerGameObject->PosY)
  {
    TeleportBack();
  }
}

void TownPortalComponent::SavePosition(MapType mapToReturn, const Position& posToReturn)
{
  _posToReturn.first = mapToReturn;
  _posToReturn.second = posToReturn;
}

void TownPortalComponent::TeleportBack()
{
  OwnerGameObject->IsDestroyed = true;
  Map::Instance().RemoveDestroyed();
  Map::Instance().TeleportToExistingLevel(_posToReturn.first, _posToReturn.second);
}
