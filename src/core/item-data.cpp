#include "item-data.h"

#include "spells-database.h"
#include "util.h"

bool ItemData::IsWeaponOrArmor()
{
  bool cond = (ItemType_ == ItemType::WEAPON
            || ItemType_ == ItemType::RANGED_WEAPON
            || ItemType_ == ItemType::ARMOR);

  return cond;
}

// =============================================================================

bool ItemData::RepairNeeded()
{
  bool cond = (Durability.Min().Get() < Durability.Max().Get());

  return cond;
}

// =============================================================================

int ItemData::GetCost()
{
  bool weaponOrArmor = IsWeaponOrArmor();

  int price = 0;

  if (weaponOrArmor)
  {
    price = Durability.Min().Get() * 2;
  }
  else
  {
    price = Cost;
  }

  if (ItemType_ == ItemType::FOOD)
  {
    price *= 0.05;
  }

  if (ItemType_ == ItemType::GEM)
  {
    price = (IsIdentified) ? Cost : 20;
  }

  if (ItemType_ == ItemType::WAND)
  {
    int capacity = WandCapacity.Get();
    SpellInfo* si = Game::gSD.GetSpellInfoFromDatabase(SpellHeld.SpellType_);
    int spellCost = si->SpellBaseCost;
    price = capacity + spellCost * Amount;
  }
  else
  {
    if (IsStackable || IsChargeable)
    {
      price *= Amount;
    }
  }

  if (ItemType_ == ItemType::RETURNER && !IsIdentified)
  {
    price = 20;
  }

  if (IsIdentified)
  {
    if (Prefix == ItemPrefix::BLESSED)
    {
      price *= 2;
    }
    else if (Prefix == ItemPrefix::CURSED)
    {
      price *= 0.5;
    }

    double newCost = (double)price / _costModByQ.at(ItemQuality_);
    price = (int)newCost;
  }

  return price;
}

// =============================================================================

bool ItemData::HasBonus(ItemBonusType bonusType)
{
  for (auto& i : Bonuses)
  {
    if (i.Type == bonusType)
    {
      return true;
    }
  }

  return false;
}

// =============================================================================

ItemBonusStruct* ItemData::GetBonus(ItemBonusType type)
{
  ItemBonusStruct* res = nullptr;

  for (auto& i : Bonuses)
  {
    if (i.Type == type)
    {
      res = &i;
      break;
    }
  }

  return res;
}

// =============================================================================

int ItemData::GetTotalBonusValue(ItemBonusType type)
{
  int res = 0;

  for (auto& i : Bonuses)
  {
    if (i.Type == type)
    {
      res += i.BonusValue;
    }
  }

  return res;
}

// =============================================================================

bool ItemData::CanBeUsed()
{
  return (UseCallback.target_type() != typeid(void));
}

// =============================================================================

std::vector<std::string> ItemBonusStruct::ToStrings()
{
  std::vector<std::string> res;

  res.push_back(Util::StringFormat("Type              = %i", (int)Type));
  res.push_back(Util::StringFormat("BonusValue        = %i", BonusValue));
  res.push_back(Util::StringFormat("Duration          = %i", Duration));
  res.push_back(Util::StringFormat("Period            = %i", Period));
  res.push_back(Util::StringFormat("EffectCounter     = %i", EffectCounter));
  res.push_back(Util::StringFormat("MoneyCostIncrease = %i", MoneyCostIncrease));
  res.push_back(Util::StringFormat("Cumulative        = %i", Cumulative));
  res.push_back(Util::StringFormat("Persistent        = %i", Persistent));
  res.push_back(Util::StringFormat("Id                = %i", Id));

  return res;
}

#ifdef DEBUG_BUILD
StringV ItemBonusStruct::Dump(size_t indent)
{
  const std::string spaces(indent, ' ');

  StringV res;

  res.push_back( I_OBJ_START(spaces, this) );

  res.push_back( I_INT(spaces, Type) );
  res.push_back( I_INT(spaces, BonusValue) );
  res.push_back( I_INT(spaces, Duration) );
  res.push_back( I_INT(spaces, Period) );
  res.push_back( I_INT(spaces, EffectCounter) );
  res.push_back( I_INT(spaces, MoneyCostIncrease) );
  res.push_back( I_BOOL(spaces, Cumulative) );
  res.push_back( I_BOOL(spaces, Persistent) );
  res.push_back( I_ULL(spaces, Id) );

  res.push_back( I_OBJ_END(spaces) );

  return res;
}
#endif
