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
  if (Data.IsPrefixDiscovered)
  {
    if (Data.Prefix == ItemPrefix::BLESSED)
    {
      desc.push_back("This one is blessed and will yield better results.");
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
