#include "spells-processor.h"

#include "application.h"
#include "printer.h"
#include "map.h"
#include "town-portal-component.h"
#include "game-objects-factory.h"

void SpellsProcessor::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void SpellsProcessor::ProcessWand(ItemComponent* wand)
{
  Printer::Instance().AddMessage("You invoke the wand...");

  wand->Data.Amount--;

  switch (wand->Data.SpellHeld.SpellType_)
  {
    case SpellType::LIGHT:
      ProcessWandOfLight(wand);
      break;

    default:
      Printer::Instance().AddMessage(_kNoActionText);
      break;
  }
}

void SpellsProcessor::ProcessWandOfLight(ItemComponent* wand)
{
  int radius = Map::Instance().CurrentLevel->VisibilityRadius;

  int playerPow = _playerRef->Attrs.Mag.Get();

  int power = radius + wand->Data.Range + playerPow;

  int duration = playerPow * GlobalConstants::EffectDefaultDuration;
  if (duration == 0)
  {
    duration = GlobalConstants::EffectDefaultDuration;
  }

  std::string message = "The golden light surrounds you!";

  if (wand->Data.Prefix == ItemPrefix::BLESSED)
  {
    duration *= 2;
    power    *= 2;
  }
  else if (wand->Data.Prefix == ItemPrefix::CURSED)
  {
    power = -GlobalConstants::MaxVisibilityRadius;
    message = "You are surrounded by darkness!";
  }

  if (!wand->Data.IsPrefixDiscovered
   && !wand->Data.IsIdentified)
  {
    wand->Data.IsPrefixDiscovered = true;
    wand->Data.IsIdentified = true;
  }

  Printer::Instance().AddMessage(message);

  ItemBonusStruct b;

  b.Type       = ItemBonusType::ILLUMINATED;
  b.BonusValue = power;
  b.Duration   = duration;
  b.Id         = wand->OwnerGameObject->ObjectId();

  _playerRef->AddEffect(b);
}

void SpellsProcessor::ProcessScroll(ItemComponent* scroll)
{
  std::string scrollName = scroll->Data.IsIdentified ?
                           scroll->Data.IdentifiedName :
                           scroll->Data.UnidentifiedName;

  auto str = Util::StringFormat("You read the scroll %s...", scrollName.data());

  Printer::Instance().AddMessage(str);

  switch(scroll->Data.SpellHeld.SpellType_)
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

    case SpellType::ENCHANT:
      ProcessScrollOfEnchantment(scroll);
      break;

    default:
      Printer::Instance().AddMessage(_kNoActionText);
      break;
  }

  Printer::Instance().AddMessage("The scroll crumbles to dust");
}

void SpellsProcessor::ProcessScrollOfRepair(ItemComponent* scroll)
{
  _playerRef->RememberItem(scroll, GlobalConstants::UnidentifiedEffectText);

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

  _playerRef->RememberItem(scroll, "repair");
}

void SpellsProcessor::ProcessScrollOfIdentify(ItemComponent* scroll)
{
  // TODO: identify control

  _playerRef->RememberItem(scroll, GlobalConstants::UnidentifiedEffectText);

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

    _playerRef->RememberItem(scroll, "identify");
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

    _playerRef->RememberItem(scroll, "identify");
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

    _playerRef->RememberItem(scroll, "identify");
  }
}

void SpellsProcessor::ProcessScrollOfNeutralizePoison(ItemComponent* scroll)
{
  _playerRef->RememberItem(scroll, GlobalConstants::UnidentifiedEffectText);

  int playerPow = _playerRef->Attrs.Mag.Get();

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    ItemBonusStruct b;
    b.Type = ItemBonusType::POISONED;
    b.BonusValue = -1;
    b.Period = 10;
    b.Duration = GlobalConstants::EffectDefaultDuration;
    b.Cumulative = true;
    b.Id = scroll->OwnerGameObject->ObjectId();

    _playerRef->AddEffect(b);

    Printer::Instance().AddMessage("You feel unwell!");

    _playerRef->RememberItem(scroll, "neutralize poison");
  }
  else
  {
    if (!_playerRef->HasEffect(ItemBonusType::POISONED))
    {
      Printer::Instance().AddMessage(_kNoActionText);
      return;
    }

    _playerRef->DispelEffect(ItemBonusType::POISONED);

    Printer::Instance().AddMessage("You feel better");

    if (scroll->Data.Prefix == ItemPrefix::BLESSED)
    {
      ItemBonusStruct b;
      b.Type = ItemBonusType::REGEN;
      b.BonusValue = 1;
      b.Period = 10;
      b.Duration = (playerPow <= 0) ? GlobalConstants::EffectDefaultDuration : GlobalConstants::EffectDefaultDuration * playerPow;
      b.Cumulative = true;
      b.Id = scroll->OwnerGameObject->ObjectId();

      _playerRef->AddEffect(b);
    }

    _playerRef->RememberItem(scroll, "neutralize poison");
  }
}

