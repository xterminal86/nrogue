#ifndef SHRINECOMPONENT_H
#define SHRINECOMPONENT_H

#include "constants.h"

#include "component.h"

// TODO: moar?
static const std::vector<ItemBonusType> PositiveEffects =
{
  ItemBonusType::MANA_SHIELD,
  ItemBonusType::ILLUMINATED,
  ItemBonusType::REGEN,
  ItemBonusType::REFLECT,
  ItemBonusType::TELEPATHY,
  ItemBonusType::INVISIBILITY,
  ItemBonusType::LEVITATION
};

static const std::vector<ItemBonusType> NegativeEffects =
{
  ItemBonusType::PARALYZE,
  ItemBonusType::POISONED,
  ItemBonusType::BURNING,
  ItemBonusType::FROZEN,
  ItemBonusType::BLINDNESS
};

class ShrineComponent : public Component
{
  public:
    ShrineComponent();

    void Update() override;

    void Interact();

    int Timeout = -1;
    int Counter = 0;

    ShrineType Type;

  private:
    void Activate();    
    void ProcessEffect();
    void ApplyRandomEffect(std::string& logMessageToWrite);
    void ApplyRandomPositiveEffect(std::string& logMessageToWrite);
    void ApplyRandomNegativeEffect(std::string& logMessageToWrite);
    void ApplyTemporaryStatRaise(std::string& logMessageToWrite);
    void SetEffectGainMessage(std::string& logMessageToWrite, ItemBonusType e);    

    int _power;
    int _duration;
};

#endif // SHRINECOMPONENT_H
