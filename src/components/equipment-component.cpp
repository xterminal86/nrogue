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

  _playerRef = &Game::gApp.PlayerInstance;
}

// =============================================================================

bool EquipmentComponent::Equip(ItemComponent* item, bool suppressLog)
{
  bool res = false;

  if (item->Data.EqCategory == EquipmentCategory::NOT_EQUIPPABLE)
  {
    if (IsThisPlayer() && !suppressLog)
    {
      Game::gApp.ShowMessageBox(MessageBoxType::ANY_KEY,
                                 Strings::MessageBoxEpicFailHeaderText,
                                 { "Can't be equipped!" },
                                 Colors::MessageBoxRedBorder);
    }

    return res;
  }

  auto category = item->Data.EqCategory;

  if (category == EquipmentCategory::RING)
  {
    res = ProcessRingEquiption(item, suppressLog);
  }
  else
  {
    res = ProcessItemEquiption(item, suppressLog);
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

bool EquipmentComponent::ProcessRingEquiption(ItemComponent* item,
                                              bool suppressLog)
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

        if (IsThisPlayer() && !suppressLog)
        {
          auto str =
              Util::StringFormat("You can't unequip %s - it's cursed!",
                                 rings[i]->OwnerGameObject->ObjectName.data());
          Game::gApp.ShowMessageBox(MessageBoxType::ANY_KEY,
                                     Strings::MessageBoxEpicFailHeaderText,
                                     { str },
                                     Colors::MessageBoxRedBorder);
        }

        return false;
      }

      UnequipRing(rings[i], i, suppressLog);
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
      EquipRing(item, i, suppressLog);
      return true;
    }
  }

  //
  // Finally, if no empty slots found, display a warning.
  //
  if (!suppressLog && !emptySlotFound && IsThisPlayer())
  {
    Game::gApp.ShowMessageBox(MessageBoxType::ANY_KEY,
                               Strings::MessageBoxEpicFailHeaderText,
                               { Strings::MsgUnequipFirst },
                               Colors::MessageBoxRedBorder);
  }

  return false;
}

// =============================================================================

void EquipmentComponent::EquipRing(ItemComponent* ring,
                                   int index,
                                   bool suppressLog)
{
  ring->Data.IsEquipped = true;
  EquipmentByCategory[ring->Data.EqCategory][index] = ring;

  OwnerGameObject->ApplyBonuses(ring);

  if (IsThisPlayer() && !suppressLog)
  {
    std::string objName = ring->Data.IsIdentified ?
                          ring->OwnerGameObject->ObjectName :
                          ring->Data.UnidentifiedName;

    Game::gPrnt.AddMessage(
      Util::StringFormat("You put on %s", objName.data())
    );
  }
}

// =============================================================================

void EquipmentComponent::UnequipRing(ItemComponent* ring,
                                     int index,
                                     bool suppressLog)
{
  ring->Data.IsEquipped = false;
  EquipmentByCategory[ring->Data.EqCategory][index] = nullptr;

  OwnerGameObject->UnapplyBonuses(ring);

  if (IsThisPlayer() && !suppressLog)
  {
    std::string objName = ring->Data.IsIdentified ?
                          ring->OwnerGameObject->ObjectName :
                          ring->Data.UnidentifiedName;

    Game::gPrnt.AddMessage(
      Util::StringFormat("You take off %s", objName.data())
    );
  }
}

// --------------------------------- ITEM --------------------------------------

bool EquipmentComponent::ProcessItemEquiption(ItemComponent* item,
                                              bool suppressLog)
{
  bool res = true;

  auto itemEquipped = EquipmentByCategory[item->Data.EqCategory][0];

  if (itemEquipped == nullptr)
  {
    //
    // If nothing was equipped, equip item.
    //
    EquipItem(item, suppressLog);
  }
  else if (itemEquipped != item)
  {
    if (IsThisPlayer() && !suppressLog)
    {
      Game::gApp.ShowMessageBox(MessageBoxType::ANY_KEY,
                                 Strings::MessageBoxEpicFailHeaderText,
                                 { Strings::MsgUnequipFirst },
                                 Colors::MessageBoxRedBorder);
    }

    res = false;
  }
  else
  {
    if (itemEquipped->Data.Prefix == ItemPrefix::CURSED)
    {
      itemEquipped->Data.IsPrefixDiscovered = true;

      if (IsThisPlayer() && !suppressLog)
      {
        auto str =
            Util::StringFormat("You can't unequip %s - it's cursed!",
                               itemEquipped->OwnerGameObject->ObjectName.data());
        Game::gApp.ShowMessageBox(MessageBoxType::ANY_KEY,
                                   Strings::MessageBoxEpicFailHeaderText,
                                   { str },
                                   Colors::MessageBoxRedBorder);
      }

      res = false;
    }
    else
    {
      //
      // If it's the same item, just unequip it.
      //
      UnequipItem(itemEquipped, suppressLog);
    }
  }

  return res;
}

// =============================================================================

void EquipmentComponent::EquipItem(ItemComponent* item, bool suppressLog)
{
  item->Data.IsEquipped = true;
  EquipmentByCategory[item->Data.EqCategory][0] = item;

  OwnerGameObject->ApplyBonuses(item);

  if (IsThisPlayer() && !suppressLog)
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

    Game::gPrnt.AddMessage(
      Util::StringFormat("You %s %s", verb.data(), objName.data())
    );
  }
}

// =============================================================================

void EquipmentComponent::UnequipItem(ItemComponent* item, bool suppressLog)
{
  item->Data.IsEquipped = false;
  EquipmentByCategory[item->Data.EqCategory][0] = nullptr;

  OwnerGameObject->UnapplyBonuses(item);

  if (IsThisPlayer() && !suppressLog)
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

    Game::gPrnt.AddMessage(
      Util::StringFormat("You %s %s", verb.data(), objName.data())
    );
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

#ifdef DEBUG_BUILD
StringV EquipmentComponent::Dump(size_t indent)
{
  const std::string spaces(indent, ' ');

  StringV res;

  res.push_back( I_OBJ_START_NAMED(spaces, typeid(*this).name()) );

  StringV base = Component::Dump(indent + 2);
  for (auto& l : base)
  {
    res.push_back(l);
  }

  const std::unordered_map<EquipmentCategory, std::string> reverseMap =
  {
    { EquipmentCategory::NOT_EQUIPPABLE, "NOT_EQUIPPABLE" }
  , { EquipmentCategory::HEAD          , "HEAD"           }
  , { EquipmentCategory::NECK          , "NECK"           }
  , { EquipmentCategory::TORSO         , "TORSO"          }
  , { EquipmentCategory::BOOTS         , "BOOTS"          }
  , { EquipmentCategory::WEAPON        , "WEAPON"         }
  , { EquipmentCategory::SHIELD        , "SHIELD"         }
  , { EquipmentCategory::RING          , "RING"           }
  };

  for (auto& kvp : EquipmentByCategory)
  {
    EquipmentCategory cat = kvp.first;
    std::string list = "[ ";

    for (ItemComponent* c : kvp.second)
    {
      list += Util::StringFormat("0x%" PRIXLEAST64 ", ", c);
    }

    list.pop_back();
    list.pop_back();

    list += " ]";

    res.push_back( I_STR_NAMED(spaces, reverseMap.at(cat).data(), list) );
  }

  res.push_back( I_OBJ_END(spaces) );

  return res;
}
#endif
