#include "item-use-handlers.h"

#include "item-component.h"
#include "application.h"
#include "map.h"
#include "player.h"
#include "printer.h"
#include "spells-processor.h"
#include "rng.h"
#include "util.h"

namespace ItemUseHandlers
{
  UseResult HealingPotionUseHandler(ItemComponent *item, GameObject* user)
  {
    int amount = 0;

    int statMax = user->Attrs.HP.Max().Get();
    int statCur = user->Attrs.HP.Min().Get();

    std::string message;

    double scale = 0.4;

    switch (item->Data.Prefix)
    {
      case ItemPrefix::BLESSED:
      {
        amount = statMax;
        message = (statCur == statMax)
                  ? Strings::NoActionText
                  : "Your wounds are healed completely!";
      }
      break;

      case ItemPrefix::UNCURSED:
      {
        amount = statMax * scale;
        message = "You feel better";
        message = (statCur == statMax)
                  ? Strings::NoActionText
                  : message;
      }
      break;

      case ItemPrefix::CURSED:
      {
        amount = statMax * (scale / 2.0);

        int var = RNG::Instance().RandomRange(0, 3);
        if (var == 0)
        {
          message = (statCur == statMax)
                    ? Strings::NoActionText
                    : "You feel a little better";
        }
        else if (var == 1)
        {
          amount = -amount;
          message = "You are damaged by a cursed potion!";
        }
        else if (var == 2)
        {
          message = "You feel unwell!";

          ItemBonusStruct e;

          e.Type       = ItemBonusType::POISONED;
          e.Duration   = GlobalConstants::EffectDefaultDuration;
          e.Period     = 10;
          e.BonusValue = -1;
          e.Cumulative = true;

          user->AddEffect(e);
        }
      }
      break;
    }

    if (Util::IsPlayer(user))
    {
      Printer::Instance().AddMessage(message);
    }

    user->Attrs.HP.AddMin(amount);

    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult ManaPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    int amount = 0;

    int statCur = user->Attrs.MP.Min().Get();
    int statMax = user->Attrs.MP.Max().Get();

    std::string message;

    switch (item->Data.Prefix)
    {
      case ItemPrefix::BLESSED:
      {
        amount = statMax;
        message = "Your spirit force was restored!";
        message = (statCur == statMax)
                  ? Strings::NoActionText
                  : message;
      }
      break;

      case ItemPrefix::UNCURSED:
      {
        amount = statMax * 0.3;
        message = "Your spirit is reinforced";
        message = (statCur == statMax)
                  ? Strings::NoActionText
                  : message;
      }
      break;

      case ItemPrefix::CURSED:
      {
        amount = -statMax * 0.3;
        message = "Your spirit force was drained!";
      }
      break;
    }

    user->Attrs.MP.AddMin(amount);

    if (Util::IsPlayer(user))
    {
      Printer::Instance().AddMessage(message);
    }

    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult NeutralizePoisonPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    std::string message = Strings::NoActionText;

    //
    // Blessed potion removes all poison, uncursed removes
    // only one of the accumulated ones, if any.
    //
    switch (item->Data.Prefix)
    {
      case ItemPrefix::BLESSED:
      {
        if (user->HasEffect(ItemBonusType::POISONED))
        {
          user->DispelEffectsAllOf(ItemBonusType::POISONED);
          message = "The illness is gone!";
        }
      }
      break;

      case ItemPrefix::UNCURSED:
      {
        if (user->HasEffect(ItemBonusType::POISONED))
        {
          user->DispelEffectFirstFound(ItemBonusType::POISONED);
          message = "The illness subsides";
        }
      }
      break;

      case ItemPrefix::CURSED:
      {
        ItemBonusStruct bs;

        bs.BonusValue = -1;
        bs.Period     = 10;
        bs.Duration   = GlobalConstants::EffectDefaultDuration;
        bs.Id         = item->OwnerGameObject->ObjectId();
        bs.Cumulative = true;
        bs.Type       = ItemBonusType::POISONED;

        user->AddEffect(bs);

        message = "You are feeling unwell...";
      }
      break;
    }

    if (Util::IsPlayer(user))
    {
      Printer::Instance().AddMessage(message);
    }

    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult JuicePotionUseHandler(ItemComponent* item, GameObject* user)
  {
    int amount = 0;

    int statMax = user->Attrs.Hunger;
    int& statCur = user->Attrs.Hunger;

    std::string message;

    switch (item->Data.Prefix)
    {
      case ItemPrefix::BLESSED:
      {
        amount = statMax * 0.6;
        message = (statCur == statMax)
                  ? Strings::NoActionText
                  : "Delicious fruit juice!";
      }
      break;

      case ItemPrefix::UNCURSED:
      {
        amount = statMax * 0.3;
        message = (statCur == statMax)
                  ? Strings::NoActionText
                  : "Tasted like fruit juice";
      }
      break;

      case ItemPrefix::CURSED:
      {
        amount = -statMax * 0.3;
        message = "Your feel peckish";
      }
      break;
    }

    statCur += amount;
    statCur = Util::Clamp(statCur, 0, statMax);

    if (Util::IsPlayer(user))
    {
      Printer::Instance().AddMessage(message);
    }

    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult CWPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    std::string message = Strings::NoActionText;

    switch (item->Data.Prefix)
    {
      case ItemPrefix::BLESSED:
      {
        bool fail = (user->Attrs.HP.IsFull()
                 && !user->HasEffect(ItemBonusType::WEAKNESS));

        if (!fail)
        {
          user->DispelEffectsAllOf(ItemBonusType::WEAKNESS);
          user->Attrs.HP.Restore();

          message = "You feel great!";
        }
      }
      break;

      case ItemPrefix::UNCURSED:
      {
        if (user->HasEffect(ItemBonusType::WEAKNESS))
        {
          user->DispelEffectsAllOf(ItemBonusType::WEAKNESS);
          message = "The weakness is gone!";
        }
      }
      break;

      case ItemPrefix::CURSED:
      {
        ItemBonusStruct bs;

        bs.Type       = ItemBonusType::POISONED;
        bs.BonusValue = -1;
        bs.Duration   = GlobalConstants::EffectDefaultDuration;
        bs.Period     = GlobalConstants::EffectDurationSkipsForTurn * 2;
        bs.Cumulative = true;
        bs.Id         = item->OwnerGameObject->ObjectId();

        user->AddEffect(bs);

        message = "You feel unwell...";
      }
      break;
    }

    if (Util::IsPlayer(user))
    {
      Printer::Instance().AddMessage(message);
    }

    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult RAPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    std::string message = Strings::NoActionText;

    auto& luh = user->GetLevelUpHistory();
    int lastInd = luh.size() - 1;
    auto it = luh.begin();
    std::advance(it, lastInd);
    int lostLevel = it->first;

    switch (item->Data.Prefix)
    {
      case ItemPrefix::BLESSED:
      {
        if (user->Attrs.Lvl.Get() < lostLevel)
        {
          int iterations = lostLevel - user->Attrs.Lvl.Get();
          for (int i = 0; i < iterations; i++)
          {
            if (Util::IsPlayer(user))
            {
              static_cast<Player*>(user)->LevelUpSilent();
            }
            else
            {
              user->LevelUp();
            }
          }

          message = "You regained lost experience!";
        }
      }
      break;

      case ItemPrefix::UNCURSED:
      {
        if (user->Attrs.Lvl.Get() < lostLevel)
        {
          if (Util::IsPlayer(user))
          {
            static_cast<Player*>(user)->LevelUpSilent();
          }
          else
          {
            user->LevelUp();
          }

          message = "You regain some of your loss back";
        }
      }
      break;

      case ItemPrefix::CURSED:
      {
        if (user->Attrs.Lvl.Get() != 1)
        {
          if (Util::IsPlayer(user))
          {
            static_cast<Player*>(user)->LevelDownSilent();
          }
          else
          {
            user->LevelDown();
          }

          message = "You feel less confident...";
        }
      }
      break;
    }

    if (Util::IsPlayer(user))
    {
      Printer::Instance().AddMessage(message);
    }

    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult ExpPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    int amount = 0;

    int statMax = 100;

    std::string message;

    switch (item->Data.Prefix)
    {
      case ItemPrefix::BLESSED:
      {
        amount = statMax;
        message = "You feel enlightened!";
      }
      break;

      case ItemPrefix::UNCURSED:
      {
        amount = statMax * 0.3;
        message = "You feel more experienced";
      }
      break;

      case ItemPrefix::CURSED:
      {
        amount = -statMax * 0.3;
        message = "You lose some experience!";
      }
      break;
    }

    user->AwardExperience(amount);

    if (Util::IsPlayer(user))
    {
      Printer::Instance().AddMessage(message);
    }

    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult StatPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    ItemPrefix buc = item->Data.Prefix;

    std::unordered_map<PotionType, std::string> useMessagesByType =
    {
      { PotionType::STR_POTION, "Your Strength has "   },
      { PotionType::DEF_POTION, "Your Defence has "    },
      { PotionType::MAG_POTION, "Your Magic has "      },
      { PotionType::RES_POTION, "Your Resistance has " },
      { PotionType::SKL_POTION, "Your Skill has "      },
      { PotionType::SPD_POTION, "Your Speed has "      }
    };

    int valueToAdd = 0;

    auto message = useMessagesByType[item->Data.PotionType_];

    switch (buc)
    {
      case ItemPrefix::UNCURSED:
      {
        valueToAdd = 1;
        message += "increased!";
      }
      break;

      case ItemPrefix::BLESSED:
      {
        valueToAdd = 2;
        message += "increased significantly!";
      }
      break;

      case ItemPrefix::CURSED:
      {
        valueToAdd = -1;
        message += "decreased!";
      }
      break;
    }

    std::unordered_map<PotionType, Attribute&> userStats =
    {
      { PotionType::STR_POTION, user->Attrs.Str },
      { PotionType::DEF_POTION, user->Attrs.Def },
      { PotionType::MAG_POTION, user->Attrs.Mag },
      { PotionType::RES_POTION, user->Attrs.Res },
      { PotionType::SKL_POTION, user->Attrs.Skl },
      { PotionType::SPD_POTION, user->Attrs.Spd }
    };

    int newValue = userStats.at(item->Data.PotionType_).OriginalValue() + valueToAdd;

    if (newValue < 0)
    {
      newValue = 0;
      message = Strings::NoActionText;
    }

    userStats.at(item->Data.PotionType_).Set(newValue);

    if (Util::IsPlayer(user))
    {
      Printer::Instance().AddMessage(message);
    }

    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult ReturnerUseHandler(ItemComponent* item, GameObject* user)
  {
    //
    // TODO: monsters usage?
    //
    if (!item->Data.IsIdentified)
    {
      return UseResult::UNUSABLE;
    }

    if (item->Data.Amount == 0
     || Map::Instance().CurrentLevel->MysteriousForcePresent)
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                             Strings::MessageBoxInformationHeaderText,
                                             { "You invoke the returner, but nothing happens." },
                                             Colors::ShadesOfGrey::Six);
      return UseResult::FAILURE;
    }

    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult RepairKitUseHandler(ItemComponent* item, GameObject* user)
  {
    //
    // TODO: monsters usage?
    //
    Player* playerRef = &Application::Instance().PlayerInstance;

    if (!playerRef->HasSkill(PlayerSkills::REPAIR))
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                             Strings::MessageBoxEpicFailHeaderText,
                                             { "You don't possess the necessary skill!" },
                                             Colors::MessageBoxRedBorderColor);
      return UseResult::FAILURE;
    }

    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult ScrollUseHandler(ItemComponent* item, GameObject* user)
  {
    SpellsProcessor::Instance().ProcessScroll(item, user);
    return UseResult::SUCCESS;
  }

