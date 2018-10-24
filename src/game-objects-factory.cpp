#include "game-objects-factory.h"

#include "constants.h"
#include "rng.h"
#include "item-component.h"
#include "shrine-component.h"
#include "ai-monster-basic.h"
#include "go-timer-destroyer.h"
#include "map.h"
#include "application.h"

void GameObjectsFactory::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

GameObject* GameObjectsFactory::CreateGameObject(int x, int y, ItemType objType)
{
  GameObject* go = nullptr;

  switch (objType)
  {
    case ItemType::COINS:
      go = CreateMoney();
      break;

    default:
      break;
  }

  go->PosX = x;
  go->PosY = y;

  return go;
}

GameObject* GameObjectsFactory::CreateMonster(int x, int y, MonsterType monsterType)
{
  GameObject* go = nullptr;

  switch (monsterType)
  {
    case MonsterType::RAT:
      go = CreateRat(x, y);
      break;
  }

  go->PosX = x;
  go->PosY = y;

  return go;
}

GameObject* GameObjectsFactory::CreateMoney(int amount)
{
  GameObject* go = new GameObject();

  go->ObjectName = "Gold Coins";
  go->Image = '$';
  go->FgColor = GlobalConstants::CoinsColor;

  Component* c = go->AddComponent<ItemComponent>();

  ((ItemComponent*)c)->Description = { "You can buy things with these" };

  int pl = _playerRef->Attrs.Lvl.CurrentValue;

  int money = (amount == 0) ? RNG::Instance().RandomRange(1 * pl, 10 * pl) : amount;
  ((ItemComponent*)c)->Cost = money;
  ((ItemComponent*)c)->Amount = money;
  ((ItemComponent*)c)->IsStackable = true;
  ((ItemComponent*)c)->TypeOfObject = ItemType::COINS;

  return go;
}

GameObject* GameObjectsFactory::CreateShrine(int x, int y, ShrineType type, int timeout)
{
  GameObject* go = new GameObject(x, y, '/',
                                (type == ShrineType::MIGHT) ?
                                GlobalConstants::ShrineMightColor :
                                GlobalConstants::ShrineSpiritColor,
                                GlobalConstants::WallColor);

  auto c = go->AddComponent<ShrineComponent>();
  ShrineComponent* sc = static_cast<ShrineComponent*>(c);

  sc->Type = type;
  sc->Counter = timeout;
  sc->Timeout = timeout;

  go->ObjectName = (type == ShrineType::MIGHT) ? "Shrine of Might" : "Shrine of Spirit";
  go->FogOfWarName = "?Shrine?";
  go->InteractionCallback = std::bind(&ShrineComponent::Interact, sc);

  return go;
}

GameObject* GameObjectsFactory::CreateRat(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(x, y, 'r', GlobalConstants::MonsterColor);
  go->ObjectName = "Rat";
  go->Attrs.Indestructible = false;

  // Sets Occupied flag for _currentCell
  go->Move(0, 0);

  auto c = go->AddComponent<AIMonsterBasic>();
  AIMonsterBasic* ai = static_cast<AIMonsterBasic*>(c);

  ai->AgroRadius = 8;

  // Set attributes
  if (randomize)
  {
    int pl = _playerRef->Attrs.Lvl.CurrentValue;

    int randomStr = RNG::Instance().RandomRange(1 * pl, 2 * pl);
    int randomDef = RNG::Instance().RandomRange(0, 1 * pl);
    int randomSkl = RNG::Instance().RandomRange(0, 1 * pl);
    int randomHp = RNG::Instance().RandomRange(3 * pl, 10 * pl);
    int randomSpd = RNG::Instance().RandomRange(30 * pl, 60 * pl);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
  }
  else
  {
    // For debugging purposes
    go->Attrs.Str.Set(2 * _playerRef->Attrs.Lvl.CurrentValue);
    go->Attrs.Def.Set(1 * _playerRef->Attrs.Lvl.CurrentValue);

    go->Attrs.HP.Set(10);
  }

  return go;
}

GameObject* GameObjectsFactory::CreateRemains(GameObject* from)
{
  GameObject* go = new GameObject(from->PosX, from->PosY, '%', from->FgColor);

  auto td = go->AddComponent<TimerDestroyerComponent>();
  ((TimerDestroyerComponent*)td)->Time = from->Attrs.HP.OriginalValue * 2;

  auto str = Util::StringFormat("%s's remains", from->ObjectName.data());
  go->ObjectName = str;

  return go;
}

bool GameObjectsFactory::HandleItemEquip(ItemComponent* item)
{
  bool res = false;

  if (item->EquipmentType == EquipmentCategory::NOT_EQUIPPABLE)
  {
    Application::Instance().ShowMessageBox("Information", { "Can't be equipped!" });
    return res;
  }

  // TODO: cursed items

  auto category = item->EquipmentType;

  if (category == EquipmentCategory::RING)
  {
    ProcessRingEquiption(item);
    res = true;
  }
  else
  {
    ProcessItemEquiption(item);
    res = true;
  }

  return res;
}

void GameObjectsFactory::ProcessItemEquiption(ItemComponent* item)
{
  auto itemEquipped = _playerRef->EquipmentByCategory[item->EquipmentType][0];

  if (itemEquipped == nullptr)
  {
    // If nothing was equipped, equip item
    EquipItem(item);
  }
  else if (itemEquipped != item)
  {
    // FIXME: performed in one action
    //
    // If something was equipped, replace with item
    UnequipItem(itemEquipped);
    EquipItem(item);
  }
  else
  {
    // If it's the same item, just unequip it
    UnequipItem(itemEquipped);
  }
}

