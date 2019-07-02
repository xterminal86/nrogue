#include "shrine-component.h"

#include "game-object.h"
#include "util.h"
#include "application.h"
#include "effect.h"

ShrineComponent::ShrineComponent()
{
  _hash = typeid(*this).hash_code();
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
      int choice = RNG::Instance().RandomRange(0, 2);
      if (choice == 0)
      {
        int orig = playerRef.Attrs.Str.OriginalValue;
        playerRef.Attrs.Str.Set(orig + 1);
        playerRef.RecalculateStatsModifiers();
        msg = "You feel stronger!";
      }
      else
      {
        int hpToAdd = 2 * (playerRef.Attrs.HP.Talents + 1);
        playerRef.Attrs.HP.OriginalValue += hpToAdd;
        msg = "You feel tougher!";
      }
    }
    break;

    // raises MAG or MP
    case ShrineType::SPIRIT:
    {
      int choice = RNG::Instance().RandomRange(0, 2);
      if (choice == 0)
      {
        int orig = playerRef.Attrs.Mag.OriginalValue;
        playerRef.Attrs.Mag.Set(orig + 1);
        msg = "You feel strong-willed!";
      }
      else
      {
        int mpToAdd = 2 * (playerRef.Attrs.MP.Talents + 1);
        playerRef.Attrs.MP.OriginalValue += mpToAdd;
        msg = "You spirituality has increased!";
      }
    }
    break;

    // restores MP
    case ShrineType::TRANQUILITY:
    {
      if (playerRef.Attrs.MP.OriginalValue != 0)
      {
        playerRef.Attrs.MP.CurrentValue = playerRef.Attrs.MP.OriginalValue;
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
      playerRef.Attrs.HP.CurrentValue = playerRef.Attrs.HP.OriginalValue;
      msg = "You feel better!";
    }
    break;

    // random effect + receive curse
    case ShrineType::DESECRATED:
    {
      ApplyRandomEffect();

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
      ApplyRandomEffect();

      if (Util::Rolld100(25))
      {
        ApplyRandomNegativeEffect();
      }
    }
    break;

    // random effect + high chance to receive negative effect
    case ShrineType::DISTURBING:
    {
      ApplyRandomEffect();

      if (Util::Rolld100(75))
      {
        ApplyRandomNegativeEffect();
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
      it->second.OriginalValue++;

      ApplyRandomNegativeEffect();
    }
    break;

    // random effect
    case ShrineType::FORGOTTEN:
      ApplyRandomEffect();
      break;

    // temporary raises stats
    case ShrineType::POTENTIAL:
      // TODO:
      break;

    // random effect
    case ShrineType::HIDDEN:
      ApplyRandomEffect();
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

void ShrineComponent::ApplyRandomEffect()
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int power = Timeout / 100;
  int dur = Timeout / 10;

  int effectIndex = RNG::Instance().RandomRange(0, EffectNameByType.size());
  auto it = EffectNameByType.begin();
  std::advance(it, effectIndex);
  EffectType e = it->first;

  playerRef.AddEffect(e, power, dur, false, true);
}

void ShrineComponent::ApplyRandomPositiveEffect()
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int power = Timeout / 100;
  int dur = Timeout / 5;

  int effectIndex = RNG::Instance().RandomRange(0, PositiveEffects.size());
  EffectType e = PositiveEffects[effectIndex];

  if (e == EffectType::MANA_SHIELD && playerRef.Attrs.MP.OriginalValue != 0)
  {
    playerRef.AddEffect(e, 0, -1, false, true);
  }
  else
  {
    playerRef.AddEffect(e, power, dur, false, true);
  }
}

void ShrineComponent::ApplyRandomNegativeEffect()
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int power = Timeout / 100;
  int dur = Timeout / 5;

  int effectIndex = RNG::Instance().RandomRange(0, NegativeEffects.size());
  EffectType e = NegativeEffects[effectIndex];

  playerRef.AddEffect(e, power, dur, false, true);
}
