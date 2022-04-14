#include "spells-processor.h"

#include "application.h"
#include "printer.h"
#include "map.h"
#include "map-level-town.h"
#include "town-portal-component.h"
#include "game-objects-factory.h"

void SpellsProcessor::InitSpecific()
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

void SpellsProcessor::ProcessScroll(ItemComponent* scroll, GameObject* user)
{
  _scrollUseMessages.clear();

  switch(scroll->Data.SpellHeld.SpellType_)
  {
    case SpellType::LIGHT:
      ProcessScrollOfLight(scroll, user);
      break;

    case SpellType::MAGIC_MAPPING:
      ProcessScrollOfMM(scroll, user);
      break;

    case SpellType::HEAL:
      ProcessScrollOfHealing(scroll, user);
      break;

    case SpellType::NEUTRALIZE_POISON:
      ProcessScrollOfNeutralizePoison(scroll, user);
      break;

    case SpellType::IDENTIFY:
      ProcessScrollOfIdentify(scroll, user);
      break;

    case SpellType::REPAIR:
      ProcessScrollOfRepair(scroll, user);
      break;

    case SpellType::DETECT_MONSTERS:
      ProcessScrollOfDetectMonsters(scroll, user);
      break;

    case SpellType::TOWN_PORTAL:
      ProcessScrollOfTownPortal(scroll, user);
      break;

    case SpellType::TELEPORT:
      ProcessScrollOfTeleport(scroll, user);
      break;

    case SpellType::MANA_SHIELD:
      ProcessScrollOfManaShield(scroll, user);
      break;

    case SpellType::REMOVE_CURSE:
      ProcessScrollOfRemoveCurse(scroll, user);
      break;

    default:
      DebugLog("[WAR] SpellsProcessor::ProcessScroll() spell %i not handled!", (int)scroll->Data.SpellHeld.SpellType_);
      _scrollUseMessages.push_back(_kNoActionText);
      break;
  }

  PrintUsageResult(scroll, user);
}

void SpellsProcessor::PrintUsageResult(ItemComponent* scroll, GameObject* user)
{
  if (user == _playerRef)
  {
    std::string scrollName = scroll->Data.IsIdentified ?
                             scroll->Data.IdentifiedName :
                             scroll->Data.UnidentifiedName;

    auto str = Util::StringFormat("You read the scroll %s...", scrollName.data());

    Printer::Instance().AddMessage(str);

    for (auto& msg : _scrollUseMessages)
    {
      Printer::Instance().AddMessage(msg);
    }

    Printer::Instance().AddMessage("The scroll crumbles to dust");
  }
}

void SpellsProcessor::ProcessScrollOfRepair(ItemComponent* scroll, GameObject* user)
{
  //
  // TODO: should monsters be able to use this?
  //
  if (!Util::IsPlayer(user))
  {
    return;
  }

  _playerRef->RememberItem(scroll, Strings::UnidentifiedEffectText);

  std::vector<ItemComponent*> itemsToRepair;
  for (auto& i : _playerRef->Inventory->Contents)
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
    _scrollUseMessages.push_back(_kNoActionText);
    return;
  }

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    int index = RNG::Instance().RandomRange(0, itemsToRepair.size());
    ItemComponent* item = itemsToRepair[index];

    item->Break(_playerRef);
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    auto str = Util::StringFormat("Your %s is repaired!", itemsToRepair[0]->OwnerGameObject->ObjectName.data());
    _scrollUseMessages.push_back(str);

    itemsToRepair[0]->Data.Durability.Restore();
  }
  else if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    _scrollUseMessages.push_back("Your items have been repaired!");

    for (auto& i : itemsToRepair)
    {
      i->Data.Durability.Restore();
    }
  }

  _playerRef->RememberItem(scroll, "repair");
}