void GameObjectsFactory::EquipItem(ItemComponent* item)
{
  item->IsEquipped = true;
  _playerRef->EquipmentByCategory[item->EquipmentType][0] = item;

  std::string verb;

  if (item->EquipmentType == EquipmentCategory::WEAPON)
  {
    verb = "arm yourself with";
  }
  else
  {
    verb = "put on";
  }

  auto message = Util::StringFormat("You %s %s", verb.data(), ((GameObject*)item->OwnerGameObject)->ObjectName.data());
  Printer::Instance().AddMessage(message);
}

void GameObjectsFactory::UnequipItem(ItemComponent* item)
{
  item->IsEquipped = false;
  _playerRef->EquipmentByCategory[item->EquipmentType][0] = nullptr;

  std::string verb;

  if (item->EquipmentType == EquipmentCategory::WEAPON)
  {
    verb = "put away";
  }
  else
  {
    verb = "take off";
  }

  auto message = Util::StringFormat("You %s %s", verb.data(), ((GameObject*)item->OwnerGameObject)->ObjectName.data());
  Printer::Instance().AddMessage(message);
}

void GameObjectsFactory::ProcessRingEquiption(ItemComponent* item)
{
  bool emptySlotFound = false;

  auto& rings = _playerRef->EquipmentByCategory[item->EquipmentType];

  // First, search if this ring is already equipped
  for (int i = 0; i < rings.size(); i++)
  {
    if (rings[i] == item)
    {
      UnequipRing(rings[i], i);
      return;
    }
  }

  // Second, if it's different item, try to find empty slot for it
  for (int i = 0; i < rings.size(); i++)
  {
    if (rings[i] == nullptr)
    {
      EquipRing(item, i);
      return;
    }
  }

  // Finally, if no empty slots found, replace first equipped ring
  if (!emptySlotFound)
  {
    UnequipRing(rings[0], 0);
    EquipRing(item, 0);
  }
}

void GameObjectsFactory::EquipRing(ItemComponent* ring, int index)
{
  ring->IsEquipped = true;
  _playerRef->EquipmentByCategory[ring->EquipmentType][index] = ring;

  auto str = Util::StringFormat("You put on %s", ((GameObject*)ring->OwnerGameObject)->ObjectName.data());
  Printer::Instance().AddMessage(str);
}

void GameObjectsFactory::UnequipRing(ItemComponent* ring, int index)
{
  ring->IsEquipped = false;
  _playerRef->EquipmentByCategory[ring->EquipmentType][index] = nullptr;

  auto str = Util::StringFormat("You take off %s", ((GameObject*)ring->OwnerGameObject)->ObjectName.data());
  Printer::Instance().AddMessage(str);
}

bool GameObjectsFactory::HandleItemUse(ItemComponent* item)
{
  bool res = false;

  switch (item->TypeOfObject)
  {
    case ItemType::HEALING_POTION:
      if (_playerRef->Attrs.HP.CurrentValue == _playerRef->Attrs.HP.OriginalValue)
      {        
        Application::Instance().ShowMessageBox("Information", { "You are already at full health!" });
      }
      else
      {
        _playerRef->Attrs.HP.Set(_playerRef->Attrs.HP.OriginalValue);
        Printer::Instance().AddMessage("Your wounds are healed!");
        res = true;
      }
      break;

    case ItemType::MANA_POTION:
      if (_playerRef->Attrs.MP.OriginalValue == 0)
      {
        std::string m1 = "Your spirituality is too low";
        std::string m2 = "for this to have any effect on you";

        Application::Instance().ShowMessageBox("Information", { m1, m2 });
      }
      else if (_playerRef->Attrs.MP.CurrentValue == _playerRef->Attrs.MP.OriginalValue)
      {        
        Application::Instance().ShowMessageBox("Information", { "You are already in fighting spirit!" });
      }
      else
      {
        _playerRef->Attrs.MP.Set(_playerRef->Attrs.MP.OriginalValue);
        Printer::Instance().AddMessage("Your spirit is reinforced!");
        res = true;
      }
      break;

    case ItemType::COINS:      
      Application::Instance().ShowMessageBox("Information", { "You don't 'use' money like that" });
      break;

    default:
      auto go = (GameObject*)item->OwnerGameObject;
      auto msg = Util::StringFormat("You can't use %s!", go->ObjectName.data());
      Application::Instance().ShowMessageBox("Information", { msg });
      break;
  }

  return res;
}

GameObject* GameObjectsFactory::CreateHealingPotion()
{
  GameObject* go = new GameObject();
  go->FgColor = "#FFFFFF";
  go->BgColor = "#FF0000";
  go->Image = '!';
  go->ObjectName = "Healing Potion";

  Component* c = go->AddComponent<ItemComponent>();

  ((ItemComponent*)c)->TypeOfObject = ItemType::HEALING_POTION;
  ((ItemComponent*)c)->Amount = 1;
  ((ItemComponent*)c)->IsStackable = true;

  ((ItemComponent*)c)->Description = { "Restores you to full health" };

  return go;
}

GameObject* GameObjectsFactory::CreateManaPotion()
{
  GameObject* go = new GameObject();
  go->FgColor = "#FFFFFF";
  go->BgColor = "#0000FF";
  go->Image = '!';
  go->ObjectName = "Mana Potion";

  Component* c = go->AddComponent<ItemComponent>();

  ((ItemComponent*)c)->TypeOfObject = ItemType::MANA_POTION;
  ((ItemComponent*)c)->Amount = 1;
  ((ItemComponent*)c)->IsStackable = true;

  ((ItemComponent*)c)->Description = { "Helps you regain spiritual powers" };

  return go;
}
