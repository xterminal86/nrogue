#include "item-data.h"

bool ItemData::IsWeaponOrArmor()
{
  bool cond = (ItemType_ == ItemType::WEAPON
            || ItemType_ == ItemType::RANGED_WEAPON
            || ItemType_ == ItemType::ARMOR);

  return cond;
}

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
    price *= 0.05f;
  }

  if (ItemType_ == ItemType::GEM)
  {
    price = (IsIdentified) ? Cost : 20;
  }

  if (IsStackable || IsChargeable)
  {
    price *= Amount;
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
      price *= 0.5f;
    }

    std::map<ItemQuality, float> costModByQ =
    {
      { ItemQuality::CRACKED,      2.0f },
      { ItemQuality::FLAWED,       1.5f },
      { ItemQuality::NORMAL,       1.0f },
      { ItemQuality::FINE,         0.8f },
      { ItemQuality::EXCEPTIONAL,  0.6f }
    };

    float newCost = (float)price / costModByQ[ItemQuality_];
    price = (int)newCost;
  }

  return price;
}

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
