#include "equipment-component.h"

#include "container-component.h"

EquipmentComponent::EquipmentComponent()
{
  _componentHash = typeid(*this).hash_code();

  EquipmentByCategory[EquipmentCategory::HEAD]   = { nullptr };
  EquipmentByCategory[EquipmentCategory::NECK]   = { nullptr };
  EquipmentByCategory[EquipmentCategory::TORSO]  = { nullptr };
  EquipmentByCategory[EquipmentCategory::BOOTS]  = { nullptr };
  EquipmentByCategory[EquipmentCategory::WEAPON] = { nullptr };
  EquipmentByCategory[EquipmentCategory::SHIELD] = { nullptr };
  EquipmentByCategory[EquipmentCategory::RING]   = { nullptr, nullptr };

  //
  // Equipment cannot exist without inventory
  //
  ContainerComponent* inventory = OwnerGameObject->GetComponent<ContainerComponent>();
  if (inventory == nullptr)
  {
    inventory = OwnerGameObject->AddComponent<ContainerComponent>();
  }

  _inventory = inventory;
}

void EquipmentComponent::Update()
{
}
