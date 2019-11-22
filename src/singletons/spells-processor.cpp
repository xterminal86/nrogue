#include "spells-processor.h"

#include "application.h"
#include "printer.h"
#include "map.h"
#include "town-portal-component.h"

void SpellsProcessor::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void SpellsProcessor::ProcessWand(ItemComponent* wand)
{
  Printer::Instance().AddMessage("You invoke the wand...");

  wand->Data.Amount--;

  switch (wand->Data.SpellHeld)
  {
    case SpellType::LIGHT:
      ProcessWandOfLight(wand);
      break;

    default:
      Printer::Instance().AddMessage(_kNoActionText);
      break;
  }
}

void SpellsProcessor::ProcessScroll(ItemComponent* scroll)
{
  std::string scrollName = scroll->Data.IsIdentified ?
                           scroll->Data.IdentifiedName :
                           scroll->Data.UnidentifiedName;

  auto str = Util::StringFormat("You read the scroll %s...", scrollName.data());

  Printer::Instance().AddMessage(str);

  switch(scroll->Data.SpellHeld)
  {
    case SpellType::LIGHT:
      ProcessScrollOfLight(scroll);
      break;

    case SpellType::MAGIC_MAPPING:
      ProcessScrollOfMM(scroll);
      break;

    case SpellType::HEAL:
      ProcessScrollOfHealing(scroll);
      break;

    case SpellType::NEUTRALIZE_POISON:
      ProcessScrollOfNeutralizePoison(scroll);
      break;

    case SpellType::IDENTIFY:
      ProcessScrollOfIdentify(scroll);
      break;

    case SpellType::REPAIR:
      ProcessScrollOfRepair(scroll);
      break;

    case SpellType::DETECT_MONSTERS:
      ProcessScrollOfDetectMonsters(scroll);
      break;

    case SpellType::TOWN_PORTAL:
      ProcessScrollOfTownPortal(scroll);
      break;

    case SpellType::TELEPORT:
      ProcessScrollOfTeleport(scroll);
      break;

    case SpellType::MANA_SHIELD:
      ProcessScrollOfManaShield(scroll);
      break;

    case SpellType::REMOVE_CURSE:
      ProcessScrollOfRemoveCurse(scroll);
      break;

    default:
      Printer::Instance().AddMessage(_kNoActionText);
      break;
  }

  Printer::Instance().AddMessage("The scroll crumbles to dust");
}

void SpellsProcessor::ProcessScrollOfRepair(ItemComponent* scroll)
{
  std::vector<ItemComponent*> itemsToRepair;
  for (auto& i : _playerRef->Inventory.Contents)
  {
    ItemComponent* ic = i->GetComponent<ItemComponent>();
    if ((ic->Data.ItemType_ == ItemType::WEAPON
      || ic->Data.ItemType_ == ItemType::ARMOR)
     && (ic->Data.Durability.Min().Get() < ic->Data.Durability.Max().Get()))
    {
      itemsToRepair.push_back(ic);
    }
  }

  if (itemsToRepair.empty())
  {
    Printer::Instance().AddMessage(_kNoActionText);
    return;
  }

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    int index = RNG::Instance().RandomRange(0, itemsToRepair.size());
    ItemComponent* item = itemsToRepair[index];

    _playerRef->BreakItem(item);
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    auto str = Util::StringFormat("Your %s is repaired!", itemsToRepair[0]->OwnerGameObject->ObjectName.data());
    Printer::Instance().AddMessage(str);

    itemsToRepair[0]->Data.Durability.Restore();
  }
  else if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    Printer::Instance().AddMessage("Your items have been repaired!");

    for (auto& i : itemsToRepair)
    {
      i->Data.Durability.Restore();
    }
  }
}

