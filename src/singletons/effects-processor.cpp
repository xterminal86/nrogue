#include "effects-processor.h"

#include "application.h"
#include "printer.h"
#include "map.h"

void EffectsProcessor::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void EffectsProcessor::ProcessWand(ItemComponent* wand)
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

void EffectsProcessor::ProcessScroll(ItemComponent* scroll)
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

    default:
      Printer::Instance().AddMessage("...but nothing happens.");
      break;
  }

  Printer::Instance().AddMessage("The scroll crumbles to dust");
}

void EffectsProcessor::ProcessScrollOfRepair(ItemComponent* scroll)
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

void EffectsProcessor::ProcessScrollOfIdentify(ItemComponent* scroll)
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

void EffectsProcessor::ProcessScrollOfNeutralizePoison(ItemComponent* scroll)
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

void EffectsProcessor::ProcessScrollOfHealing(ItemComponent* scroll)
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

void EffectsProcessor::ProcessWandOfLight(ItemComponent* wand)
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

  IlluminatePlayer(power, duration);
}

void EffectsProcessor::ProcessScrollOfMM(ItemComponent* scroll)
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

    Printer::Instance().AddMessage("You suddenly forgot where you are!");
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

    Printer::Instance().AddMessage("The map coalesces in your mind!");

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

void EffectsProcessor::ProcessScrollOfLight(ItemComponent* scroll)
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

  IlluminatePlayer(power, duration);
}

void EffectsProcessor::IlluminatePlayer(int pow, int dur)
{
  _playerRef->AddEffect(EffectType::ILLUMINATED, pow, dur, false, true);
}
