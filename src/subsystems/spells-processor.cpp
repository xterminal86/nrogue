#include "spells-processor.h"

#include "application.h"
#include "printer.h"
#include "map.h"
#include "map-level-town.h"
#include "town-portal-component.h"
#include "game-objects-factory.h"

void SpellsProcessor::Init()
{
  if (_initialized)
  {
    return;
  }

  _playerRef = &Game::gApp.PlayerInstance;

  _initialized = true;
}

// =============================================================================

void SpellsProcessor::ProcessWand(ItemComponent* wand)
{
  Game::gPrnt.AddMessage("You invoke the wand...");

  wand->Data.Amount--;

  switch (wand->Data.SpellHeld.SpellType_)
  {
    case SpellType::LIGHT:
      ProcessWandOfLight(wand);
      break;

    default:
      Game::gPrnt.AddMessage(_kNoActionText);
      break;
  }
}

// =============================================================================

void SpellsProcessor::ProcessWandOfLight(ItemComponent* wand)
{
  int radius = Game::gMap.CurrentLevel->VisibilityRadius;

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

  Game::gPrnt.AddMessage(message);

  ItemBonusStruct b;

  b.Type       = ItemBonusType::ILLUMINATED;
  b.BonusValue = power;
  b.Duration   = duration;
  b.Id         = wand->OwnerGameObject->ObjectId();

  _playerRef->AddEffect(b);
}

// =============================================================================

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
      ProcessScrollOfHiddenDetection(scroll, user, ItemBonusType::TELEPATHY);
      break;

    case SpellType::TRUE_SEEING:
      ProcessScrollOfHiddenDetection(scroll, user, ItemBonusType::TRUE_SEEING);
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
      DebugLog("[WAR] SpellsProcessor::ProcessScroll() spell %d not handled!",
               (int)scroll->Data.SpellHeld.SpellType_);
      _scrollUseMessages.push_back(_kNoActionText);
      break;
  }

  PrintUsageResult(scroll, user);
}

// =============================================================================

