#include "shrine-component.h"

#include "game-object.h"
#include "util.h"
#include "application.h"
#include "printer.h"
#include "game-objects-factory.h"
#include "map.h"

ShrineComponent::ShrineComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void ShrineComponent::Update()
{
  if (Counter < Timeout)
  {
    Counter++;
  }

  if (Counter == Timeout)
  {
    // So that we won't go into this block again
    Counter++;

    Activate();
  }
}

void ShrineComponent::Interact()
{
  if (Counter < Timeout)
  {
    Printer::Instance().AddMessage("Shrine is inactive");
  }
  else
  {
    int dungLvl = Map::Instance().CurrentLevel->DungeonLevel;

    _power    = dungLvl; //Timeout / 100;
    _duration = ((float)(dungLvl * 10) * 1.25f); //Timeout / 2;

    Counter = 0;
    OwnerGameObject->FgColor = GlobalConstants::BlackColor;

    std::string shrineName = GlobalConstants::ShrineNameByType.at(Type);
    std::string message = Util::StringFormat("You touch the %s...", shrineName.data());

    if (GlobalConstants::ShrineSaintByType.count(Type) == 1)
    {
      std::string saint = GlobalConstants::ShrineSaintByType.at(Type);
      message = Util::StringFormat("You pray to %s...", saint.data());
    }

    Printer::Instance().AddMessage(message);

    ProcessEffect();
  }
}

void ShrineComponent::Activate()
{
  OwnerGameObject->FgColor = GlobalConstants::ShrineColorsByType.at(Type).first;
}

void ShrineComponent::ProcessEffect()
{
  auto& playerRef = Application::Instance().PlayerInstance;

  std::string msg = "...but nothing happens";

  switch (Type)
  {
    // raises STR or HP
    case ShrineType::MIGHT:
    {
      if (Util::Rolld100(50))
      {        
        playerRef.Attrs.Str.Add(1);        
        msg = "You feel stronger!";
      }
      else
      {
        int hpToAdd = 2 * (playerRef.Attrs.HP.Talents + 1);
        playerRef.Attrs.HP.AddMax(hpToAdd);
        msg = "You feel tougher!";
      }
    }
    break;

    // raises MAG or MP
    case ShrineType::SPIRIT:
    {
      if (Util::Rolld100(50))
      {        
        playerRef.Attrs.Mag.Add(1);
        msg = "You feel strong-willed!";
      }
      else
      {
        int mpToAdd = 2 * (playerRef.Attrs.MP.Talents + 1);
        playerRef.Attrs.MP.AddMax(mpToAdd);
        msg = "You spirituality has increased!";
      }
    }
    break;

    // restores MP
    case ShrineType::TRANQUILITY:
    {
      if (playerRef.Attrs.MP.Max().OriginalValue() != 0)
      {
        playerRef.Attrs.MP.Restore();
        msg = "Your spirit force is restored!";
      }
    }
    break;

    // identifies items
    case ShrineType::KNOWLEDGE:
    {
      bool success = false;
      for (auto& i : playerRef.Inventory.Contents)
      {
        auto c = i->GetComponent<ItemComponent>();
        ItemComponent* ic = static_cast<ItemComponent*>(c);
        if (!ic->Data.IsIdentified)
        {
          ic->Data.IsIdentified = true;
          success = true;
        }
      }

      if (success)
      {
        msg = "Your possessions have been identified!";
      }
    }
    break;

    // reveals BUC status
    case ShrineType::PERCEPTION:
    {
      bool success = false;
      for (auto& i : playerRef.Inventory.Contents)
      {
        auto c = i->GetComponent<ItemComponent>();
        ItemComponent* ic = static_cast<ItemComponent*>(c);
        if (!ic->Data.IsPrefixDiscovered)
        {
          ic->Data.IsPrefixDiscovered = true;
          success = true;
        }
      }

      if (success)
      {
        msg = "Your possessions alignment has been revealed!";
      }
    }
    break;

    // restores HP
    case ShrineType::HEALING:
    {      
      playerRef.Attrs.HP.Restore();
      msg = "You feel better!";
    }
    break;

    // random effect + receive curse
    case ShrineType::DESECRATED:
    {
      ApplyRandomEffect(msg);

      std::vector<ItemComponent*> itemsToCurse;
      for (auto& i : playerRef.Inventory.Contents)
      {
        auto c = i->GetComponent<ItemComponent>();
        ItemComponent* ic = static_cast<ItemComponent*>(c);
        if (ic->Data.Prefix != ItemPrefix::CURSED)
        {
          itemsToCurse.push_back(ic);
        }
      }

      if (!itemsToCurse.empty())
      {
        int index = RNG::Instance().RandomRange(0, itemsToCurse.size());
        itemsToCurse[index]->Data.Prefix = ItemPrefix::CURSED;
        msg = "You sense the malevolent energy";
      }
    }
    break;

    // random effect + low chance to receive negative effect
    case ShrineType::RUINED:
    {
      ApplyRandomEffect(msg);

      if (Util::Rolld100(25))
      {
        ApplyRandomNegativeEffect(msg);
      }
    }
    break;

    // random effect + high chance to receive negative effect
    case ShrineType::DISTURBING:
    {
      ApplyRandomEffect(msg);

      if (Util::Rolld100(75))
      {
        ApplyRandomNegativeEffect(msg);
      }
    }
    break;

    // random stat raise + negative effect
    case ShrineType::ABYSSAL:
    {
      std::map<int, Attribute&> playerStatsRef =
      {
        { 0, playerRef.Attrs.Str },
        { 1, playerRef.Attrs.Def },
        { 2, playerRef.Attrs.Mag },
        { 3, playerRef.Attrs.Res },
        { 4, playerRef.Attrs.Skl },
        { 5, playerRef.Attrs.Spd }
      };

      int index = RNG::Instance().RandomRange(0, playerStatsRef.size());
      auto it = playerStatsRef.begin();
      std::advance(it, index);
      it->second.Add(1);

      ApplyRandomNegativeEffect(msg);
    }
    break;

    // random effect
    case ShrineType::FORGOTTEN:
      ApplyRandomEffect(msg);
      break;

    // temporary raises stats
    case ShrineType::POTENTIAL:
      ApplyTemporaryStatRaise(msg);
      break;

    // random effect
    case ShrineType::HIDDEN:
      ApplyRandomEffect(msg);
      break;

    // removes curse
    case ShrineType::HOLY:
    {
      bool success = false;
      for (auto& i : playerRef.Inventory.Contents)
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
        msg = "The malevolent energy disperses!";
      }
    }
    break;
  }

  Printer::Instance().AddMessage(msg);
}

