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
      Printer::Instance().AddMessage("...but nothing happens.");
      break;
  }
}

void SpellsProcessor::ProcessScroll(ItemComponent* scroll)
{
  Printer::Instance().AddMessage("You read the scroll...");

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

    default:
      Printer::Instance().AddMessage("...but nothing happens.");
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
    if ((ic->Data.ItemType_ == ItemType::WEAPON || ic->Data.ItemType_ == ItemType::ARMOR)
     && (ic->Data.Durability.CurrentValue < ic->Data.Durability.OriginalValue))
    {
      itemsToRepair.push_back(ic);
    }
  }

  if (itemsToRepair.empty())
  {
    Printer::Instance().AddMessage("...but nothing happens.");
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

    itemsToRepair[0]->Data.Durability.CurrentValue = itemsToRepair[0]->Data.Durability.OriginalValue;
  }
  else if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    Printer::Instance().AddMessage("Your items have been repaired!");

    for (auto& i : itemsToRepair)
    {
      i->Data.Durability.CurrentValue = i->Data.Durability.OriginalValue;
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
      Printer::Instance().AddMessage("...but nothing happens.");
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
      Printer::Instance().AddMessage("...but nothing happens.");
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
      Printer::Instance().AddMessage("...but nothing happens.");
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
    Printer::Instance().AddMessage("...but nothing happens.");
    return;
  }

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    int power = RNG::Instance().RandomRange(1, 10);
    _playerRef->AddEffect(EffectType::POISONED, power, 10, true, true);

    Printer::Instance().AddMessage("You feel unwell!");
  }
  else
  {
    _playerRef->RemoveEffect(EffectType::POISONED);

    Printer::Instance().AddMessage("You feel better");

    if (scroll->Data.Prefix == ItemPrefix::BLESSED)
    {
      _playerRef->AddEffect(EffectType::REGEN, 1, 20, false, true);
    }
  }
}

void SpellsProcessor::ProcessScrollOfHealing(ItemComponent* scroll)
{
  int power = _playerRef->Attrs.HP.OriginalValue;

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    power = RNG::Instance().RandomRange(1, _playerRef->Attrs.HP.CurrentValue / 2);
    Printer::Instance().AddMessage("You writhe in pain!");
  }
  else if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    power = -power;
    Printer::Instance().AddMessage("Your wounds are healed completely!");
  }
  else
  {
    power = -power / 2;
    Printer::Instance().AddMessage("You feel better");
  }

  _playerRef->ReceiveDamage(nullptr, power, true, false, true);
}

void SpellsProcessor::ProcessWandOfLight(ItemComponent* wand)
{
  int playerPow = _playerRef->Attrs.Mag.Get();
  int power = wand->Data.WandCapacity.CurrentValue / 100 + playerPow;
  int duration = wand->Data.WandCapacity.CurrentValue;

  if (wand->Data.Prefix == ItemPrefix::BLESSED)
  {
    duration *= 2;
    power *= 2;

    if (!wand->Data.IsPrefixDiscovered)
    {
      Printer::Instance().AddMessage("The golden light surrounds you!");
    }
  }
  else if (wand->Data.Prefix == ItemPrefix::CURSED)
  {
    duration /= 2;
    power = -power;

    if (!wand->Data.IsPrefixDiscovered)
    {
      Printer::Instance().AddMessage("You are surrounded by darkness!");
    }
  }

  if (!wand->Data.IsPrefixDiscovered
   && !wand->Data.IsIdentified)
  {
    wand->Data.IsPrefixDiscovered = true;
    wand->Data.IsIdentified = true;
  }

  _playerRef->AddEffect(EffectType::ILLUMINATED, power, duration, false, true);
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
  int power = playerPow;
  int duration = playerPow * 10;

  if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    duration *= 2;
    power *= 2;

    Printer::Instance().AddMessage("The golden light surrounds you!");
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    duration /= 2;
    power = -power;

    Printer::Instance().AddMessage("You are surrounded by darkness!");
  }

  _playerRef->AddEffect(EffectType::ILLUMINATED, power, duration, false, true);
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

  _playerRef->AddEffect(EffectType::INFRAVISION, power, duration, false, true);
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

    Map::Instance().TeleportToExistingLevel(MapType::TOWN, res);
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
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
