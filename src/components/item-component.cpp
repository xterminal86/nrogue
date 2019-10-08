#include "item-component.h"

#include "application.h"
#include "map.h"
#include "game-objects-factory.h"

ItemComponent::ItemComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void ItemComponent::Update()
{
}

/// Transfers GameObject to the destination,
/// or tile occupied by player if destination = nullptr
void ItemComponent::Transfer(ContainerComponent* destination)
{
  // OwnerGameObject should be in released state

  if (destination == nullptr)
  {
    OwnerGameObject->PosX = Application::Instance().PlayerInstance.PosX;
    OwnerGameObject->PosY = Application::Instance().PlayerInstance.PosY;

    Map::Instance().InsertGameObject(OwnerGameObject);
  }
  else
  {
    destination->AddToInventory(OwnerGameObject);
  }
}

bool ItemComponent::Equip()
{
  return GameObjectsFactory::Instance().HandleItemEquip(this);
}

std::pair<std::string, StringsArray2D> ItemComponent::GetInspectionInfo(bool overrideDescriptions)
{
  std::pair<std::string, StringsArray2D> res;

  std::string header = Data.IsIdentified ? Data.IdentifiedName : Data.UnidentifiedName;
  StringsArray2D lore = Data.IsIdentified ? Data.IdentifiedDescription : Data.UnidentifiedDescription;
  //StringsArray2D lore;

  if (!overrideDescriptions)
  {
    if (Data.IsIdentified)
    {
      std::vector<std::string> info;

      if (Data.ItemType_ == ItemType::WEAPON
       || Data.ItemType_ == ItemType::RANGED_WEAPON)
      {
        info = GetWeaponInspectionInfo();
      }
      else if (Data.ItemType_ == ItemType::ARMOR)
      {
        info = GetArmorInspectionInfo();
      }
      else if (Data.ItemType_ == ItemType::RETURNER)
      {
        info = GetReturnerInspectionInfo();
      }

      // To avoid empty line at the bottom if there is no description.
      if (!lore.empty() && !info.empty())
      {
        lore.push_back("");
      }

      for (auto& s : info)
      {
        lore.push_back(s);
      }
    }
    else if (!Data.IsIdentified && Data.IsPrefixDiscovered)
    {
      if (Data.Prefix == ItemPrefix::BLESSED)
      {
        lore.push_back("This one is blessed and will perform better.");
      }
      else if (Data.Prefix == ItemPrefix::CURSED)
      {
        lore.push_back("This one is cursed and should be avoided.");
      }
    }
  }

  res.first = header;
  res.second = lore;

  return res;
}

void ItemComponent::Inspect(bool overrideDescriptions)
{  
  auto lore = GetInspectionInfo(overrideDescriptions);
  Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, lore.first, lore.second);
}

bool ItemComponent::Use()
{
  return GameObjectsFactory::Instance().HandleItemUse(this);
}

// ************************** PRIVATE METHODS ************************** //

std::vector<std::string> ItemComponent::GetWeaponInspectionInfo()
{
  std::vector<std::string> res =
  {
    { Util::StringFormat("DMG: %id%i", Data.Damage.CurrentValue, Data.Damage.OriginalValue) },
    { Util::StringFormat("%i / %i", Data.Durability.CurrentValue, Data.Durability.OriginalValue) },
    { "" },
    { Util::StringFormat("STR required: %i", Data.StatRequirements[StatsEnum::STR]) },
    { "" }
  };

  AddModifiersInfo(res);
  AddBonusesInfo(res);

  return res;
}

std::vector<std::string> ItemComponent::GetArmorInspectionInfo()
{
  std::vector<std::string> res =
  {
    { Util::StringFormat("%i / %i", Data.Durability.CurrentValue, Data.Durability.OriginalValue) },
    { "" },
    { Util::StringFormat("STR required: %i", Data.StatRequirements[StatsEnum::STR]) },
    { "" }
  };

  AddModifiersInfo(res);
  AddBonusesInfo(res);

  return res;
}

