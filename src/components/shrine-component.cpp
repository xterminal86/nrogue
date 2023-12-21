#include "shrine-component.h"

#include "game-object.h"
#include "util.h"
#include "application.h"
#include "printer.h"
#include "game-objects-factory.h"
#include "map.h"

ShrineComponent::ShrineComponent(ShrineType shrineType, int timeout, bool oneTimeUse)
{
  _timeout = timeout;

  //
  // So that shrine can activate on Update()
  //
  _counter = timeout;

  _oneTimeUse = oneTimeUse;

  _type = shrineType;
}

// =============================================================================

void ShrineComponent::Update()
{
  if (_counter < _timeout)
  {
    _counter++;
  }

  if (_counter == _timeout)
  {
    //
    // So that we won't go into this block again
    //
    _counter++;

    Activate();
  }
}

// =============================================================================

IR ShrineComponent::Interact()
{
  if (_timeout == -1 || _counter < _timeout)
  {
    Printer::Instance().AddMessage("Shrine is inactive");
    return { InteractionResult::FAILURE, GameStates::MAIN_STATE };
  }
  else
  {
    int dungLvl = Map::Instance().CurrentLevel->DungeonLevel;

    _power    = dungLvl;
    _duration = GlobalConstants::EffectDefaultDuration * _power;

    _counter = 0;
    OwnerGameObject->FgColor = Colors::BlackColor;

    std::string shrineName = GlobalConstants::ShrineNameByType.at(_type);
    std::string message = Util::StringFormat("You touch the %s...", shrineName.data());

    if (GlobalConstants::ShrineSaintByType.count(_type) == 1)
    {
      std::string saint = GlobalConstants::ShrineSaintByType.at(_type);
      message = Util::StringFormat("You pray to %s...", saint.data());
    }

    Printer::Instance().AddMessage(message);

    ProcessEffect();

    if (_oneTimeUse)
    {
      _timeout = -1;
    }

    return { InteractionResult::SUCCESS, GameStates::MAIN_STATE };
  }
}

// =============================================================================

void ShrineComponent::Activate()
{
  OwnerGameObject->FgColor = Colors::ShrineColorsByType.at(_type).first;
}

// =============================================================================

void ShrineComponent::ProcessEffect()
{
  auto& playerRef = Application::Instance().PlayerInstance;

  std::string msg = "...but nothing happens";

  switch (_type)
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
      for (auto& i : playerRef.Inventory->Contents)
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
      for (auto& i : playerRef.Inventory->Contents)
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
      for (auto& i : playerRef.Inventory->Contents)
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
        Util::UpdateItemPrefix(itemsToCurse[index], ItemPrefix::CURSED);
        msg = "You sense the malevolent energy";
        auto& idName = itemsToCurse[index]->Data.IdentifiedName;
        idName = Util::ReplaceItemPrefix(idName, { "Blessed", "Uncursed" }, "Cursed");
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
      for (auto& i : playerRef.Inventory->Contents)
      {
        auto c = i->GetComponent<ItemComponent>();
        ItemComponent* ic = static_cast<ItemComponent*>(c);
        if (ic->Data.Prefix == ItemPrefix::CURSED)
        {
          ic->Data.IsPrefixDiscovered = true;
          Util::UpdateItemPrefix(ic, ItemPrefix::UNCURSED);
          success = true;
          ic->Data.IdentifiedName = Util::ReplaceItemPrefix(ic->Data.IdentifiedName, { "Cursed" }, "Uncursed");
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

// =============================================================================

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

  switch (b.Type)
  {
    case ItemBonusType::REGEN:
    case ItemBonusType::POISONED:
    case ItemBonusType::PARALYZE:
    case ItemBonusType::BURNING:
    {
      b.Cumulative = true;
      b.BonusValue = (b.Type == ItemBonusType::POISONED
                   || b.Type == ItemBonusType::BURNING) ? -1 : 1;
      b.Duration   = _duration;
      b.Period     = (b.Type == ItemBonusType::POISONED)
                     ? GlobalConstants::EffectDurationSkipsForTurn * 2
                     : 1;
    }
    break;

    case ItemBonusType::BLINDNESS:
    {
      b.BonusValue = -GlobalConstants::MaxVisibilityRadius;
    }
    break;

    case ItemBonusType::MANA_SHIELD:
    {
      if (playerRef.Attrs.MP.Min().Get() == 0)
      {
        b.Type = ItemBonusType::NONE;
      }
    }
    break;
  }

  if (b.Type != ItemBonusType::NONE)
  {
    playerRef.AddEffect(b);
    SetEffectGainMessage(logMessageToWrite, b.Type);
  }
  else
  {
    Printer::Instance().AddMessage(Strings::NoActionText);
  }
}

// =============================================================================

//
// WARNING: unused method?! (not found in "Find References")
//
// Maybe was prepared to be used but not used yet?
//
void ShrineComponent::ApplyRandomPositiveEffect(std::string& logMessageToWrite)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int effectIndex = RNG::Instance().RandomRange(0, _positiveEffects.size());
  ItemBonusType t = _positiveEffects[effectIndex];

  ItemBonusStruct b;

  b.Type       = t;
  b.BonusValue = _power;
  b.Duration   = _duration;
  b.Id         = OwnerGameObject->ObjectId();

  bool manaShieldOk = (playerRef.Attrs.MP.Min().Get() != 0 ||
                      (playerRef.Attrs.MP.Max().Get() != 0 &&
                       playerRef.Attrs.Mag.Get() > 0));

  if (t == ItemBonusType::MANA_SHIELD)
  {
    if (playerRef.Attrs.MP.Min().Get() > 0)
    {
      b.Duration = manaShieldOk ? -1 : 0;
    }
    else
    {
      b.Type = ItemBonusType::NONE;
    }
  }
  else if (t == ItemBonusType::REGEN)
  {
    b.Cumulative = true;
    b.BonusValue = 1;
    b.Period     = 1;
  }

  if (b.Type != ItemBonusType::NONE)
  {
    playerRef.AddEffect(b);
    SetEffectGainMessage(logMessageToWrite, b.Type);
  }
  else
  {
    Printer::Instance().AddMessage(Strings::NoActionText);
  }
}

// =============================================================================

void ShrineComponent::ApplyRandomNegativeEffect(std::string& logMessageToWrite)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int effectIndex = RNG::Instance().RandomRange(0, _negativeEffects.size());
  ItemBonusType t = _negativeEffects[effectIndex];

  ItemBonusStruct b;

  b.Type       = t;
  b.BonusValue = _power;
  b.Duration   = (t == ItemBonusType::BURNING) ? 10 : _duration;
  b.Id         = OwnerGameObject->ObjectId();

  if (t == ItemBonusType::BLINDNESS)
  {
    b.BonusValue = -GlobalConstants::MaxVisibilityRadius;
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

// =============================================================================

void ShrineComponent::ApplyTemporaryStatRaise(std::string& logMessageToWrite)
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int index = RNG::Instance().RandomRange(0, _attrs.size());

  ItemBonusStruct bs;

  bs.Type       = _attrs[index];
  bs.Duration   = 30 + _duration;
  bs.BonusValue = 1;
  bs.Id         = OwnerGameObject->ObjectId();
  bs.Cumulative = true;

  playerRef.AddEffect(bs);

  SetEffectGainMessage(logMessageToWrite, bs.Type);
}

// =============================================================================

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

    case ItemBonusType::TRUE_SEEING:
      logMessageToWrite = "You can see hidden things";
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

    case ItemBonusType::WEAKNESS:
      logMessageToWrite = "You feel weak!";
      break;
  }
}