void ShrineComponent::ApplyRandomEffect(std::string& logMessageToWrite)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int effectIndex = RNG::Instance().RandomRange(0, GlobalConstants::BonusDisplayNameByType.size());
  auto it = GlobalConstants::BonusDisplayNameByType.begin();
  std::advance(it, effectIndex);

  ItemBonusStruct b;

  b.Type       = it->first;
  b.BonusValue = _power;
  b.Duration   = _duration;
  b.Id         = OwnerGameObject->ObjectId();

  if (b.Type == ItemBonusType::REGEN
   || b.Type == ItemBonusType::POISONED
   || b.Type == ItemBonusType::PARALYZE
   || b.Type == ItemBonusType::BURNING)
  {
    b.Cumulative = true;
    b.BonusValue = (b.Type == ItemBonusType::POISONED
                 || b.Type == ItemBonusType::BURNING) ? -1 : 1;
    b.Duration   = _duration;
    b.Period     = (b.Type == ItemBonusType::POISONED) ? 2 : 1;
  }
  else if (b.Type == ItemBonusType::BLINDNESS)
  {
    b.BonusValue = -_power;
  }

  playerRef.AddEffect(b);

  SetEffectGainMessage(logMessageToWrite, b.Type);
}

void ShrineComponent::ApplyRandomPositiveEffect(std::string& logMessageToWrite)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int effectIndex = RNG::Instance().RandomRange(0, PositiveEffects.size());
  ItemBonusType t = PositiveEffects[effectIndex];

  ItemBonusStruct b;

  b.Type       = t;
  b.BonusValue = _power;
  b.Duration   = _duration;
  b.Id         = OwnerGameObject->ObjectId();

  if (t == ItemBonusType::MANA_SHIELD && playerRef.Attrs.MP.Max().Get() != 0)
  {
    b.Duration = -1;
  }
  else if (t == ItemBonusType::REGEN)
  {
    b.Cumulative = true;
    b.BonusValue = 1;
    b.Period     = 1;
  }

  playerRef.AddEffect(b);

  SetEffectGainMessage(logMessageToWrite, b.Type);
}