void SpellsProcessor::ProcessScrollOfHealing(ItemComponent* scroll)
{
  _playerRef->RememberItem(scroll, GlobalConstants::UnidentifiedEffectText);

  int power = _playerRef->Attrs.HP.Max().Get();
  bool atFullHealth = (_playerRef->Attrs.HP.Min().Get() == _playerRef->Attrs.HP.Max().Get());

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    power = RNG::Instance().RandomRange(1, _playerRef->Attrs.HP.Min().Get() / 2);
    power = -power;
    Printer::Instance().AddMessage("You writhe in pain!");

    _playerRef->RememberItem(scroll, "healing");
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
      Printer::Instance().AddMessage("Your wounds are healed completely!");
    }
    else
    {
      power = power / 2;
      Printer::Instance().AddMessage("You feel better");
    }
  }

  _playerRef->Attrs.HP.AddMin(power);

  _playerRef->RememberItem(scroll, "healing");
}

void SpellsProcessor::ProcessScrollOfLight(ItemComponent* scroll)
{
  int radius = Map::Instance().CurrentLevel->VisibilityRadius;

  int playerPow = _playerRef->Attrs.Mag.Get();
  if (playerPow < 0)
  {
    playerPow = 0;
  }

  std::string message = "The golden light surrounds you!";

  int power = radius + playerPow;

  int duration = playerPow * GlobalConstants::EffectDefaultDuration;
  if (duration == 0)
  {
    duration = GlobalConstants::EffectDefaultDuration;
  }

  if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    duration *= 2;
    power    *= 2;
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    power   = -GlobalConstants::MaxVisibilityRadius;
    message = "You are surrounded by darkness!";
  }

  Printer::Instance().AddMessage(message);

  ItemBonusStruct b;

  b.Type       = ItemBonusType::ILLUMINATED;
  b.BonusValue = power;
  b.Duration   = duration;
  b.Id         = scroll->OwnerGameObject->ObjectId();

  _playerRef->AddEffect(b);

  _playerRef->RememberItem(scroll, "light");
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

  _playerRef->RememberItem(scroll, "magic mapping");
}

void SpellsProcessor::ProcessScrollOfDetectMonsters(ItemComponent* scroll)
{
  int playerPow = _playerRef->Attrs.Mag.Get();
  if (playerPow <= 0)
  {
    playerPow = 1;
  }

  int power = 10 + playerPow * 2;

  int duration = playerPow * GlobalConstants::EffectDefaultDuration;
  if (duration == 0)
  {
    duration = GlobalConstants::EffectDefaultDuration;
  }

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

  ItemBonusStruct b;
  b.Type = ItemBonusType::TELEPATHY;
  b.BonusValue = power; // TODO: power is unused in this effect (see Map::DrawActors())
  b.Duration = duration;
  b.Id = scroll->OwnerGameObject->ObjectId();

  _playerRef->AddEffect(b);

  _playerRef->RememberItem(scroll, "detect monsters");
}

void SpellsProcessor::ProcessScrollOfTownPortal(ItemComponent* scroll)
{
  _playerRef->RememberItem(scroll, GlobalConstants::UnidentifiedEffectText);

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

    GameObject* portal = new GameObject(lvl, tpPos.X, tpPos.Y, '0', "#FFFFFF", "#0000FF");

    portal->BlocksSight = true;
    portal->ObjectName = "Town Portal";

    TownPortalComponent* tpc = portal->AddComponent<TownPortalComponent>();
    tpc->SavePosition(Map::Instance().CurrentLevel->MapType_, { _playerRef->PosX, _playerRef->PosY });

    lvl->InsertGameObject(portal);

    Printer::Instance().AddMessage("You're back in town all of a sudden!");
    Map::Instance().TeleportToExistingLevel(MapType::TOWN, res);

    _playerRef->RememberItem(scroll, "town portal");
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    Printer::Instance().AddMessage("You're back in town all of a sudden!");
    Map::Instance().TeleportToExistingLevel(MapType::TOWN, lvl->TownPortalPos());

    _playerRef->RememberItem(scroll, "town portal");
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    auto& mapRef = Map::Instance().CurrentLevel;
    int index = RNG::Instance().RandomRange(0, mapRef->EmptyCells().size());
    auto pos = mapRef->EmptyCells()[index];
    Map::Instance().TeleportToExistingLevel(mapRef->MapType_, pos);

    Printer::Instance().AddMessage("You are suddenly transported elsewhere!");

    // Don't remember because it can be a scroll of teleport as well
  }
}

