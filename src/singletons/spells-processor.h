#ifndef SPELLSPROCESSOR_H
#define SPELLSPROCESSOR_H

#include "singleton.h"

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

    Player* _playerRef = nullptr;
};

#endif // SPELLSPROCESSOR_H
