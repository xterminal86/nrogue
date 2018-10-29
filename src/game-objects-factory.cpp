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
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Description = { "You can buy things with these" };

  int pl = _playerRef->Attrs.Lvl.CurrentValue;

  int money = (amount == 0) ? RNG::Instance().RandomRange(1 * pl, 10 * pl) : amount;
  ic->Data.Cost = money;
  ic->Data.Amount = money;
  ic->Data.IsStackable = true;
  ic->Data.TypeOfItem = ItemType::COINS;

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
    int randomSpd = RNG::Instance().RandomRange(1 * pl, 2 * pl);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
  }
  else
  {
    // For debugging purposes
    go->ObjectName = "Battle Rat";

    go->Attrs.Str.Set(2 * _playerRef->Attrs.Lvl.CurrentValue);
    go->Attrs.Def.Set(1 * _playerRef->Attrs.Lvl.CurrentValue);
    go->Attrs.Spd.Set(10);

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

  if (item->Data.EqCategory == EquipmentCategory::NOT_EQUIPPABLE)
  {
    Application::Instance().ShowMessageBox(false, "Information", { "Can't be equipped!" }, GlobalConstants::MessageBoxRedBorderColor);
    return res;
  }

  // TODO: cursed items

  auto category = item->Data.EqCategory;

  if (category == EquipmentCategory::RING)
  {
    res = ProcessRingEquiption(item);
  }
  else
  {
    res = ProcessItemEquiption(item);
  }

  return res;
}

bool GameObjectsFactory::ProcessItemEquiption(ItemComponent* item)
{
  bool res = true;

  auto itemEquipped = _playerRef->EquipmentByCategory[item->Data.EqCategory][0];

  if (itemEquipped == nullptr)
  {
    // If nothing was equipped, equip item
    EquipItem(item);
  }
  else if (itemEquipped != item)
  {
    Application::Instance().ShowMessageBox(false, "Information", { "Unequip first!" }, GlobalConstants::MessageBoxRedBorderColor);
    res = false;
  }
  else
  {
    // If it's the same item, just unequip it
    UnequipItem(itemEquipped);
  }

  return res;
}

void GameObjectsFactory::EquipItem(ItemComponent* item)
{
  item->Data.IsEquipped = true;
  _playerRef->EquipmentByCategory[item->Data.EqCategory][0] = item;

  std::string verb;

  if (item->Data.EqCategory == EquipmentCategory::WEAPON)
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
  item->Data.IsEquipped = false;
  _playerRef->EquipmentByCategory[item->Data.EqCategory][0] = nullptr;

  std::string verb;

  if (item->Data.EqCategory == EquipmentCategory::WEAPON)
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

bool GameObjectsFactory::ProcessRingEquiption(ItemComponent* item)
{
  bool emptySlotFound = false;

  auto& rings = _playerRef->EquipmentByCategory[item->Data.EqCategory];

  // First, search if this ring is already equipped
  for (int i = 0; i < rings.size(); i++)
  {
    if (rings[i] == item)
    {
      UnequipRing(rings[i], i);
      return true;
    }
  }

  // Second, if it's different item, try to find empty slot for it
  for (int i = 0; i < rings.size(); i++)
  {
    if (rings[i] == nullptr)
    {
      EquipRing(item, i);
      return true;
    }
  }

  // Finally, if no empty slots found, display a warning
  if (!emptySlotFound)
  {
    Application::Instance().ShowMessageBox(false, "Information", { "Unequip first!" }, GlobalConstants::MessageBoxRedBorderColor);
  }

  return false;
}

void GameObjectsFactory::EquipRing(ItemComponent* ring, int index)
{
  ring->Data.IsEquipped = true;
  _playerRef->EquipmentByCategory[ring->Data.EqCategory][index] = ring;

  auto str = Util::StringFormat("You put on %s", ((GameObject*)ring->OwnerGameObject)->ObjectName.data());
  Printer::Instance().AddMessage(str);
}

void GameObjectsFactory::UnequipRing(ItemComponent* ring, int index)
{
  ring->Data.IsEquipped = false;
  _playerRef->EquipmentByCategory[ring->Data.EqCategory][index] = nullptr;

  auto str = Util::StringFormat("You take off %s", ((GameObject*)ring->OwnerGameObject)->ObjectName.data());
  Printer::Instance().AddMessage(str);
}

bool GameObjectsFactory::HandleItemUse(ItemComponent* item)
{
  bool res = false;

  switch (item->Data.TypeOfItem)
  {
    case ItemType::HEALING_POTION:
      if (_playerRef->Attrs.HP.CurrentValue == _playerRef->Attrs.HP.OriginalValue)
      {        
        Application::Instance().ShowMessageBox(false, "Information", { "You are already at full health!" }, GlobalConstants::MessageBoxRedBorderColor);
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

        Application::Instance().ShowMessageBox(false, "Information", { m1, m2 }, GlobalConstants::MessageBoxRedBorderColor);
      }
      else if (_playerRef->Attrs.MP.CurrentValue == _playerRef->Attrs.MP.OriginalValue)
      {
        Application::Instance().ShowMessageBox(false, "Information", { "You are already in fighting spirit!" }, GlobalConstants::MessageBoxRedBorderColor);
      }
      else
      {
        _playerRef->Attrs.MP.Set(_playerRef->Attrs.MP.OriginalValue);
        Printer::Instance().AddMessage("Your spirit is reinforced!");
        res = true;
      }
      break;

    case ItemType::COINS:      
      Application::Instance().ShowMessageBox(false, "Information", { "You don't 'use' money like that" }, GlobalConstants::MessageBoxRedBorderColor);
      break;

    default:
      auto go = (GameObject*)item->OwnerGameObject;
      auto msg = Util::StringFormat("You can't use %s!", go->ObjectName.data());
      Application::Instance().ShowMessageBox(false, "Information", { msg }, GlobalConstants::MessageBoxRedBorderColor);
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
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Data.TypeOfItem = ItemType::HEALING_POTION;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;

  ic->Description = { "Restores you to full health" };

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
  ItemComponent* ic = static_cast<ItemComponent*>(c);

  ic->Data.TypeOfItem = ItemType::MANA_POTION;
  ic->Data.Amount = 1;
  ic->Data.IsStackable = true;

  ic->Description = { "Helps you regain spiritual powers" };

  return go;
}