void SpellsProcessor::ProcessScrollOfIdentify(ItemComponent* scroll, GameObject* user)
{
  if (!Util::IsPlayer(user))
  {
    return;
  }

  // TODO: identify control

  _playerRef->RememberItem(scroll, Strings::UnidentifiedEffectText);

  std::vector<ItemComponent*> itemsToId;
  std::vector<ItemComponent*> itemsKnown;
  for (auto& i : _playerRef->Inventory->Contents)
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
      _scrollUseMessages.push_back(_kNoActionText);
      return;
    }

    _scrollUseMessages.push_back("You forget the details about inventory item!");

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
      _scrollUseMessages.push_back(_kNoActionText);
      return;
    }

    _scrollUseMessages.push_back("Inventory item has been identified!");

    itemsToId[0]->Data.IsPrefixDiscovered = true;
    itemsToId[0]->Data.IsIdentified = true;

    _playerRef->RememberItem(scroll, "identify");
  }
  else if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    if (itemsToId.empty())
    {
      _scrollUseMessages.push_back(_kNoActionText);
      return;
    }

    for (auto& i : itemsToId)
    {
      i->Data.IsPrefixDiscovered = true;
      i->Data.IsIdentified = true;
    }

    _scrollUseMessages.push_back("Your possessions have been identified!");

    _playerRef->RememberItem(scroll, "identify");
  }
}

void SpellsProcessor::ProcessScrollOfNeutralizePoison(ItemComponent* scroll, GameObject* user)
{
  if (Util::IsPlayer(user))
  {
    _playerRef->RememberItem(scroll, Strings::UnidentifiedEffectText);
  }

  auto RememberScroll = [this, user, scroll]()
  {
    if (Util::IsPlayer(user))
    {
      _playerRef->RememberItem(scroll, "neutralize poison");
    }
  };

  int userPow = user->Attrs.Mag.Get();

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    ItemBonusStruct b;
    b.Type = ItemBonusType::POISONED;
    b.BonusValue = -1;
    b.Period = 10;
    b.Duration = GlobalConstants::EffectDefaultDuration;
    b.Cumulative = true;
    b.Id = scroll->OwnerGameObject->ObjectId();

    user->AddEffect(b);

    _scrollUseMessages.push_back("You feel unwell!");

    RememberScroll();
  }
  else
  {
    if (!user->HasEffect(ItemBonusType::POISONED))
    {
      _scrollUseMessages.push_back(_kNoActionText);
      return;
    }

    user->DispelEffect(ItemBonusType::POISONED);

    _scrollUseMessages.push_back("You feel better");

    if (scroll->Data.Prefix == ItemPrefix::BLESSED)
    {
      ItemBonusStruct b;
      b.Type = ItemBonusType::REGEN;
      b.BonusValue = 1;
      b.Period = 10;
      b.Duration = (userPow <= 0) ?
                   GlobalConstants::EffectDefaultDuration :
                   GlobalConstants::EffectDefaultDuration * userPow;
      b.Cumulative = true;
      b.Id = scroll->OwnerGameObject->ObjectId();

      user->AddEffect(b);
    }

    RememberScroll();
  }
}

void SpellsProcessor::ProcessScrollOfHealing(ItemComponent* scroll, GameObject* user)
{
  if (Util::IsPlayer(user))
  {
    _playerRef->RememberItem(scroll, Strings::UnidentifiedEffectText);
  }

  auto RememberScroll = [this, user, scroll]()
  {
    if (Util::IsPlayer(user))
    {
      _playerRef->RememberItem(scroll, "healing");
    }
  };

  int power = user->Attrs.HP.Max().Get();
  bool atFullHealth = (user->Attrs.HP.Min().Get() == user->Attrs.HP.Max().Get());

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    power = RNG::Instance().RandomRange(1, user->Attrs.HP.Min().Get() / 2);
    power = -power;
    _scrollUseMessages.push_back("You writhe in pain!");

    RememberScroll();
  }
  else
  {
    if (atFullHealth)
    {
      _scrollUseMessages.push_back(_kNoActionText);
      return;
    }

    if (scroll->Data.Prefix == ItemPrefix::BLESSED)
    {
      _scrollUseMessages.push_back("Your wounds are healed completely!");
    }
    else
    {
      power = power / 2;
      _scrollUseMessages.push_back("You feel better");
    }
  }

  user->Attrs.HP.AddMin(power);

  RememberScroll();
}

