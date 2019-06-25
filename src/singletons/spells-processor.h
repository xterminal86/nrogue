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

    void IlluminatePlayer(int pow, int dur);

  private:
    void ProcessWandOfLight(ItemComponent* wand);

    void ProcessScrollOfLight(ItemComponent* scroll);
    void ProcessScrollOfMM(ItemComponent* scroll);
    void ProcessScrollOfHealing(ItemComponent* scroll);
    void ProcessScrollOfNeutralizePoison(ItemComponent* scroll);
    void ProcessScrollOfIdentify(ItemComponent* scroll);
    void ProcessScrollOfRepair(ItemComponent* scroll);

    Player* _playerRef = nullptr;
};

#endif // SPELLSPROCESSOR_H
