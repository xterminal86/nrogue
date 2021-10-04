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
  bool HealingPotionUseHandler(ItemComponent *item)
  {
    Player* playerRef = &Application::Instance().PlayerInstance;

    playerRef->RememberItem(item, GlobalConstants::UnidentifiedEffectText);

    int amount = 0;

    int statMax = playerRef->Attrs.HP.Max().Get();
    int statCur = playerRef->Attrs.HP.Min().Get();

    std::string message;

    float scale = 0.4f;

    if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      amount = statMax;
      message = (statCur == statMax)
                ? GlobalConstants::NoActionText
                : "Your wounds are healed completely!";
    }
    else if (item->Data.Prefix == ItemPrefix::UNCURSED)
    {
      amount = statMax * scale;
      message = "You feel better";
      message = (statCur == statMax)
                ? GlobalConstants::NoActionText
                : "You feel better";
    }
    else if (item->Data.Prefix == ItemPrefix::CURSED)
    {
      amount = statMax * (scale / 2.0f);

      int var = RNG::Instance().RandomRange(0, 3);
      if (var == 0)
      {
        message = (statCur == statMax)
                  ? GlobalConstants::NoActionText
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

        playerRef->AddEffect(e);
      }
    }

    if (message != GlobalConstants::NoActionText && amount > 0)
    {
      playerRef->RememberItem(item, "healing potion");
    }

    Printer::Instance().AddMessage(message);

    playerRef->Attrs.HP.AddMin(amount);

    Application::Instance().ChangeState(GameStates::MAIN_STATE);

    return true;
  }

  bool ManaPotionUseHandler(ItemComponent* item)
  {
    Player* playerRef = &Application::Instance().PlayerInstance;

    int amount = 0;

    int statMax = playerRef->Attrs.MP.Max().Get();

    if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      amount = statMax;
      Printer::Instance().AddMessage("Your spirit force was restored!");
    }
    else if (item->Data.Prefix == ItemPrefix::UNCURSED)
    {
      amount = statMax * 0.3f;
      Printer::Instance().AddMessage("Your spirit is reinforced");
    }
    else if (item->Data.Prefix == ItemPrefix::CURSED)
    {
      amount = -statMax * 0.3f;
      Printer::Instance().AddMessage("Your spirit force was drained!");
    }

    playerRef->RememberItem(item, "mana potion");

    playerRef->Attrs.MP.AddMin(amount);

    Application::Instance().ChangeState(GameStates::MAIN_STATE);

    return true;
  }

  bool NeutralizePoisonPotionUseHandler(ItemComponent* item)
  {
    Player* playerRef = &Application::Instance().PlayerInstance;

    playerRef->RememberItem(item, GlobalConstants::UnidentifiedEffectText);

    std::string message = GlobalConstants::NoActionText;

    // Blessed potion removes all poison, uncursed removes
    // only one of the accumulated ones, if any.

    if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      if (playerRef->HasEffect(ItemBonusType::POISONED))
      {
        playerRef->RemoveEffectAllOf(ItemBonusType::POISONED);
        message = "The illness is gone!";

        playerRef->RememberItem(item, "neutralize poison");
      }
    }
    else if (item->Data.Prefix == ItemPrefix::UNCURSED)
    {
      if (playerRef->HasEffect(ItemBonusType::POISONED))
      {
        playerRef->RemoveEffectFirstFound(ItemBonusType::POISONED);
        message = "The illness subsides";

        playerRef->RememberItem(item, "neutralize poison");
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

      playerRef->AddEffect(bs);

      message = "You are feeling unwell...";
    }

    Printer::Instance().AddMessage(message);

    Application::Instance().ChangeState(GameStates::MAIN_STATE);

    return true;
  }

  bool HungerPotionUseHandler(ItemComponent* item)
  {
    Player* playerRef = &Application::Instance().PlayerInstance;

    playerRef->RememberItem(item, GlobalConstants::UnidentifiedEffectText);

    int amount = 0;

    int statMax = playerRef->Attrs.Hunger;
    int& statCur = playerRef->Attrs.Hunger;

    std::string message;

    if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      amount = statMax;
      message = (statCur == statMax)
                ? GlobalConstants::NoActionText
                : "You feel satiated!";
    }
    else if (item->Data.Prefix == ItemPrefix::UNCURSED)
    {
      amount = statMax * 0.3f;
      message = (statCur == statMax)
                ? GlobalConstants::NoActionText
                : "Your hunger has abated somewhat";
    }
    else if (item->Data.Prefix == ItemPrefix::CURSED)
    {
      amount = -statMax * 0.3f;
      message = "Your feel peckish";
    }

    if (message != GlobalConstants::NoActionText)
    {
      playerRef->RememberItem(item, "food potion");
    }

    Printer::Instance().AddMessage(message);

    statCur += amount;
    statCur = Util::Clamp(statCur, 0, statMax);

    Application::Instance().ChangeState(GameStates::MAIN_STATE);

    return true;
  }

  bool ExpPotionUseHandler(ItemComponent* item)
  {
    Player* playerRef = &Application::Instance().PlayerInstance;

    int amount = 0;

    int statMax = 100;

    if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      amount = statMax;
      Printer::Instance().AddMessage("You feel enlighted!");
    }
    else if (item->Data.Prefix == ItemPrefix::UNCURSED)
    {
      amount = statMax * 0.3f;
      Printer::Instance().AddMessage("You feel more experienced");
    }
    else if (item->Data.Prefix == ItemPrefix::CURSED)
    {
      amount = -statMax * 0.3f;
      Printer::Instance().AddMessage("You lose some experience!");
    }

    playerRef->RememberItem(item, "potion of learning");

    Application::Instance().ChangeState(GameStates::MAIN_STATE);

    playerRef->AwardExperience(amount);

    return true;
  }

  bool StatPotionUseHandler(ItemComponent* item)
  {
    Player* playerRef = &Application::Instance().PlayerInstance;

    ItemPrefix buc = item->Data.Prefix;

    std::map<ItemType, std::string> useMessagesByType =
    {
      { ItemType::STR_POTION, "Your Strength has " },
      { ItemType::DEF_POTION, "Your Defence has " },
      { ItemType::MAG_POTION, "Your Magic has " },
      { ItemType::RES_POTION, "Your Resistance has " },
      { ItemType::SKL_POTION, "Your Skill has " },
      { ItemType::SPD_POTION, "Your Speed has " }
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

    Printer::Instance().AddMessage(message);

    std::map<ItemType, Attribute&> playerStats =
    {
      { ItemType::STR_POTION, playerRef->Attrs.Str },
      { ItemType::DEF_POTION, playerRef->Attrs.Def },
      { ItemType::MAG_POTION, playerRef->Attrs.Mag },
      { ItemType::RES_POTION, playerRef->Attrs.Res },
      { ItemType::SKL_POTION, playerRef->Attrs.Skl },
      { ItemType::SPD_POTION, playerRef->Attrs.Spd }
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

    int newValue = playerStats.at(itemType).OriginalValue() + valueToAdd;

    if (newValue < 0)
    {
      newValue = 0;
      message = GlobalConstants::NoActionText;
    }

    playerStats.at(itemType).Set(newValue);

    playerRef->RememberItem(item, playerMemoryTextByType.at(itemType));

    Application::Instance().ChangeState(GameStates::MAIN_STATE);

    return true;
  }

  bool ReturnerUseHandler(ItemComponent* item)
  {
    if (!item->Data.IsIdentified)
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                             GlobalConstants::MessageBoxInformationHeaderText,
                                             { "Can't be used!" },
                                             Colors::MessageBoxRedBorderColor);
      return false;
    }

    if (item->Data.Amount == 0)
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                             GlobalConstants::MessageBoxInformationHeaderText,
                                             { "You invoke the returner, but nothing happens." },
                                             Colors::ShadesOfGrey::Six);
      return false;
    }

    return true;
  }

  bool RepairKitUseHandler(ItemComponent* item)
  {
    Player* playerRef = &Application::Instance().PlayerInstance;

    if (!playerRef->HasSkill(PlayerSkills::REPAIR))
    {
      Application::Instance().ShowMessageBox(MessageBoxType::ANY_KEY,
                                             GlobalConstants::MessageBoxEpicFailHeaderText,
                                             { "You don't possess the necessary skill!" },
                                             Colors::MessageBoxRedBorderColor);
      return false;
    }

    return true;
  }

  bool ScrollUseHandler(ItemComponent* item)
  {
    SpellsProcessor::Instance().ProcessScroll(item);
    Application::Instance().ChangeState(GameStates::MAIN_STATE);
    return true;
  }

  bool FoodUseHandler(ItemComponent* item)
  {
    Player* playerRef = &Application::Instance().PlayerInstance;

    auto objName = (item->Data.IsIdentified) ?
                    item->Data.IdentifiedName :
                    item->Data.UnidentifiedName;

    auto str = Util::StringFormat("You eat %s...", objName.data());
    Printer::Instance().AddMessage(str);

    if (item->Data.Prefix == ItemPrefix::CURSED)
    {
      Printer::Instance().AddMessage("Disgusting!");

      if (Util::Rolld100(50))
      {
        playerRef->Attrs.Hunger += item->Data.Cost;

        Printer::Instance().AddMessage("Rotten food!");

        // NOTE: assuming player hunger meter is in order of 1000
        int dur = item->Data.Cost / 100;

        ItemBonusStruct b;
        b.Type = ItemBonusType::POISONED;
        b.BonusValue = -1;
        b.Period = 10;
        b.Duration = dur;
        b.Cumulative = true;
        b.Id = item->OwnerGameObject->ObjectId();

        playerRef->AddEffect(b);
      }
      else
      {
        playerRef->Attrs.Hunger -= item->Data.Cost * 0.5f;
      }
    }
    else if (item->Data.Prefix == ItemPrefix::BLESSED)
    {
      Printer::Instance().AddMessage("It's delicious!");

      playerRef->Attrs.Hunger -= item->Data.Cost;
      playerRef->IsStarving = false;
    }
    else
    {
      Printer::Instance().AddMessage("It tasted OK");

      playerRef->Attrs.Hunger -= item->Data.Cost * 0.75f;
      playerRef->IsStarving = false;
    }

    playerRef->Attrs.Hunger = Util::Clamp(playerRef->Attrs.Hunger, 0, playerRef->Attrs.HungerRate.Get());

    Application::Instance().ChangeState(GameStates::MAIN_STATE);

    return true;
  }
}
