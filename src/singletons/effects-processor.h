#ifndef EFFECTSPROCESSOR_H
#define EFFECTSPROCESSOR_H

#include "singleton.h"

class Player;
class ItemComponent;

class EffectsProcessor : public Singleton<EffectsProcessor>
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

#endif // EFFECTSPROCESSOR_H
