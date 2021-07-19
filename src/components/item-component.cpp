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
    destination->Add(OwnerGameObject);
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
      else if (Data.ItemType_ == ItemType::ACCESSORY)
      {
        info = GetAccessoryInspectionInfo();
      }
      else if (Data.ItemType_ == ItemType::RETURNER)
      {
        info = GetReturnerInspectionInfo();
      }
      else if (Data.ItemType_ == ItemType::WAND)
      {
        info = GetWandInspectionInfo();
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
    { Util::StringFormat("DMG: %id%i", Data.Damage.Min().Get(), Data.Damage.Max().Get()) },
    { Util::StringFormat("%i / %i", Data.Durability.Min().Get(), Data.Durability.Max().Get()) }
  };

  AddBonusesInfo(res);

  return res;
}

std::vector<std::string> ItemComponent::GetArmorInspectionInfo()
{
  std::vector<std::string> res =
  {
    { Util::StringFormat("%i / %i", Data.Durability.Min().Get(), Data.Durability.Max().Get()) }
  };

  AddBonusesInfo(res);

  return res;
}

std::vector<std::string> ItemComponent::GetAccessoryInspectionInfo()
{
  std::vector<std::string> res;

  AddBonusesInfo(res);

  if (!res.empty())
  {
    res.erase(res.begin());
  }

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

std::vector<std::string> ItemComponent::GetWandInspectionInfo()
{
  std::vector<std::string> res;

  std::string damage  = Util::StringFormat("Damage  : %id%i ", Data.SpellHeld.SpellBaseDamage.first, Data.SpellHeld.SpellBaseDamage.second);
  std::string charges = Util::StringFormat("Charges : %i", Data.Amount);
  std::string range   = Util::StringFormat("Range   : %i", Data.Range);

  std::string type = "(RES)";

  switch (Data.SpellHeld.SpellType_)
  {
    case SpellType::STRIKE:
    case SpellType::LASER:
    {
      type = "(DEF)";
    }
    break;
  }

  damage += type;

  if (Data.SpellHeld.SpellType_ == SpellType::NONE)
  {
    res.push_back("This wand hasn't been imbued");
    res.push_back("with anything yet");
    res.push_back("");
    res.push_back(charges);
    res.push_back(range);
  }
  else if (Data.SpellHeld.SpellType_ != SpellType::LIGHT)
  {
    // Calculate space padding to align lines relative to 'Damage'

    size_t chargesPadding = damage.length() - charges.length();
    size_t rangePadding   = damage.length() - range.length();

    for (size_t i = 0; i < chargesPadding; i++)
    {
      charges += ' ';
    }

    for (size_t i = 0; i < rangePadding; i++)
    {
      range += ' ';
    }

    res.push_back(damage);
    res.push_back(charges);
    res.push_back(range);
  }

  if (Data.SpellHeld.SpellType_ != SpellType::LIGHT)
  {
    if (Data.Prefix == ItemPrefix::BLESSED)
    {
      res.push_back("");
      res.push_back("This wand has higher capacity");
      res.push_back("and is more effective due to being blessed");
    }
    else if (Data.Prefix == ItemPrefix::CURSED)
    {
      res.push_back("");
      res.push_back("This wand has lower capacity");
      res.push_back("and is dangerous due to being cursed");
    }
  }

  return res;
}

void ItemComponent::AddModifiersInfo(std::vector<std::string>& res)
{
  if (!_allStatNames.empty())
  {
    res.push_back("");
  }

  std::map<StatsEnum, ItemBonusType> bonusByStat =
  {
    { StatsEnum::STR, ItemBonusType::STR },
    { StatsEnum::DEF, ItemBonusType::DEF },
    { StatsEnum::MAG, ItemBonusType::MAG },
    { StatsEnum::RES, ItemBonusType::RES },
    { StatsEnum::SKL, ItemBonusType::SKL },
    { StatsEnum::SPD, ItemBonusType::SPD }
  };

  for (auto& kvp : _allStatNames)
  {
    int bonus = 0;
    for (auto& b : Data.Bonuses)
    {
      if (b.Type == bonusByStat[kvp.second.first])
      {
        bonus = b.BonusValue;
        break;
      }
    }

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
  // Certain items give bonus to stats by default (e.g. dagger to SKL)
  // so to avoid several lines that show increase in the same stat
  // because of additional bonus via magic count them all beforehand.
  //
  // Also innate bonus is force-added, so we need to check if it's 0
  // due to curse penalty and not include it in the info screen.
  auto ret = CountAllStatBonuses();
  if (_nonZeroStatBonuses != 0)
  {
    res.push_back("");

    AppendStatBonuses(ret, res);
  }

  // If we have stat bonuses and non-stat bonuses,
  // separate them with empty line.
  if (_nonZeroStatBonuses != 0 && _nonStatBonusesPresent)
  {
    res.push_back("");
  }

  for (auto& i : Data.Bonuses)
  {
    switch (i.Type)
    {
      case ItemBonusType::INDESTRUCTIBLE:
        res.push_back("Does not wear out");
        break;

      case ItemBonusType::SELF_REPAIR:
      {
        auto str = Util::StringFormat("Repairs 1 point of durability every %i turns", i.Period);
        res.push_back(str);
      }
      break;

      case ItemBonusType::VISIBILITY:
      {
        auto str = Util::StringFormat("+%i to light radius", i.BonusValue);
        res.push_back(str);
      }
      break;

      case ItemBonusType::TELEPATHY:
        res.push_back("You can sense nearby monsters");
        break;

      case ItemBonusType::INVISIBILITY:
        res.push_back("Makes you invisible");
        break;

      case ItemBonusType::DAMAGE:
      {
        auto str = Util::StringFormat("+%i to total damage", i.BonusValue);
        res.push_back(str);
      }
      break;

      case ItemBonusType::HUNGER:
        res.push_back("Removes need for food");
        break;

      case ItemBonusType::IGNORE_DEFENCE:
        res.push_back("Ignores target's DEF");
        break;

      case ItemBonusType::KNOCKBACK:
      {
        int tiles = i.BonusValue;
        std::string tilesStr = (tiles == 1) ? "tile" : "tiles";
        auto str = Util::StringFormat("Knocks enemy %i %s back", tiles, tilesStr.data());
        res.push_back(str);
      }
      break;

      case ItemBonusType::MANA_SHIELD:
        res.push_back("Applies mana shield while equipped");
        break;

      case ItemBonusType::REGEN:
      {
        auto str = Util::StringFormat("Regenerates 1 HP every %i turns", i.Period);
        res.push_back(str);
      }
      break;

      case ItemBonusType::REFLECT:
        res.push_back("Reflects spells while equipped");
        break;

      case ItemBonusType::LEECH:
      {
        auto str = Util::StringFormat("%i%c of the inflicted physical damage goes to HP", i.BonusValue, '%');
        res.push_back(str);
      }
      break;

      case ItemBonusType::DMG_ABSORB:
      {
        auto str = Util::StringFormat("Absorbs %i physical damage", i.BonusValue);
        res.push_back(str);
      }
      break;

      case ItemBonusType::MAG_ABSORB:
      {
        auto str = Util::StringFormat("Absorbs %i magical damage", i.BonusValue);
        res.push_back(str);
      }
      break;

      case ItemBonusType::THORNS:
      {
        auto str = Util::StringFormat("Returns %i%c of the received physical damage", i.BonusValue, '%');
        res.push_back(str);
      }
      break;
    }
  }
}

void ItemComponent::AppendStatBonuses(const std::map<ItemBonusType, int>& statBonuses, std::vector<std::string>& res)
{
  for (auto& kvp : statBonuses)
  {
    if (kvp.second == 0)
    {
      continue;
    }

    std::string name = _bonusNameByType[kvp.first];
    auto modStr = Util::StringFormat("%i", kvp.second);
    if (kvp.second > 0)
    {
      modStr.insert(modStr.begin(), '+');
    }

    auto str = Util::StringFormat("%s: %s", name.data(), modStr.data());
    res.push_back(str);
  }
}

std::map<ItemBonusType, int> ItemComponent::CountAllStatBonuses()
{
  _nonZeroStatBonuses = 0;
  _nonStatBonusesPresent = false;

  std::map<ItemBonusType, int> allStatModifiers =
  {
    { ItemBonusType::STR, 0 },
    { ItemBonusType::DEF, 0 },
    { ItemBonusType::MAG, 0 },
    { ItemBonusType::RES, 0 },
    { ItemBonusType::SPD, 0 },
    { ItemBonusType::SKL, 0 },
    { ItemBonusType::HP,  0 },
    { ItemBonusType::MP,  0 }
  };

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
        allStatModifiers[i.Type] += i.BonusValue;
      }
      break;

      default:
        _nonStatBonusesPresent = true;
        break;
    }
  }

  for (auto& kvp : allStatModifiers)
  {
    if (kvp.second == 0)
    {
      continue;
    }

    _nonZeroStatBonuses++;
  }

  return allStatModifiers;
}