void ShrineComponent::ApplyRandomNegativeEffect(std::string& logMessageToWrite)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int effectIndex = RNG::Instance().RandomRange(0, NegativeEffects.size());  
  ItemBonusType t = NegativeEffects[effectIndex];

  ItemBonusStruct b;

  b.Type       = t;
  b.BonusValue = _power;
  b.Duration   = _duration;
  b.Id         = OwnerGameObject->ObjectId();

  if (t == ItemBonusType::BLINDNESS)
  {
    b.BonusValue = -_power;
  }

  if (t == ItemBonusType::POISONED
   || t == ItemBonusType::PARALYZE
   || t == ItemBonusType::BURNING)
  {
    b.Cumulative = true;
    b.BonusValue = (b.Type == ItemBonusType::POISONED
                 || b.Type == ItemBonusType::BURNING) ? -1 : 1;
    b.Period = (b.Type == ItemBonusType::POISONED) ? 2 : 1;
  }

  playerRef.AddEffect(b);

  SetEffectGainMessage(logMessageToWrite, b.Type);
}

void ShrineComponent::ApplyTemporaryStatRaise(std::string& logMessageToWrite)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  std::vector<ItemBonusType> attrs =
  {
    { ItemBonusType::STR },
    { ItemBonusType::DEF },
    { ItemBonusType::MAG },
    { ItemBonusType::RES },
    { ItemBonusType::SPD },
    { ItemBonusType::SKL }
  };

  int index = RNG::Instance().RandomRange(0, attrs.size());

  ItemBonusStruct bs;

  bs.Type       = attrs[index];
  bs.Duration   = 30 + _duration;
  bs.BonusValue = 1;
  bs.Id         = OwnerGameObject->ObjectId();
  bs.Cumulative = true;

  playerRef.AddEffect(bs);

  SetEffectGainMessage(logMessageToWrite, bs.Type);
}

void ShrineComponent::SetEffectGainMessage(std::string& logMessageToWrite,
                                           ItemBonusType e)
{
  switch (e)
  {
    case ItemBonusType::STR:
      logMessageToWrite = "You feel stronger!";
      break;

    case ItemBonusType::DEF:
      logMessageToWrite = "Your skin hardens!";
      break;

    case ItemBonusType::MAG:
      logMessageToWrite = "Your spirit is invigorated!";
      break;

    case ItemBonusType::RES:
      logMessageToWrite = "You don't fear magic anymore!";
      break;

    case ItemBonusType::SPD:
      logMessageToWrite = "Your moves became quicker!";
      break;

    case ItemBonusType::SKL:
      logMessageToWrite = "Your melee skills increased!";
      break;

    case ItemBonusType::MANA_SHIELD:
      logMessageToWrite = "Spirit force is guarding you";
      break;

    case ItemBonusType::ILLUMINATED:
      logMessageToWrite = "The darkness disperses!";
      break;

    case ItemBonusType::REGEN:
      logMessageToWrite = "Your wounds heal faster";
      break;

    case ItemBonusType::REFLECT:
      logMessageToWrite = "Magic spells will be reflected";
      break;

    case ItemBonusType::TELEPATHY:
      logMessageToWrite = "You can sense others";
      break;

    case ItemBonusType::LEVITATION:
      logMessageToWrite = "You float above ground!";
      break;

    case ItemBonusType::PARALYZE:
      logMessageToWrite = "You can't move!";
      break;

    case ItemBonusType::POISONED:
      logMessageToWrite = "You are feeling unwell...";
      break;

    case ItemBonusType::BURNING:
      logMessageToWrite = "You catch fire!";
      break;

    case ItemBonusType::FROZEN:
      logMessageToWrite = "You are shackled by cold!";
      break;

    case ItemBonusType::BLINDNESS:
      logMessageToWrite = "You can't see!";
      break;

    case ItemBonusType::INVISIBILITY:
      logMessageToWrite = "You can see through yourself!";
      break;
  }
}