void SpellsProcessor::PrintUsageResult(ItemComponent* scroll, GameObject* user)
{
  if (Util::IsPlayer(user))
  {
    std::string scrollName = scroll->Data.IsIdentified ?
                             scroll->Data.IdentifiedName :
                             scroll->Data.UnidentifiedName;

    Game::gPrnt.AddMessage(
      Util::StringFormat("You read the scroll %s...", scrollName.data())
    );

    for (auto& msg : _scrollUseMessages)
    {
      Game::gPrnt.AddMessage(msg);
    }

    Game::gPrnt.AddMessage("The scroll crumbles to dust");
  }
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfRepair(ItemComponent* scroll,
                                            GameObject* user)
{
  //
  // TODO: should monsters be able to use this?
  //
  if (!Util::IsPlayer(user))
  {
    return;
  }

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
    int index = Game::gRng.RandomRange(0, itemsToRepair.size());
    ItemComponent* item = itemsToRepair[index];

    auto str = Util::StringFormat("Your %s disintegrates!",
                                  item->OwnerGameObject->ObjectName.data());
    _scrollUseMessages.push_back(str);

    item->Break(_playerRef);
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    auto str = Util::StringFormat(
                 "Your %s is repaired!",
                 itemsToRepair[0]->OwnerGameObject->ObjectName.data()
    );
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
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfIdentify(ItemComponent* scroll,
                                              GameObject* user)
{
  //
  // Monsters dont't use this.
  //
  if (!Util::IsPlayer(user))
  {
    return;
  }

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

    int index = Game::gRng.RandomRange(0, itemsKnown.size());
    auto item = itemsKnown[index];
    item->Data.IsIdentified = false;
    item->Data.IsPrefixDiscovered = false;

    _scrollUseMessages.push_back(
      Util::StringFormat("You forget the details about %s!",
                         item->OwnerGameObject->ObjectName.data())
    );
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    if (itemsToId.empty())
    {
      _scrollUseMessages.push_back(_kNoActionText);
      return;
    }

    _scrollUseMessages.push_back(
      Util::StringFormat("%s has been identified!",
                         itemsToId[0]->OwnerGameObject->ObjectName.data())
    );

    itemsToId[0]->Data.IsPrefixDiscovered = true;
    itemsToId[0]->Data.IsIdentified = true;
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
  }
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfNeutralizePoison(ItemComponent* scroll,
                                                      GameObject* user)
{
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
  }
  else
  {
    if (!user->HasEffect(ItemBonusType::POISONED))
    {
      _scrollUseMessages.push_back(_kNoActionText);
      return;
    }

    user->DispelEffectFirstFound(ItemBonusType::POISONED);

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
  }
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfHealing(ItemComponent* scroll,
                                             GameObject* user)
{
  int power = user->Attrs.HP.Max().Get();
  bool atFullHealth =
      (user->Attrs.HP.Min().Get() == user->Attrs.HP.Max().Get());

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    power = Game::gRng.RandomRange(1, user->Attrs.HP.Min().Get() / 2);
    power = -power;
    _scrollUseMessages.push_back("You writhe in pain!");
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
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfLight(ItemComponent* scroll,
                                           GameObject* user)
{
  int radius = Game::gMap.CurrentLevel->VisibilityRadius;

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
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfMM(ItemComponent* scroll, GameObject* user)
{
  //
  // Pointless for anyone but the player.
  //
  if (!Util::IsPlayer(user))
  {
    return;
  }

  // NOTE: blessed scroll reveals traps as well (level explored percentage?)

  auto& mapRef = Game::gMap.CurrentLevel;

  if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    for (int x = 0; x < mapRef->MapSize.X; x++)
    {
      for (int y = 0; y < mapRef->MapSize.Y; y++)
      {
        mapRef->MapArray[x][y]->Revealed = false;
      }
    }

    Game::gMap.CurrentLevel->ExitFound = false;

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

    Game::gMap.CurrentLevel->ExitFound = true;

    _scrollUseMessages.push_back("A map coalesces in your mind!");

    if (scroll->Data.Prefix == ItemPrefix::BLESSED)
    {
      // TODO: reveal traps

      std::vector<std::string> monstersOnLevel;

      for (auto& actor : mapRef->ActorGameObjects)
      {
        monstersOnLevel.push_back(
          Util::StringFormat("You sense: %s (danger %d)",
                             actor->ObjectName.data(),
                             actor->Attrs.Rating())
        );
      }

      for (auto& str : monstersOnLevel)
      {
        _scrollUseMessages.push_back(str);
      }
    }
  }
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfHiddenDetection(ItemComponent* scroll,
                                                     GameObject* user,
                                                     ItemBonusType type)
{
  //
  // TODO: possible application for monsters?
  //
  if (!Util::IsPlayer(user))
  {
    return;
  }

  bool isValidType = (type == ItemBonusType::TELEPATHY
                   || type == ItemBonusType::TRUE_SEEING);

  if (!isValidType)
  {
    DebugLog("[WAR] ProcessScrollOfDetectMonsters() type is wrong: %d!",
             (int)type);
    type = ItemBonusType::TELEPATHY;
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
  b.Type       = type;

  // Power is unused in this effect (see Map::DrawActors())
  b.BonusValue = power;

  b.Duration   = duration;
  b.Id         = scroll->OwnerGameObject->ObjectId();

  _playerRef->AddEffect(b);
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfTownPortal(ItemComponent* scroll,
                                                GameObject* user)
{
  //
  // Monsters dont't use this
  //
  if (!Util::IsPlayer(user))
  {
    return;
  }

  if (Game::gMap.CurrentLevel->MysteriousForcePresent
   || Game::gMap.CurrentLevel->MapType_ == MapType::TOWN)
  {
    _scrollUseMessages.push_back(_kNoActionText);
    return;
  }

  auto p = Game::gMap.GetLevelRefByType(MapType::TOWN);
  MapLevelTown* lvl = static_cast<MapLevelTown*>(p);

  int ind = -1;

  for (size_t i = 0; i < lvl->GameObjects.size(); i++)
  {
    TownPortalComponent* tpc =
        lvl->GameObjects[i]->GetComponent<TownPortalComponent>();

    if (tpc != nullptr)
    {
      ind = i;
      break;
    }
  }

  if (ind != -1)
  {
    lvl->GameObjects.erase(lvl->GameObjects.begin() + ind);
  }

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

    int index = Game::gRng.RandomRange(0, posToAppear.size());
    Position res = posToAppear[index];

    GameObject* portal = new GameObject(lvl,
                                        tpPos.X,
                                        tpPos.Y,
                                        '0',
                                        Colors::WhiteColor,
                                        Colors::BlueColor);

    portal->BlocksSight = true;
    portal->ObjectName = "Town Portal";

    TownPortalComponent* tpc = portal->AddComponent<TownPortalComponent>();
    tpc->SavePosition(Game::gMap.CurrentLevel->MapType_,
                      _playerRef->GetPosition());

    lvl->PlaceGameObject(portal);

    _scrollUseMessages.push_back(Strings::MsgTeleportBackInTown);
    Game::gMap.TeleportToExistingLevel(MapType::TOWN, res);
  }
  else if (scroll->Data.Prefix == ItemPrefix::UNCURSED)
  {
    _scrollUseMessages.push_back(Strings::MsgTeleportBackInTown);
    Game::gMap.TeleportToExistingLevel(MapType::TOWN,
                                            lvl->TownPortalPos());
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    auto& mapRef = Game::gMap.CurrentLevel;
    int index = Game::gRng.RandomRange(0, mapRef->EmptyCells().size());
    auto pos = mapRef->EmptyCells()[index];
    auto str = Game::gMap.TeleportToExistingLevel(mapRef->MapType_, pos);

    _scrollUseMessages.push_back(Strings::MsgTeleportElsewhere);

    if (!str.empty())
    {
      _scrollUseMessages.push_back(str);
    }
  }
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfTeleport(ItemComponent* scroll,
                                              GameObject* user)
{
  if (Game::gMap.CurrentLevel->MysteriousForcePresent)
  {
    if (Util::IsPlayer(user))
    {
      _scrollUseMessages.push_back(_kNoActionText);
    }

    return;
  }

  _scrollUseMessages.push_back(Strings::MsgTeleportElsewhere);

  auto& mapRef = Game::gMap.CurrentLevel;

  // TODO: blessed scroll of teleport - what positive effect for player?

  if (scroll->Data.Prefix == ItemPrefix::UNCURSED
   || scroll->Data.Prefix == ItemPrefix::BLESSED)
  {
    int index = Game::gRng.RandomRange(0, mapRef->EmptyCells().size());
    auto pos = mapRef->EmptyCells()[index];
    Game::gMap.TeleportToExistingLevel(mapRef->MapType_, pos, user);
  }
  else if (scroll->Data.Prefix == ItemPrefix::CURSED)
  {
    int rx = Game::gRng.RandomRange(1, mapRef->MapSize.X);
    int ry = Game::gRng.RandomRange(1, mapRef->MapSize.Y);
    Position pos = { rx, ry };
    auto str = Game::gMap.TeleportToExistingLevel(mapRef->MapType_, pos, user);
    if (!str.empty())
    {
      _scrollUseMessages.push_back(str);
    }
  }
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfManaShield(ItemComponent *scroll,
                                                GameObject* user)
{
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
  }
}

// =============================================================================

void SpellsProcessor::ProcessScrollOfRemoveCurse(ItemComponent* scroll,
                                                 GameObject* user)
{
  if (!Util::IsPlayer(user))
  {
    return;
  }

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
      int index = Game::gRng.RandomRange(0, nonCursedItems.size());
      ItemComponent* item = nonCursedItems[index];
      item->Data.IsPrefixDiscovered = true;
      Util::UpdateItemPrefix(item, ItemPrefix::CURSED);

      //
      // No additional stat penalties, just can't uneqip.
      //
      auto& idName = item->Data.IdentifiedName;
      idName = Util::ReplaceItemPrefix(idName,
                                       { "Blessed", "Uncursed" },
                                       "Cursed");

      _scrollUseMessages.push_back("The malevolent energy creeps in...");
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
      int index = Game::gRng.RandomRange(0, cursedItems.size());
      ItemComponent* item = cursedItems[index];
      item->Data.IsPrefixDiscovered = true;
      Util::UpdateItemPrefix(item, ItemPrefix::UNCURSED);

      auto& idName = item->Data.IdentifiedName;
      idName = Util::ReplaceItemPrefix(idName, { "Cursed" }, "Uncursed");

      _scrollUseMessages.push_back("The malevolent energy disperses!");
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
        Util::UpdateItemPrefix(ic, ItemPrefix::UNCURSED);

        success = true;

        auto& idName = ic->Data.IdentifiedName;
        idName = Util::ReplaceItemPrefix(idName, { "Cursed" }, "Uncursed");
      }
    }

    if (success)
    {
      _scrollUseMessages.push_back(
        "The malevolent energy disperses completely!"
      );
    }
    else
    {
      _scrollUseMessages.push_back(_kNoActionText);
    }
  }
}
