#ifndef ITEMDATA_H
#define ITEMDATA_H

#include "constants.h"
#include "attribute.h"

// NOTE: possible god object
struct ItemData
{
  ItemType ItemType_ = ItemType::DUMMY;
  ItemPrefix Prefix = ItemPrefix::UNCURSED;
  ItemRarity Rarity = ItemRarity::COMMON;

  EquipmentCategory EqCategory = EquipmentCategory::NOT_EQUIPPABLE;

  Attribute Durability;
  Attribute Damage;
  Attribute WandCapacity;

  SpellType SpellHeld = SpellType::NONE;

  RangedWeaponType RangedWeaponType_ = RangedWeaponType::NONE;
  ArrowType AmmoType = ArrowType::NONE;
  WeaponType WeaponType_ = WeaponType::NONE;

  int IgnoreArmorPercentage = 0;

  std::pair<MapType, std::pair<int, int>> ReturnerPosition =
  {
    MapType::NOWHERE, { -1, -1 }
  };

  bool IsUsable = false;
  bool IsEquipped = false;
  bool IsStackable = false;
  bool IsIdentified = false;
  bool IsPrefixDiscovered = false;
  bool IsChargeable = false;

  // For stackable
  int Amount = 1;

  // For ranged weapons
  int Range = 1;

  // !!! Use GetCost() when cost is needed !!!
  int Cost = 0;

  bool IsWeaponOrArmor();

  int GetCost();

  std::map<StatsEnum, int> StatBonuses =
  {
    { StatsEnum::STR, 0 },
    { StatsEnum::DEF, 0 },
    { StatsEnum::MAG, 0 },
    { StatsEnum::RES, 0 },
    { StatsEnum::SKL, 0 },
    { StatsEnum::SPD, 0 },
    { StatsEnum::HP,  0 },
    { StatsEnum::MP,  0 }
  };

  std::map<StatsEnum, int> StatRequirements =
  {
    { StatsEnum::STR, 0 },
    { StatsEnum::DEF, 0 },
    { StatsEnum::MAG, 0 },
    { StatsEnum::RES, 0 },
    { StatsEnum::SKL, 0 },
    { StatsEnum::SPD, 0 },
    { StatsEnum::HP,  0 },
    { StatsEnum::MP,  0 }
  };

  // BUC status + object name [+ suffix]
  // Used in inspection window
  std::string IdentifiedName;

  // ? + GameObject::ObjectName + ?
  std::string UnidentifiedName;

  std::vector<std::string> UnidentifiedDescription;
  std::vector<std::string> IdentifiedDescription;

  std::function<bool(void*)> UseCallback;
  std::function<void(void*)> EquipCallback;

  size_t ItemTypeHash;
};

#endif // ITEMDATA_H
