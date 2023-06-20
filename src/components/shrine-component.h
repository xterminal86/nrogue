#ifndef SHRINECOMPONENT_H
#define SHRINECOMPONENT_H

#include "constants.h"

#include "component.h"

class ShrineComponent : public Component
{
  public:
    ShrineComponent(ShrineType shrineType, int timeout, bool oneTimeUse = true);

    void Update() override;

    IR Interact();

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

    int _timeout = -1;
    int _counter = 0;

    bool _oneTimeUse = true;

    ShrineType _type = ShrineType::NONE;

    // -------------------------------------------------------------------------

    const std::vector<ItemBonusType> _attrs =
    {
      { ItemBonusType::STR },
      { ItemBonusType::DEF },
      { ItemBonusType::MAG },
      { ItemBonusType::RES },
      { ItemBonusType::SPD },
      { ItemBonusType::SKL }
    };

    // TODO: moar?
    const std::vector<ItemBonusType> _positiveEffects =
    {
      ItemBonusType::MANA_SHIELD,
      ItemBonusType::ILLUMINATED,
      ItemBonusType::REGEN,
      ItemBonusType::REFLECT,
      ItemBonusType::TELEPATHY,
      ItemBonusType::TRUE_SEEING,
      ItemBonusType::INVISIBILITY,
      ItemBonusType::LEVITATION
    };

    const std::vector<ItemBonusType> _negativeEffects =
    {
      ItemBonusType::PARALYZE,
      ItemBonusType::POISONED,
      ItemBonusType::BURNING,
      ItemBonusType::FROZEN,
      ItemBonusType::BLINDNESS,
      ItemBonusType::WEAKNESS
    };
};

#endif // SHRINECOMPONENT_H
