#include "item-component.h"

#include "application.h"
#include "map.h"
#include "game-objects-factory.h"

ItemComponent::ItemComponent()
{
  _hash = typeid(*this).hash_code();
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

void ItemComponent::Inspect()
{  
  std::string header = Data.IsIdentified ? Data.IdentifiedName : Data.UnidentifiedName;
  auto desc = Data.IsIdentified ? Data.IdentifiedDescription : Data.UnidentifiedDescription;

  if (Data.IsIdentified)
  {
    if (Data.ItemType_ == ItemType::WEAPON)
    {
      desc = GetWeaponInspectionInfo();
    }
    else if (Data.ItemType_ == ItemType::ARMOR)
    {
      desc = GetArmorInspectionInfo();
    }
    else if (Data.ItemType_ == ItemType::RETURNER)
    {
      desc = GetReturnerInspectionInfo();
    }
  }
  else if (!Data.IsIdentified && Data.IsPrefixDiscovered)
  {
    if (Data.Prefix == ItemPrefix::BLESSED)
    {
      desc.push_back("This one is blessed and will perform better.");
    }
    else if (Data.Prefix == ItemPrefix::CURSED)
    {
      desc.push_back("This one is cursed and should be avoided.");
    }
  }

  Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY, header, desc);
}

void ItemComponent::Throw()
{
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
    "and instantly return to it any time you want."
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
