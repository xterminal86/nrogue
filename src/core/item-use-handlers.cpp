#include "item-use-handlers.h"

#include "item-component.h"
#include "application.h"
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

    float scale = 0.4f;

    if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      amount = statMax;
      message = (statCur == statMax)
                ? Strings::NoActionText
                : "Your wounds are healed completely!";
    }
    else if (item->Data.Prefix == ItemPrefix::UNCURSED)
    {
      amount = statMax * scale;
      message = "You feel better";
      message = (statCur == statMax)
                ? Strings::NoActionText
                : message;
    }
    else if (item->Data.Prefix == ItemPrefix::CURSED)
    {
      amount = statMax * (scale / 2.0f);

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

    if (Util::IsPlayer(user))
    {
      Player* playerRef = &Application::Instance().PlayerInstance;
      playerRef->RememberItem(item, Strings::UnidentifiedEffectText);

      Printer::Instance().AddMessage(message);

      //
      // Potion that deals damage is not necessarily
      // a cursed healing potion, thus check for amount > 0.
      //
      if(message != Strings::NoActionText && amount > 0)
      {
        playerRef->RememberItem(item, "healing potion");
      }
    }

    user->Attrs.HP.AddMin(amount);

    return UseResult::SUCCESS;
  }

  UseResult ManaPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    int amount = 0;

    int statCur = user->Attrs.MP.Min().Get();
    int statMax = user->Attrs.MP.Max().Get();

    std::string message;

    if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      amount = statMax;
      message = "Your spirit force was restored!";
      message = (statCur == statMax)
                ? Strings::NoActionText
                : message;
    }
    else if (item->Data.Prefix == ItemPrefix::UNCURSED)
    {
      amount = statMax * 0.3f;
      message = "Your spirit is reinforced";
      message = (statCur == statMax)
                ? Strings::NoActionText
                : message;
    }
    else if (item->Data.Prefix == ItemPrefix::CURSED)
    {
      amount = -statMax * 0.3f;
      message = "Your spirit force was drained!";
    }

    user->Attrs.MP.AddMin(amount);

    if (Util::IsPlayer(user))
    {
      Player* playerRef = &Application::Instance().PlayerInstance;
      playerRef->RememberItem(item, Strings::UnidentifiedEffectText);

      Printer::Instance().AddMessage(message);

      if (message != Strings::NoActionText && amount > 0)
      {
        playerRef->RememberItem(item, "mana potion");
      }
    }

    return UseResult::SUCCESS;
  }

  UseResult NeutralizePoisonPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    std::string message = Strings::NoActionText;

    // Blessed potion removes all poison, uncursed removes
    // only one of the accumulated ones, if any.

    if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      if (user->HasEffect(ItemBonusType::POISONED))
      {
        user->RemoveEffectAllOf(ItemBonusType::POISONED);
        message = "The illness is gone!";
      }
    }
    else if (item->Data.Prefix == ItemPrefix::UNCURSED)
    {
      if (user->HasEffect(ItemBonusType::POISONED))
      {
        user->RemoveEffectFirstFound(ItemBonusType::POISONED);
        message = "The illness subsides";
      }
    }
    else if (item->Data.Prefix == ItemPrefix::CURSED)
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

    if (Util::IsPlayer(user))
    {
      Player* playerRef = &Application::Instance().PlayerInstance;
      playerRef->RememberItem(item, Strings::UnidentifiedEffectText);

      Printer::Instance().AddMessage(message);

      if (message != Strings::NoActionText)
      {
        playerRef->RememberItem(item, "neutralize poison");
      }
    }

    return UseResult::SUCCESS;
  }

  UseResult HungerPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    int amount = 0;

    int statMax = user->Attrs.Hunger;
    int& statCur = user->Attrs.Hunger;

    std::string message;

    if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      amount = statMax;
      message = (statCur == statMax)
                ? Strings::NoActionText
                : "You feel satiated!";
    }
    else if (item->Data.Prefix == ItemPrefix::UNCURSED)
    {
      amount = statMax * 0.3f;
      message = (statCur == statMax)
                ? Strings::NoActionText
                : "Your hunger has abated somewhat";
    }
    else if (item->Data.Prefix == ItemPrefix::CURSED)
    {
      amount = -statMax * 0.3f;
      message = "Your feel peckish";
    }

    statCur += amount;
    statCur = Util::Clamp(statCur, 0, statMax);

    if (Util::IsPlayer(user))
    {
      Player* playerRef = &Application::Instance().PlayerInstance;
      playerRef->RememberItem(item, Strings::UnidentifiedEffectText);

      Printer::Instance().AddMessage(message);

      if (message != Strings::NoActionText)
      {
        playerRef->RememberItem(item, "food potion");
      }
    }

    return UseResult::SUCCESS;
  }

  UseResult ExpPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    int amount = 0;

    int statMax = 100;

    std::string message;

    if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      amount = statMax;
      message = "You feel enlighted!";
    }
    else if (item->Data.Prefix == ItemPrefix::UNCURSED)
    {
      amount = statMax * 0.3f;
      message = "You feel more experienced";
    }
    else if (item->Data.Prefix == ItemPrefix::CURSED)
    {
      amount = -statMax * 0.3f;
      message = "You lose some experience!";
    }

    user->AwardExperience(amount);

    if (Util::IsPlayer(user))
    {
      Printer::Instance().AddMessage(message);

      Player* playerRef = &Application::Instance().PlayerInstance;
      playerRef->RememberItem(item, "potion of learning");
    }

    return UseResult::SUCCESS;
  }

  UseResult StatPotionUseHandler(ItemComponent* item, GameObject* user)
  {
    ItemPrefix buc = item->Data.Prefix;

    std::map<ItemType, std::string> useMessagesByType =
    {
      { ItemType::STR_POTION, "Your Strength has "   },
      { ItemType::DEF_POTION, "Your Defence has "    },
      { ItemType::MAG_POTION, "Your Magic has "      },
      { ItemType::RES_POTION, "Your Resistance has " },
      { ItemType::SKL_POTION, "Your Skill has "      },
      { ItemType::SPD_POTION, "Your Speed has "      }
    };

    int valueToAdd = 0;

    auto message = useMessagesByType[item->Data.ItemType_];

    if (buc == ItemPrefix::UNCURSED)
    {
      valueToAdd = 1;
      message += "increased!";
    }
    else if (buc == ItemPrefix::BLESSED)
    {
      valueToAdd = 2;
      message += "increased significantly!";
    }
    else if (buc == ItemPrefix::CURSED)
    {
      valueToAdd = -1;
      message += "decreased!";
    }

    std::map<ItemType, Attribute&> userStats =
    {
      { ItemType::STR_POTION, user->Attrs.Str },
      { ItemType::DEF_POTION, user->Attrs.Def },
      { ItemType::MAG_POTION, user->Attrs.Mag },
      { ItemType::RES_POTION, user->Attrs.Res },
      { ItemType::SKL_POTION, user->Attrs.Skl },
      { ItemType::SPD_POTION, user->Attrs.Spd }
    };

    const std::map<ItemType, std::string> playerMemoryTextByType =
    {
      { ItemType::STR_POTION, "+STR" },
      { ItemType::DEF_POTION, "+DEF" },
      { ItemType::MAG_POTION, "+MAG" },
      { ItemType::RES_POTION, "+RES" },
      { ItemType::SKL_POTION, "+SKL" },
      { ItemType::SPD_POTION, "+SPD" }
    };

    auto itemType = item->Data.ItemType_;

    int newValue = userStats.at(itemType).OriginalValue() + valueToAdd;

    if (newValue < 0)
    {
      newValue = 0;
      message = Strings::NoActionText;
    }

    userStats.at(itemType).Set(newValue);

    if (Util::IsPlayer(user))
    {
      Printer::Instance().AddMessage(message);

      if (message != Strings::NoActionText)
      {
        Player* playerRef = &Application::Instance().PlayerInstance;
        playerRef->RememberItem(item, playerMemoryTextByType.at(itemType));
      }
    }

    return UseResult::SUCCESS;
  }

  UseResult ReturnerUseHandler(ItemComponent* item, GameObject* user)
  {
    if (!item->Data.IsIdentified)
    {
      return UseResult::UNUSABLE;
    }

    if (item->Data.Amount == 0)
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                             Strings::MessageBoxInformationHeaderText,
                                             { "You invoke the returner, but nothing happens." },
                                             Colors::ShadesOfGrey::Six);
      return UseResult::FAILURE;
    }

    return UseResult::SUCCESS;
  }

  UseResult RepairKitUseHandler(ItemComponent* item, GameObject* user)
  {
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

  UseResult ScrollUseHandler(ItemComponent* item, GameObject* user)
  {
    SpellsProcessor::Instance().ProcessScroll(item, user);
    return UseResult::SUCCESS;
  }

  UseResult FoodUseHandler(ItemComponent* item, GameObject* user)
  {
    auto objName = (item->Data.IsIdentified) ?
                    item->Data.IdentifiedName :
                    item->Data.UnidentifiedName;

    std::vector<std::string> eatMessages;

    eatMessages.push_back(Util::StringFormat("You eat %s...", objName.data()));

    if (item->Data.Prefix == ItemPrefix::CURSED)
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
        user->Attrs.Hunger -= item->Data.Cost * 0.5f;
      }
    }
    else if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      eatMessages.push_back("It's delicious!");
      user->Attrs.Hunger -= item->Data.Cost;
    }
    else
    {
      eatMessages.push_back("It tasted OK");
      user->Attrs.Hunger -= item->Data.Cost * 0.75f;
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