void SpellsProcessor::ProcessScrollOfLight(ItemComponent* scroll, GameObject* user)
{
  auto RememberScroll = [this, scroll, user]()
  {
    if (Util::IsPlayer(user))
    {
      _playerRef->RememberItem(scroll, "light");
    }
  };

  int radius = Map::Instance().CurrentLevel->VisibilityRadius;

  int playerPow = user->Attrs.Mag.Get();
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

  _scrollUseMessages.push_back(message);

  ItemBonusStruct b;

  b.Type       = ItemBonusType::ILLUMINATED;
  b.BonusValue = power;
  b.Duration   = duration;
  b.Id         = scroll->OwnerGameObject->ObjectId();

  user->AddEffect(b);

  RememberScroll();
}

void SpellsProcessor::ProcessScrollOfMM(ItemComponent* scroll, GameObject* user)
{
  //
  // Pointless for anyone but the player
  //
  if (!Util::IsPlayer(user))
  {
    return;
  }

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

    _scrollUseMessages.push_back("You suddenly forget where you are!");
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

    _scrollUseMessages.push_back("A map coalesces in your mind!");

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
        _scrollUseMessages.push_back(str);
      }
    }
  }

  _playerRef->RememberItem(scroll, "magic mapping");
}

void SpellsProcessor::ProcessScrollOfDetectMonsters(ItemComponent* scroll, GameObject* user)
{
  //
  // TODO: possible application for monsters?
  //
  if (!Util::IsPlayer(user))
  {
    return;
  }

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

  _scrollUseMessages.push_back("You can sense nearby creatures");

  ItemBonusStruct b;
  b.Type = ItemBonusType::TELEPATHY;
  b.BonusValue = power; // TODO: power is unused in this effect (see Map::DrawActors())
  b.Duration = duration;
  b.Id = scroll->OwnerGameObject->ObjectId();

  _playerRef->AddEffect(b);

  _playerRef->RememberItem(scroll, "detect monsters");
}

void SpellsProcessor::ProcessScrollOfTownPortal(ItemComponent* scroll, GameObject* user)
{
  if (!Util::IsPlayer(user))
  {
    return;
  }

  _playerRef->RememberItem(scroll, Strings::UnidentifiedEffectText);

  auto p = Map::Instance().GetLevelRefByType(MapType::TOWN);
  MapLevelTown* lvl = static_cast<MapLevelTown*>(p);
  if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    auto tpPos = lvl->TownPortalPos();
    std::vector<Position> posToAppear =
    {
      { tpPos.X - 1, tpPos.Y     },
      { tpPos.X + 1, tpPos.Y     },
      { tpPos.X,     tpPos.Y - 1 },
      { tpPos.X,     tpPos.Y + 1 },
    };

    int index = RNG::Instance().RandomRange(0, posToAppear.size());
    Position res = posToAppear[index];

    GameObject* portal = new GameObject(lvl, tpPos.X, tpPos.Y, '0', "#FFFFFF", "#0000FF");

    portal->BlocksSight = true;
    portal->ObjectName = "Town Portal";

    TownPortalComponent* tpc = portal->AddComponent<TownPortalComponent>();
    tpc->SavePosition(Map::Instance().CurrentLevel->MapType_, _playerRef->GetPosition());

    lvl->PlaceGameObject(portal);

    _scrollUseMessages.push_back("You're back in town all of a sudden!");
    Map::Instance().TeleportToExistingLevel(MapType::TOWN, res);

    _playerRef->RememberItem(scroll, "town portal");
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    _scrollUseMessages.push_back("You're back in town all of a sudden!");
    Map::Instance().TeleportToExistingLevel(MapType::TOWN, lvl->TownPortalPos());

    _playerRef->RememberItem(scroll, "town portal");
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    auto& mapRef = Map::Instance().CurrentLevel;
    int index = RNG::Instance().RandomRange(0, mapRef->EmptyCells().size());
    auto pos = mapRef->EmptyCells()[index];
    Map::Instance().TeleportToExistingLevel(mapRef->MapType_, pos);

    _scrollUseMessages.push_back("You are suddenly transported elsewhere!");

    // Don't remember because it can be a scroll of teleport as well
  }
}