void SpellsProcessor::ProcessScrollOfIdentify(ItemComponent* scroll)
{
  std::vector<ItemComponent*> itemsToId;
  std::vector<ItemComponent*> itemsKnown;
  for (auto& i : _playerRef->Inventory.Contents)
  {
    ItemComponent* ic = i->GetComponent<ItemComponent>();
    if (!ic->Data.IsIdentified)
    {
      itemsToId.push_back(ic);
    }
    else
    {
      itemsKnown.push_back(ic);
    }
  }

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    if (itemsKnown.empty())
    {
      Printer::Instance().AddMessage(_kNoActionText);
      return;
    }

    Printer::Instance().AddMessage("You forget the details about inventory item!");

    int index = RNG::Instance().RandomRange(0, itemsKnown.size());
    auto item = itemsKnown[index];
    item->Data.IsIdentified = false;
    item->Data.IsPrefixDiscovered = false;
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    if (itemsToId.empty())
    {
      Printer::Instance().AddMessage(_kNoActionText);
      return;
    }

    Printer::Instance().AddMessage("Inventory item has been identified!");

    itemsToId[0]->Data.IsPrefixDiscovered = true;
    itemsToId[0]->Data.IsIdentified = true;
  }
  else if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    if (itemsToId.empty())
    {
      Printer::Instance().AddMessage(_kNoActionText);
      return;
    }

    for (auto& i : itemsToId)
    {
      i->Data.IsPrefixDiscovered = true;
      i->Data.IsIdentified = true;
    }

    Printer::Instance().AddMessage("Your possessions have been identified!");
  }
}

void SpellsProcessor::ProcessScrollOfNeutralizePoison(ItemComponent* scroll)
{
  if (!_playerRef->HasEffect(EffectType::POISONED))
  {
    Printer::Instance().AddMessage(_kNoActionText);
    return;
  }

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    int power = RNG::Instance().RandomRange(1, 10);
    _playerRef->AddEffect(EffectType::POISONED, power, 10, true);

    Printer::Instance().AddMessage("You feel unwell!");
  }
  else
  {
    _playerRef->RemoveEffect(EffectType::POISONED);

    Printer::Instance().AddMessage("You feel better");

    if (scroll->Data.Prefix == ItemPrefix::BLESSED)
    {
      _playerRef->AddEffect(EffectType::REGEN, 1, 20, false);
    }
  }
}

void SpellsProcessor::ProcessScrollOfHealing(ItemComponent* scroll)
{
  int power = _playerRef->Attrs.HP.Max().Get();
  bool atFullHealth = (_playerRef->Attrs.HP.Min().Get() == _playerRef->Attrs.HP.Max().Get());

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    power = RNG::Instance().RandomRange(1, _playerRef->Attrs.HP.Min().Get() / 2);
    Printer::Instance().AddMessage("You writhe in pain!");
  }
  else
  {
    if (atFullHealth)
    {
      Printer::Instance().AddMessage(_kNoActionText);
      return;
    }

    if (scroll->Data.Prefix == ItemPrefix::BLESSED)
    {
      power = -power;
      Printer::Instance().AddMessage("Your wounds are healed completely!");
    }
    else
    {
      power = -power / 2;
      Printer::Instance().AddMessage("You feel better");
    }
  }

  _playerRef->ReceiveDamage(nullptr, power, true, false, true);
}

void SpellsProcessor::ProcessWandOfLight(ItemComponent* wand)
{
  int playerPow = _playerRef->Attrs.Mag.Get();
  int power = wand->Data.WandCapacity.Max().Get() / 100 + playerPow;
  int duration = wand->Data.WandCapacity.Max().Get();

  std::string message = "The golden light surrounds you!";

  if (wand->Data.Prefix == ItemPrefix::BLESSED)
  {
    duration *= 2;
    power *= 2;
  }
  else if (wand->Data.Prefix == ItemPrefix::CURSED)
  {
    duration /= 2;
    power = -power;

    message = "You are surrounded by darkness!";
  }

  if (!wand->Data.IsPrefixDiscovered
   && !wand->Data.IsIdentified)
  {
    wand->Data.IsPrefixDiscovered = true;
    wand->Data.IsIdentified = true;
  }

  Printer::Instance().AddMessage(message);

  _playerRef->AddEffect(EffectType::ILLUMINATED, power, duration, false);
}

