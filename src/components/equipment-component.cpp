#include "equipment-component.h"

#include "container-component.h"
#include "application.h"
#include "printer.h"

EquipmentComponent::EquipmentComponent(ContainerComponent* inventoryRef)
{
  EquipmentByCategory[EquipmentCategory::HEAD]   = { nullptr };
  EquipmentByCategory[EquipmentCategory::NECK]   = { nullptr };
  EquipmentByCategory[EquipmentCategory::TORSO]  = { nullptr };
  EquipmentByCategory[EquipmentCategory::BOOTS]  = { nullptr };
  EquipmentByCategory[EquipmentCategory::WEAPON] = { nullptr };
  EquipmentByCategory[EquipmentCategory::SHIELD] = { nullptr };
  EquipmentByCategory[EquipmentCategory::RING]   = { nullptr, nullptr };

  _inventory = inventoryRef;

  _playerRef = &Application::Instance().PlayerInstance;
}

// =============================================================================

bool EquipmentComponent::Equip(ItemComponent* item)
{
  bool res = false;

  if (item->Data.EqCategory == EquipmentCategory::NOT_EQUIPPABLE)
  {
    if (IsThisPlayer())
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                             Strings::MessageBoxEpicFailHeaderText,
                                             { "Can't be equipped!" },
                                             Colors::MessageBoxRedBorderColor);
    }

    return res;
  }

  auto category = item->Data.EqCategory;

  if (category == EquipmentCategory::RING)
  {
    res = ProcessRingEquiption(item);
  }
  else
  {
    res = ProcessItemEquiption(item);
  }

  return res;
}

// =============================================================================

bool EquipmentComponent::HasBonus(ItemBonusType type)
{
  for (auto& kvp : EquipmentByCategory)
  {
    for (ItemComponent* i : kvp.second)
    {
      if (i != nullptr && i->Data.HasBonus(type))
      {
        return true;
      }
    }
  }

  return false;
}

// --------------------------------- RING --------------------------------------

bool EquipmentComponent::ProcessRingEquiption(ItemComponent* item)
{
  bool emptySlotFound = false;

  auto& rings = EquipmentByCategory[item->Data.EqCategory];

  //
  // First, search if this ring is already equipped.
  //
  for (size_t i = 0; i < rings.size(); i++)
  {
    if (rings[i] == item)
    {
      if (rings[i]->Data.Prefix == ItemPrefix::CURSED)
      {
        rings[i]->Data.IsPrefixDiscovered = true;

        if (IsThisPlayer())
        {
          auto str = Util::StringFormat("You can't unequip %s - it's cursed!", rings[i]->OwnerGameObject->ObjectName.data());
          Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                                 Strings::MessageBoxEpicFailHeaderText,
                                                 { str },
                                                 Colors::MessageBoxRedBorderColor);
        }

        return false;
      }

      UnequipRing(rings[i], i);
      return true;
    }
  }

  //
  // Second, if it's different item, try to find empty slot for it.
  //
  for (size_t i = 0; i < rings.size(); i++)
  {
    if (rings[i] == nullptr)
    {
      EquipRing(item, i);
      return true;
    }
  }

  //
  // Finally, if no empty slots found, display a warning.
  //
  if (!emptySlotFound && IsThisPlayer())
  {
    Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                           Strings::MessageBoxEpicFailHeaderText,
                                           { Strings::MsgUnequipFirst },
                                           Colors::MessageBoxRedBorderColor);
  }

  return false;
}

// =============================================================================

void EquipmentComponent::EquipRing(ItemComponent* ring, int index)
{
  ring->Data.IsEquipped = true;
  EquipmentByCategory[ring->Data.EqCategory][index] = ring;

  OwnerGameObject->ApplyBonuses(ring);

  if (IsThisPlayer())
  {
    std::string objName = ring->Data.IsIdentified ?
                          ring->OwnerGameObject->ObjectName :
                          ring->Data.UnidentifiedName;

    auto str = Util::StringFormat("You put on %s", objName.data());
    Printer::Instance().AddMessage(str);
  }
}

// =============================================================================

void EquipmentComponent::UnequipRing(ItemComponent* ring, int index)
{
  ring->Data.IsEquipped = false;
  EquipmentByCategory[ring->Data.EqCategory][index] = nullptr;

  OwnerGameObject->UnapplyBonuses(ring);

  if (IsThisPlayer())
  {
    std::string objName = ring->Data.IsIdentified ?
                          ring->OwnerGameObject->ObjectName :
                          ring->Data.UnidentifiedName;

    auto str = Util::StringFormat("You take off %s", objName.data());
    Printer::Instance().AddMessage(str);
  }
}

// --------------------------------- ITEM --------------------------------------

bool EquipmentComponent::ProcessItemEquiption(ItemComponent* item)
{
  bool res = true;

  auto itemEquipped = EquipmentByCategory[item->Data.EqCategory][0];

  if (itemEquipped == nullptr)
  {
    //
    // If nothing was equipped, equip item.
    //
    EquipItem(item);
  }
  else if (itemEquipped != item)
  {
    if (IsThisPlayer())
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                             Strings::MessageBoxEpicFailHeaderText,
                                             { Strings::MsgUnequipFirst },
                                             Colors::MessageBoxRedBorderColor);
    }

    res = false;
  }
  else
  {
    if (itemEquipped->Data.Prefix == ItemPrefix::CURSED)
    {
      itemEquipped->Data.IsPrefixDiscovered = true;

      if (IsThisPlayer())
      {
        auto str = Util::StringFormat("You can't unequip %s - it's cursed!", itemEquipped->OwnerGameObject->ObjectName.data());
        Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                               Strings::MessageBoxEpicFailHeaderText,
                                               { str },
                                               Colors::MessageBoxRedBorderColor);
      }

      res = false;
    }
    else
    {
      //
      // If it's the same item, just unequip it.
      //
      UnequipItem(itemEquipped);
    }
  }

  return res;
}

// =============================================================================

void EquipmentComponent::EquipItem(ItemComponent* item)
{
  item->Data.IsEquipped = true;
  EquipmentByCategory[item->Data.EqCategory][0] = item;

  OwnerGameObject->ApplyBonuses(item);

  if (IsThisPlayer())
  {
    std::string verb;

    if (item->Data.EqCategory == EquipmentCategory::WEAPON)
    {
      verb = "arm yourself with";
    }
    else
    {
      verb = "put on";
    }

    std::string objName = item->Data.IsIdentified ?
                          item->OwnerGameObject->ObjectName :
                          item->Data.UnidentifiedName;

    auto message = Util::StringFormat("You %s %s", verb.data(), objName.data());
    Printer::Instance().AddMessage(message);
  }
}

// =============================================================================

void EquipmentComponent::UnequipItem(ItemComponent* item)
{
  item->Data.IsEquipped = false;
  EquipmentByCategory[item->Data.EqCategory][0] = nullptr;

  OwnerGameObject->UnapplyBonuses(item);

  if (IsThisPlayer())
  {
    std::string verb;

    if (item->Data.EqCategory == EquipmentCategory::WEAPON)
    {
      verb = "put away";
    }
    else
    {
      verb = "take off";
    }

    std::string objName = item->Data.IsIdentified
                        ? item->OwnerGameObject->ObjectName
                        : item->Data.UnidentifiedName;

    auto message = Util::StringFormat("You %s %s", verb.data(), objName.data());
    Printer::Instance().AddMessage(message);
  }
}

// =============================================================================

bool EquipmentComponent::IsThisPlayer()
{
  return (OwnerGameObject == _playerRef);
}

// =============================================================================

void EquipmentComponent::Update()
{
}
