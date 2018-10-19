#include "shrine-component.h"

#include "game-object.h"
#include "util.h"

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
    message = Util::StringFormat("You pray to %s", saint.data());
    Printer::Instance().AddMessage(message);
  }
}

void ShrineComponent::Activate()
{
  ((GameObject*)OwnerGameObject)->FgColor = (Type == ShrineType::MIGHT) ? GlobalConstants::ShrineMightColor : GlobalConstants::ShrineSpiritColor;
}
