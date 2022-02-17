#include "equipment-component.h"

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
}

void EquipmentComponent::Update()
{
}