std::vector<std::string> ItemComponent::GetReturnerInspectionInfo()
{
  std::string text;

  MapType mt = Data.ReturnerPosition.first;
  Position pos =
  {
    Data.ReturnerPosition.second.first,
    Data.ReturnerPosition.second.second
  };

  if (Data.Amount != 0)
  {
    if (mt == MapType::NOWHERE)
    {
      text = "This one has not been attuned yet.";
    }
    else
    {
      auto levelName = Map::Instance().GetLevelRefByType(mt)->LevelName;
      text = Util::StringFormat("Set to %s at [%i;%i]", levelName.data(), pos.X, pos.Y);
    }
  }
  else
  {
    text = "The stone is inert.";
  }

  std::vector<std::string> res =
  {
    "This strange stone lets you mark a spot",
    "and then instantly return to it any time you want."
  };

  if (Data.Prefix == ItemPrefix::BLESSED)
  {
    res.push_back("This one is blessed and can be used longer.");
  }
  else if (Data.Prefix == ItemPrefix::CURSED)
  {
    res.push_back("This one is cursed and its performance is quite unpredictable.");
  }

  res.push_back(text);

  return res;
}

void ItemComponent::AddModifiersInfo(std::vector<std::string>& res)
{
  for (auto& kvp : _allStatNames)
  {
    int bonus = Data.StatBonuses.at(kvp.second.first);
    if (bonus == 0)
    {
      continue;
    }

    std::string statName = kvp.second.second;
    char prefix = ' ';
    if (bonus < 0)
    {
      prefix = '-';
    }
    else if (bonus > 0)
    {
      prefix = '+';
    }

    std::string bonusStr = std::to_string(std::abs(bonus));
    char bonus1 = bonusStr[0];
    char bonus2 = ' ';
    if (std::abs(bonus) >= 10)
    {
      bonus2 = bonusStr[1];
    }

    auto str = Util::StringFormat("%s: %c%c%c", statName.data(), prefix, bonus1, bonus2);
    res.push_back(str);
  }
}

void ItemComponent::AddBonusesInfo(std::vector<std::string>& res)
{
  std::map<ItemBonusType, std::string> bonusNameByType =
  {
    { ItemBonusType::STR, "STR" },
    { ItemBonusType::DEF, "DEF" },
    { ItemBonusType::MAG, "MAG" },
    { ItemBonusType::RES, "RES" },
    { ItemBonusType::SPD, "SPD" },
    { ItemBonusType::SKL, "SKL" },
    { ItemBonusType::HP,  "HP"  },
    { ItemBonusType::MP,  "MP"  }
  };

  if (!Data.Bonuses.empty())
  {
    res.push_back("");
  }

  for (auto& i : Data.Bonuses)
  {
    switch (i.Type)
    {
      case ItemBonusType::STR:
      case ItemBonusType::DEF:
      case ItemBonusType::MAG:
      case ItemBonusType::RES:
      case ItemBonusType::SPD:
      case ItemBonusType::SKL:
      case ItemBonusType::HP:
      case ItemBonusType::MP:
      {
        std::string name = bonusNameByType[i.Type];
        auto str = Util::StringFormat("%i to %s", i.Value, name.data());
        res.push_back(str);
      }
      break;

      case ItemBonusType::INDESTRUCTIBLE:
        res.push_back("Indestructible");
        break;

      case ItemBonusType::SELF_REPAIR:
        res.push_back("Repairs durability over time");
        break;

      case ItemBonusType::VISIBILITY:
      {
        auto str = Util::StringFormat("%i to light radius", i.Value);
        res.push_back(str);
      }
      break;

      case ItemBonusType::DAMAGE:
      {
        auto str = Util::StringFormat("%i to total damage", i.Value);
        res.push_back(str);
      }
      break;

      case ItemBonusType::HUNGER:
        res.push_back("Removes need for food");
        break;

      case ItemBonusType::IGNORE_DEFENCE:
        res.push_back("Ignores target DEF");
        break;

      case ItemBonusType::KNOCKBACK:
        res.push_back("Knocks back the enemy");
        break;

      case ItemBonusType::MANA_SHIELD:
        res.push_back("Applies mana shield while equipped");
        break;

      case ItemBonusType::REGEN:
        res.push_back("Regenerates HP over time");
        break;

      case ItemBonusType::REFLECT:
        res.push_back("Reflects spells while equipped");
        break;

      case ItemBonusType::LEECH:
      {
        auto str = Util::StringFormat("%i damage goes to HP", i.Value);
        res.push_back(str);
      }
      break;

      case ItemBonusType::DMG_ABSORB:
      {
        auto str = Util::StringFormat("Absorbs %i of melee damage", i.Value);
        res.push_back(str);
      }
      break;

      case ItemBonusType::MAG_ABSORB:
      {
        auto str = Util::StringFormat("Absorbs %i of magic damage", i.Value);
        res.push_back(str);
      }
      break;

      case ItemBonusType::THORNS:
      {
        auto str = Util::StringFormat("Enemy receives %i of inflicted damage", i.Value);
        res.push_back(str);
      }
      break;
    }
  }
}
