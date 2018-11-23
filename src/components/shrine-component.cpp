#include "shrine-component.h"

#include "game-object.h"
#include "util.h"
#include "application.h"

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

    std::string message;

    std::string saint = GlobalConstants::ShrineSaintByType.at(Type);
    message = Util::StringFormat("You pray to %s...", saint.data());
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

  switch (Type)
  {
    case ShrineType::MIGHT:
    {
      int percentage = (float)playerRef.Attrs.HP.OriginalValue * 0.1f;

      bool cond = (playerRef.Attrs.HP.CurrentValue > percentage);
      if (cond)
      {
        Printer::Instance().AddMessage("... but nothing happens");
      }
      else
      {
        Printer::Instance().AddMessage("Your wounds are healed!");
        playerRef.Attrs.HP.Set(playerRef.Attrs.HP.OriginalValue);
      }
    }
    break;

    case ShrineType::HEALING:
    {
      int percentage = (float)playerRef.Attrs.HP.OriginalValue * 0.4f;
      Printer::Instance().AddMessage("You feel better");
      playerRef.Attrs.HP.Add(percentage);
    }
    break;

    case ShrineType::SPIRIT:
    {
      int percentage = (float)playerRef.Attrs.MP.OriginalValue * 0.1f;

      bool cond = (playerRef.Attrs.MP.OriginalValue == 0
                || playerRef.Attrs.MP.CurrentValue > percentage);
      if (cond)
      {
        Printer::Instance().AddMessage("... but nothing happens");
      }
      else
      {
        Printer::Instance().AddMessage("Your spirit is reinforced!");
        playerRef.Attrs.MP.Set(playerRef.Attrs.MP.OriginalValue);
      }
    }
    break;

    case ShrineType::KNOWLEDGE:
    {
      bool success = false;
      Player& pl = Application::Instance().PlayerInstance;
      for (auto& i : pl.Inventory.Contents)
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
        Printer::Instance().AddMessage("Your possessions have been identified!");
      }
      else
      {
        Printer::Instance().AddMessage("... but nothing happens");
      }
    }
    break;

    case ShrineType::PERCEPTION:
      Printer::Instance().AddMessage("... but nothing happens");
      break;
  }
}
