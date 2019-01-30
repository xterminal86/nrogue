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
    { Util::StringFormat("STR: %i", Data.StatBonuses[StatsEnum::STR]) },
    { Util::StringFormat("DEF: %i", Data.StatBonuses[StatsEnum::DEF]) },
    { Util::StringFormat("MAG: %i", Data.StatBonuses[StatsEnum::MAG]) },
    { Util::StringFormat("RES: %i", Data.StatBonuses[StatsEnum::RES]) },
    { Util::StringFormat("SKL: %i", Data.StatBonuses[StatsEnum::SKL]) },
    { Util::StringFormat("SPD: %i", Data.StatBonuses[StatsEnum::SPD]) }
  };

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
