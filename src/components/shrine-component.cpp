#include "shrine-component.h"

#include "game-object.h"
#include "util.h"
#include "application.h"

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
      int choice = RNG::Instance().RandomRange(0, 2);
      if (choice == 0)
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
      it->second.Add(1);

      ApplyRandomNegativeEffect();
    }
    break;

    // random effect
    case ShrineType::FORGOTTEN:
      ApplyRandomEffect();
      break;

    // temporary raises stats
    case ShrineType::POTENTIAL:
      ApplyTemporaryStatRaise();
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
  int dur = Timeout / 2;

  int effectIndex = RNG::Instance().RandomRange(0, GlobalConstants::BonusDisplayNameByType.size());
  auto it = GlobalConstants::BonusDisplayNameByType.begin();
  std::advance(it, effectIndex);

  ItemBonusStruct b;
  b.Type = it->first;
  b.BonusValue = power;
  b.Duration= dur;
  b.Id = OwnerGameObject->ObjectId();

  if (b.Type == ItemBonusType::REGEN
   || b.Type == ItemBonusType::POISONED
   || b.Type == ItemBonusType::PARALYZE)
  {
    b.Cumulative = true;
    b.BonusValue = 1;
    b.Period = Timeout / 10;
  }

  playerRef.AddEffect(b);
}

void ShrineComponent::ApplyRandomPositiveEffect()
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int power = Timeout / 100;
  int dur = Timeout / 2;

  int effectIndex = RNG::Instance().RandomRange(0, PositiveEffects.size());
  ItemBonusType t = PositiveEffects[effectIndex];

  ItemBonusStruct b;
  b.Type = t;
  b.BonusValue = power;
  b.Duration = dur;
  b.Id = OwnerGameObject->ObjectId();

  if (t == ItemBonusType::MANA_SHIELD && playerRef.Attrs.MP.Max().Get() != 0)
  {
    b.Duration = -1;
  }
  else if (t == ItemBonusType::REGEN)
  {
    b.Cumulative = true;
    b.BonusValue = 1;
    b.Period = Timeout / 10;
  }

  playerRef.AddEffect(b);
}

void ShrineComponent::ApplyRandomNegativeEffect()
{
  auto& playerRef = Application::Instance().PlayerInstance;

  int power = Timeout / 100;
  int dur = Timeout / 2;

  int effectIndex = RNG::Instance().RandomRange(0, NegativeEffects.size());
  ItemBonusType t = NegativeEffects[effectIndex];

  ItemBonusStruct b;
  b.Type = t;
  b.BonusValue = power;
  b.Duration = dur;
  b.Id = OwnerGameObject->ObjectId();

  if (t == ItemBonusType::BURNING)
  {
    b.BonusValue = 1;
  }

  if (t == ItemBonusType::POISONED
   || t == ItemBonusType::PARALYZE)
  {
    b.Cumulative = true;
    b.Period = Timeout / 10;
  }

  playerRef.AddEffect(b);
}

void ShrineComponent::ApplyTemporaryStatRaise()
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
  bs.Type = attrs[index];
  bs.Duration = Timeout / 2;
  bs.BonusValue = 1;
  bs.Id = OwnerGameObject->ObjectId();
  bs.Cumulative = true;

  playerRef.AddEffect(bs);
}
