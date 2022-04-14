#ifndef ITEMDATA_H
#define ITEMDATA_H

#include "constants.h"
#include "attribute.h"
#include "spells-database.h"

class GameObject;

struct ItemBonusStruct
{
  ItemBonusType Type = ItemBonusType::NONE;

  //
  // Usually used to add bonus to stats.
  //
  // In case of lingering damage (like poison or burn),
  // value must be negative.
  //
  // Must not be 0 or bonus won't be added unless 'force' flag
  // is true in ItemsFactory::AddBonusToItem().
  //
  int BonusValue = 0;

  //
  // Whole duration of effect in turns.
  // -1 for infinite duration.
  //
  int Duration = -1;

  // Number of turns after which effect action occurs
  int Period = -1;

  //
  // Internal variable to count number of turns passed
  // to determine whether we hit period variable above.
  //
  int EffectCounter = 0;

  // How much certain bonus affects item's cost in shop
  int MoneyCostIncrease = 0;

  // Unused
  bool IsCursed = false;

  // Can be stacked if true
  bool Cumulative = false;

  //
  // Effect is gained from some sort of equipped item
  // (ring or pendant and such)
  //
  bool FromItem = false;

  //
  // We shall use this to monitor active effects and remove them
  // if necessary (e.g. if item is unequipped, effect must go away)
  //
  uint64_t Id = 0;

  std::vector<std::string> ToStrings();
};

//
// WARNING: possible god object
//
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

  WandMaterials WandMaterial = WandMaterials::NONE;

  RangedWeaponType RangedWeaponType_ = RangedWeaponType::RANDOM;
  ArrowType        AmmoType          = ArrowType::NONE;
  WeaponType       WeaponType_       = WeaponType::RANDOM;

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
  bool RepairNeeded();
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

  std::function<UseResult(ItemComponent*,GameObject*)> UseCallback;

  size_t ItemTypeHash;
};

#endif // ITEMDATA_H