void SpellsProcessor::ProcessScrollOfTeleport(ItemComponent* scroll, GameObject* user)
{
  if (Util::IsPlayer(user))
  {
    _playerRef->RememberItem(scroll, Strings::UnidentifiedEffectText);
  }

  auto RememberScroll = [this, user, scroll]()
  {
    if (Util::IsPlayer(user))
    {
      _playerRef->RememberItem(scroll, "teleport");
    }
  };

  _scrollUseMessages.push_back("You are suddenly transported elsewhere!");

  auto& mapRef = Map::Instance().CurrentLevel;

  // TODO: blessed scroll of teleport - what to do?

  if (scroll->Data.Prefix == ItemPrefix::UNCURSED
   || scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    int index = RNG::Instance().RandomRange(0, mapRef->EmptyCells().size());
    auto pos = mapRef->EmptyCells()[index];
    Map::Instance().TeleportToExistingLevel(mapRef->MapType_, pos, user);

    RememberScroll();
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    int rx = RNG::Instance().RandomRange(1, mapRef->MapSize.X);
    int ry = RNG::Instance().RandomRange(1, mapRef->MapSize.Y);
    Position pos = { rx, ry };
    Map::Instance().TeleportToExistingLevel(mapRef->MapType_, pos, user);

    // Don't remember since it can be cursed scroll of TP
  }
}

void SpellsProcessor::ProcessScrollOfManaShield(ItemComponent *scroll, GameObject* user)
{
  if (Util::IsPlayer(user))
  {
    _playerRef->RememberItem(scroll, Strings::UnidentifiedEffectText);
  }

  auto RememberScroll = [this, user, scroll]()
  {
    if (Util::IsPlayer(user))
    {
      _playerRef->RememberItem(scroll, "mana shield");
    }
  };

  bool manaShieldOk = (user->Attrs.MP.Min().Get() != 0 ||
                      (user->Attrs.MP.Max().Get() != 0 &&
                       user->Attrs.Mag.Get() > 0));

  if (!manaShieldOk)
  {
    _scrollUseMessages.push_back(_kNoActionText);
    return;
  }

  if (user->Attrs.MP.Max().Get() > 0)
  {
    if (scroll->Data.Prefix == ItemPrefix::BLESSED)
    {
      user->Attrs.MP.Restore();
      _scrollUseMessages.push_back("Your spirit is reinforced!");
    }
    else if (scroll->Data.Prefix == ItemPrefix::CURSED)
    {
      int valueToAdd = user->Attrs.MP.Min().Get() / 2;
      user->Attrs.MP.AddMin(-valueToAdd);

      _scrollUseMessages.push_back("Your spirit force was drained!");
    }

    ItemBonusStruct b;
    b.Type = ItemBonusType::MANA_SHIELD;
    b.Id = scroll->OwnerGameObject->ObjectId();

    user->AddEffect(b);

    RememberScroll();
  }
}

void SpellsProcessor::ProcessScrollOfRemoveCurse(ItemComponent* scroll, GameObject* user)
{
  if (!Util::IsPlayer(user))
  {
    return;
  }

  _playerRef->RememberItem(scroll, Strings::UnidentifiedEffectText);

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    std::vector<ItemComponent*> nonCursedItems;
    for (auto& i : _playerRef->Inventory->Contents)
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

      _scrollUseMessages.push_back("The malevolent energy creeps in...");

      _playerRef->RememberItem(scroll, "remove curse");
    }
    else
    {
      _scrollUseMessages.push_back(_kNoActionText);
    }
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    std::vector<ItemComponent*> cursedItems;
    for (auto& i : _playerRef->Inventory->Contents)
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

      _scrollUseMessages.push_back("The malevolent energy disperses!");

      _playerRef->RememberItem(scroll, "remove curse");
    }
    else
    {
      _scrollUseMessages.push_back(_kNoActionText);
    }
  }
  else if (scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    bool success = false;
    for (auto& i : _playerRef->Inventory->Contents)
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
      _scrollUseMessages.push_back("The malevolent energy disperses completely!");

      _playerRef->RememberItem(scroll, "remove curse");
    }
    else
    {
      _scrollUseMessages.push_back(_kNoActionText);
    }
  }
}