void SpellsProcessor::ProcessScrollOfTeleport(ItemComponent* scroll)
{
  _playerRef->RememberItem(scroll, GlobalConstants::UnidentifiedEffectText);

  Printer::Instance().AddMessage("You are suddenly transported elsewhere!");

  auto& mapRef = Map::Instance().CurrentLevel;

  // TODO: blessed scroll of teleport - what to do?

  if (scroll->Data.Prefix == ItemPrefix::UNCURSED
   || scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    int index = RNG::Instance().RandomRange(0, mapRef->EmptyCells().size());
    auto pos = mapRef->EmptyCells()[index];
    Map::Instance().TeleportToExistingLevel(mapRef->MapType_, pos);

    _playerRef->RememberItem(scroll, "teleport");
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    int rx = RNG::Instance().RandomRange(1, mapRef->MapSize.X);
    int ry = RNG::Instance().RandomRange(1, mapRef->MapSize.Y);
    Position pos = { rx, ry };
    Map::Instance().TeleportToExistingLevel(mapRef->MapType_, pos);

    // Don't remember since it can be cursed scroll of TP
  }
}

void SpellsProcessor::ProcessScrollOfManaShield(ItemComponent *scroll)
{
  _playerRef->RememberItem(scroll, GlobalConstants::UnidentifiedEffectText);

  bool manaShieldOk = (_playerRef->Attrs.MP.Min().Get() != 0 ||
                      (_playerRef->Attrs.MP.Max().Get() != 0 &&
                       _playerRef->Attrs.Mag.Get() > 0));

  if (!manaShieldOk)
  {
    Printer::Instance().AddMessage(_kNoActionText);
    return;
  }

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

    ItemBonusStruct b;
    b.Type = ItemBonusType::MANA_SHIELD;
    b.Id = scroll->OwnerGameObject->ObjectId();

    _playerRef->AddEffect(b);

    _playerRef->RememberItem(scroll, "mana shield");
  }
}

void SpellsProcessor::ProcessScrollOfRemoveCurse(ItemComponent* scroll)
{
  _playerRef->RememberItem(scroll, GlobalConstants::UnidentifiedEffectText);

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

      // No additional stat penalties, just can't uneqip

      auto& idName = item->Data.IdentifiedName;
      idName = Util::ReplaceItemPrefix(idName, { "Blessed", "Uncursed" }, "Cursed");

      Printer::Instance().AddMessage("The malevolent energy creeps in...");

      _playerRef->RememberItem(scroll, "remove curse");
    }
    else
    {
      Printer::Instance().AddMessage(_kNoActionText);
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

      auto& idName = item->Data.IdentifiedName;
      idName = Util::ReplaceItemPrefix(idName, { "Cursed" }, "Uncursed");

      Printer::Instance().AddMessage("The malevolent energy disperses!");

      _playerRef->RememberItem(scroll, "remove curse");
    }
    else
    {
      Printer::Instance().AddMessage(_kNoActionText);
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

        auto& idName = ic->Data.IdentifiedName;
        idName = Util::ReplaceItemPrefix(idName, { "Cursed" }, "Uncursed");
      }
    }

    if (success)
    {
      Printer::Instance().AddMessage("The malevolent energy disperses completely!");

      _playerRef->RememberItem(scroll, "remove curse");
    }
    else
    {
      Printer::Instance().AddMessage(_kNoActionText);
    }
  }
}

void SpellsProcessor::ProcessScrollOfEnchantment(ItemComponent* scroll)
{
  // TODO:

  _playerRef->RememberItem(scroll, GlobalConstants::UnidentifiedEffectText);

  switch (scroll->Data.Prefix)
  {
    case ItemPrefix::CURSED:
      break;

    case ItemPrefix::UNCURSED:
      break;

    case ItemPrefix::BLESSED:
      break;
  }

  _playerRef->RememberItem(scroll, "enchant");
}