void SpellsProcessor::ProcessScrollOfMM(ItemComponent* scroll)
{
  // NOTE: blessed scroll reveals traps as well

  auto& mapRef = Map::Instance().CurrentLevel;

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    for (int x = 0; x < mapRef->MapSize.X; x++)
    {
      for (int y = 0; y < mapRef->MapSize.Y; y++)
      {
        mapRef->MapArray[x][y]->Revealed = false;
      }
    }

    Map::Instance().CurrentLevel->ExitFound = false;

    Printer::Instance().AddMessage("You suddenly forget where you are!");
  }
  else
  {
    for (int x = 0; x < mapRef->MapSize.X; x++)
    {
      for (int y = 0; y < mapRef->MapSize.Y; y++)
      {
        mapRef->MapArray[x][y]->Revealed = true;
      }
    }

    Map::Instance().CurrentLevel->ExitFound = true;

    Printer::Instance().AddMessage("A map coalesces in your mind!");

    if (scroll->Data.Prefix == ItemPrefix::BLESSED)
    {
      // TODO: reveal traps

      std::vector<std::string> monstersOnLevel;

      for (auto& actor : mapRef->ActorGameObjects)
      {
        std::string str = Util::StringFormat("You sense: %s (danger %i)", actor->ObjectName.data(), actor->Attrs.Rating());
        monstersOnLevel.push_back(str);
      }

      for (auto& str : monstersOnLevel)
      {
        Printer::Instance().AddMessage(str);
      }
    }
  }
}

void SpellsProcessor::ProcessScrollOfLight(ItemComponent* scroll)
{
  int playerPow = _playerRef->Attrs.Mag.Get();
  if (playerPow <= 0)
  {
    playerPow = 1;
  }

  std::string message = "The golden light surrounds you!";

  int power = (playerPow - 1) + 6;
  int duration = playerPow * 10;

  if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    duration *= 2;
    power *= 2;
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    duration /= 2;
    power = -power;

    message = "You are surrounded by darkness!";
  }

  Printer::Instance().AddMessage(message);

  _playerRef->AddEffect(EffectType::ILLUMINATED, power, duration, false);
}

void SpellsProcessor::ProcessScrollOfDetectMonsters(ItemComponent* scroll)
{
  int playerPow = _playerRef->Attrs.Mag.Get();
  if (playerPow <= 0)
  {
    playerPow = 1;
  }

  int power = 10 + playerPow * 2;
  int duration = 20 + playerPow * 2;

  if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    duration *= 2;
    power *= 2;
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    // TODO: what else to do with player?

    duration /= 2;
    power /= 2;
  }

  Printer::Instance().AddMessage("You can sense nearby creatures");

  _playerRef->AddEffect(EffectType::TELEPATHY, power, duration, false);
}

void SpellsProcessor::ProcessScrollOfTownPortal(ItemComponent* scroll)
{
  auto lvl = Map::Instance().GetLevelRefByType(MapType::TOWN);
  if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    Position tpPos = lvl->TownPortalPos();
    std::vector<Position> posToAppear =
    {
      { tpPos.X - 1, tpPos.Y },
      { tpPos.X + 1, tpPos.Y },
      { tpPos.X, tpPos.Y - 1 },
      { tpPos.X, tpPos.Y + 1 },
    };

    int index = RNG::Instance().RandomRange(0, posToAppear.size());
    Position res = posToAppear[index];

    GameObject* portal = new GameObject(lvl, tpPos.X, tpPos.Y, 'O', "#FFFFFF", "#0000FF");

    portal->BlocksSight = true;
    portal->ObjectName = "Town Portal";

    TownPortalComponent* tpc = portal->AddComponent<TownPortalComponent>();
    tpc->SavePosition(Map::Instance().CurrentLevel->MapType_, { _playerRef->PosX, _playerRef->PosY });

    lvl->InsertGameObject(portal);

    Printer::Instance().AddMessage("You're back in town all of a sudden!");
    Map::Instance().TeleportToExistingLevel(MapType::TOWN, res);        
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    Printer::Instance().AddMessage("You're back in town all of a sudden!");
    Map::Instance().TeleportToExistingLevel(MapType::TOWN, lvl->TownPortalPos());
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    auto& mapRef = Map::Instance().CurrentLevel;
    int index = RNG::Instance().RandomRange(0, mapRef->EmptyCells().size());
    auto pos = mapRef->EmptyCells()[index];
    Map::Instance().TeleportToExistingLevel(mapRef->MapType_, pos);

    Printer::Instance().AddMessage("You are suddenly transported elsewhere!");
  }
}

