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
    ((GameObject*)OwnerGameObject)->FgColor = GlobalConstants::BlackColor;

    std::string message;

    std::string saint = (Type == ShrineType::MIGHT) ? "Saint George" : "Virgin Mary";
    message = Util::StringFormat("You pray to %s...", saint.data());
    Printer::Instance().AddMessage(message);

    ProcessEffect();
  }
}

void ShrineComponent::Activate()
{
  ((GameObject*)OwnerGameObject)->FgColor = (Type == ShrineType::MIGHT) ? GlobalConstants::ShrineMightColor : GlobalConstants::ShrineSpiritColor;
}

void ShrineComponent::ProcessEffect()
{
  auto& playerRef = Application::Instance().PlayerInstance;

  if (Type == ShrineType::MIGHT)
  {
    int percentage = (float)playerRef.Attrs.HP.CurrentValue * 0.1f;

    bool cond = (playerRef.Attrs.HP.OriginalValue == 0
              || playerRef.Attrs.HP.CurrentValue > percentage);
    if (cond)
    {
      Printer::Instance().AddMessage("... but nothing happens.");
    }
    else
    {
      Printer::Instance().AddMessage("Your wounds are healed!");
      playerRef.Attrs.HP.Set(playerRef.Attrs.HP.OriginalValue);
    }
  }
  else
  {
    int percentage = (float)playerRef.Attrs.MP.CurrentValue * 0.1f;

    bool cond = (playerRef.Attrs.MP.OriginalValue == 0
              || playerRef.Attrs.MP.CurrentValue > percentage);
    if (cond)
    {
      Printer::Instance().AddMessage("... but nothing happens.");
    }
    else
    {
      Printer::Instance().AddMessage("Your spirit is reinforced!");
      playerRef.Attrs.MP.Set(playerRef.Attrs.MP.OriginalValue);
    }
  }
}
