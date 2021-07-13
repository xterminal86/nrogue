#ifndef ITEMDATA_H
#define ITEMDATA_H

#include "constants.h"
#include "attribute.h"
#include "spells-database.h"

struct ItemBonusStruct
{
  ItemBonusType Type = ItemBonusType::NONE;

  // Usually used to add bonus to stats
  int BonusValue = 0;

  // Whole duration of effect in turns
  int Duration = -1;

  // Number of turns after which effect action occurs
  int Period = -1;

  // Internal variable to count number of turns passed
  // to determine whether we hit period variable above.
  int EffectCounter = 0;

  int MoneyCostIncrease = 0;

  bool IsCursed   = false;
  bool Cumulative = false;
  bool FromItem   = false;

  uint64_t Id = 0;
};

// WARNING: possible god object
struct ItemData
{
  ItemType    ItemType_    = ItemType::DUMMY;
  ItemPrefix  Prefix       = ItemPrefix::UNCURSED;
  ItemRarity  Rarity       = ItemRarity::COMMON;
  ItemQuality ItemQuality_ = ItemQuality::NORMAL;

  EquipmentCategory EqCategory = EquipmentCategory::NOT_EQUIPPABLE;

  Attribute WandCapacity;

  RangedAttribute Durability;
  RangedAttribute Damage;

  SpellInfo SpellHeld;

  RangedWeaponType RangedWeaponType_ = RangedWeaponType::NONE;
  ArrowType        AmmoType          = ArrowType::NONE;
  WeaponType       WeaponType_       = WeaponType::NONE;

  std::pair<MapType, std::pair<int, int>> ReturnerPosition =
  {
    MapType::NOWHERE, { -1, -1 }
  };

  bool CanBeUsed();

  bool IsEquipped         = false;
  bool IsStackable        = false;
  bool IsIdentified       = false;
  bool IsPrefixDiscovered = false;
  bool IsChargeable       = false;
  bool IsBurnable         = false;

  // Can't be dropped from inventory if true
  bool IsImportant = false;

  // For stackable
  int Amount = 1;

  // For ranged weapons and wands
  int Range = 1;

  // !!! Use GetCost() when cost is needed !!!
  int Cost = 0;

  bool IsWeaponOrArmor();
  bool HasBonus(ItemBonusType bonusType);
  ItemBonusStruct* GetBonus(ItemBonusType type);

  int GetCost();

  // BUC status + object name [+ suffix]
  // Used in inspection window
  std::string IdentifiedName;

  // ? + GameObject::ObjectName + ?
  std::string UnidentifiedName;

  std::vector<std::string> UnidentifiedDescription;
  std::vector<std::string> IdentifiedDescription;

  std::vector<ItemBonusStruct> Bonuses;

  std::function<bool(void*)> UseCallback;
  std::function<void(void*)> EquipCallback;

  size_t ItemTypeHash;
};

#endif // ITEMDATA_H