void SpellsProcessor::ProcessScrollOfTeleport(ItemComponent* scroll)
{
  auto& mapRef = Map::Instance().CurrentLevel;

  // TODO: blessed scroll of teleport - what to do?

  if (scroll->Data.Prefix == ItemPrefix::UNCURSED
   || scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    int index = RNG::Instance().RandomRange(0, mapRef->EmptyCells().size());
    auto pos = mapRef->EmptyCells()[index];
    Map::Instance().TeleportToExistingLevel(mapRef->MapType_, pos);
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    int rx = RNG::Instance().RandomRange(1, mapRef->MapSize.X);
    int ry = RNG::Instance().RandomRange(1, mapRef->MapSize.Y);
    Position pos = { rx, ry };
    Map::Instance().TeleportToExistingLevel(mapRef->MapType_, pos);
  }

  Printer::Instance().AddMessage("You are suddenly transported elsewhere!");
}

void SpellsProcessor::ProcessScrollOfManaShield(ItemComponent *scroll)
{
  if (_playerRef->Attrs.MP.Max().Get() > 0)
  {
    if (scroll->Data.Prefix == ItemPrefix::BLESSED)
    {
      _playerRef->Attrs.MP.Restore();
      Printer::Instance().AddMessage("Your spirit is reinforced!");
    }
    else if (scroll->Data.Prefix == ItemPrefix::CURSED)
    {
      int valueToAdd = _playerRef->Attrs.MP.Min().Get() / 2;
      _playerRef->Attrs.MP.AddMin(-valueToAdd);

      Printer::Instance().AddMessage("Your spirit force was drained!");
    }

    _playerRef->AddEffect(EffectType::MANA_SHIELD, 0, -1, false);
  }
  else
  {
    Printer::Instance().AddMessage(_kNoActionText);
  }
}

void SpellsProcessor::ProcessScrollOfRemoveCurse(ItemComponent* scroll)
{
  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    std::vector<ItemComponent*> nonCursedItems;
    for (auto& i : _playerRef->Inventory.Contents)
    {
      auto c = i->GetComponent<ItemComponent>();
      ItemComponent* ic = static_cast<ItemComponent*>(c);
      if (ic->Data.Prefix != ItemPrefix::CURSED)
      {
        nonCursedItems.push_back(ic);
      }
    }

    if (!nonCursedItems.empty())
    {
      int index = RNG::Instance().RandomRange(0, nonCursedItems.size());
      ItemComponent* item = nonCursedItems[index];
      item->Data.IsPrefixDiscovered = true;
      item->Data.Prefix = ItemPrefix::CURSED;

      Printer::Instance().AddMessage("The malevolent energy creeps in...");
    }
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    std::vector<ItemComponent*> cursedItems;
    for (auto& i : _playerRef->Inventory.Contents)
    {
      auto c = i->GetComponent<ItemComponent>();
      ItemComponent* ic = static_cast<ItemComponent*>(c);
      if (ic->Data.Prefix == ItemPrefix::CURSED)
      {
        cursedItems.push_back(ic);
      }
    }

    if (!cursedItems.empty())
    {
      int index = RNG::Instance().RandomRange(0, cursedItems.size());
      ItemComponent* item = cursedItems[index];
      item->Data.IsPrefixDiscovered = true;
      item->Data.Prefix = ItemPrefix::UNCURSED;

      Printer::Instance().AddMessage("The malevolent energy disperses!");
    }
  }
  else if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    bool success = false;
    for (auto& i : _playerRef->Inventory.Contents)
    {
      auto c = i->GetComponent<ItemComponent>();
      ItemComponent* ic = static_cast<ItemComponent*>(c);
      if (ic->Data.Prefix == ItemPrefix::CURSED)
      {
        ic->Data.IsPrefixDiscovered = true;
        ic->Data.Prefix = ItemPrefix::UNCURSED;
        success = true;
      }
    }

    if (success)
    {
      Printer::Instance().AddMessage("The malevolent energy disperses completely!");
    }
  }
}
