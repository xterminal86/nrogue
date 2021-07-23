#ifndef SPELLSPROCESSOR_H
#define SPELLSPROCESSOR_H

#include "singleton.h"
#include "constants.h"

#include <string>

class Player;
class ItemComponent;

class SpellsProcessor : public Singleton<SpellsProcessor>
{
  public:
    void Init() override;

    void ProcessWand(ItemComponent* wand);
    void ProcessScroll(ItemComponent* scroll);

  private:
    void ProcessWandOfLight(ItemComponent* wand);

    void ProcessScrollOfLight(ItemComponent* scroll);
    void ProcessScrollOfMM(ItemComponent* scroll);
    void ProcessScrollOfHealing(ItemComponent* scroll);
    void ProcessScrollOfNeutralizePoison(ItemComponent* scroll);
    void ProcessScrollOfIdentify(ItemComponent* scroll);
    void ProcessScrollOfRepair(ItemComponent* scroll);
    void ProcessScrollOfDetectMonsters(ItemComponent* scroll);
    void ProcessScrollOfTownPortal(ItemComponent* scroll);
    void ProcessScrollOfTeleport(ItemComponent* scroll);
    void ProcessScrollOfManaShield(ItemComponent* scroll);
    void ProcessScrollOfRemoveCurse(ItemComponent* scroll);
    void ProcessScrollOfEnchantment(ItemComponent* scroll);

    Player* _playerRef = nullptr;

    const std::string _kNoActionText = "...but nothing happens.";

    // Calculate number of WaitForTurns for player before he can act (for SPD = 0)
    const int _kSkipsForTurn = ((float)GlobalConstants::TurnReadyValue / (float)GlobalConstants::TurnTickValue) + 1;

     // Default duration is 20 effective player turns
    const int _kDefaultDuration = _kSkipsForTurn * 20;
};

#endif // SPELLSPROCESSOR_H