  // ===========================================================================

  UseResult FoodUseHandler(ItemComponent* item, GameObject* user)
  {
    auto objName = (item->Data.IsIdentified) ?
                    item->Data.IdentifiedName :
                    item->Data.UnidentifiedName;

    std::vector<std::string> eatMessages;

    eatMessages.push_back(Util::StringFormat("You eat %s...", objName.data()));

    switch (item->Data.Prefix)
    {
      case ItemPrefix::UNCURSED:
      {
        eatMessages.push_back("It tasted OK");
        user->Attrs.Hunger -= item->Data.Cost * 0.75;
      }
      break;

      case ItemPrefix::BLESSED:
      {
        eatMessages.push_back("It's delicious!");
        user->Attrs.Hunger -= item->Data.Cost;
      }
      break;

      case ItemPrefix::CURSED:
      {
        eatMessages.push_back("Disgusting!");

        if (Util::Rolld100(50))
        {
          user->Attrs.Hunger += item->Data.Cost;

          eatMessages.push_back("Rotten food!");

          // NOTE: assuming player hunger meter is in order of 1000
          int dur = item->Data.Cost / 100;

          ItemBonusStruct b;
          b.Type = ItemBonusType::POISONED;
          b.BonusValue = -1;
          b.Period = 10;
          b.Duration = dur;
          b.Cumulative = true;
          b.Id = item->OwnerGameObject->ObjectId();

          user->AddEffect(b);
        }
        else
        {
          user->Attrs.Hunger -= item->Data.Cost * 0.5;
        }
      }
      break;
    }

    user->Attrs.Hunger = Util::Clamp(user->Attrs.Hunger,
                                     0,
                                     user->Attrs.HungerRate.Get());

    if (Util::IsPlayer(user))
    {
      for (auto& msg : eatMessages)
      {
        Printer::Instance().AddMessage(msg);
      }
    }

    return UseResult::SUCCESS;
  }
}
