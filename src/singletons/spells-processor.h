#ifndef SPELLSPROCESSOR_H
#define SPELLSPROCESSOR_H

#include "singleton.h"
#include "constants.h"

#include <string>

class Player;
class ItemComponent;
class GameObject;

class SpellsProcessor : public Singleton<SpellsProcessor>
{
  public:
    void ProcessWand(ItemComponent* wand);
    void ProcessScroll(ItemComponent* scroll, GameObject* user);

  protected:
    void InitSpecific() override;

  private:
    void ProcessWandOfLight(ItemComponent* wand);

    void ProcessScrollOfLight(ItemComponent* scroll, GameObject* user);
    void ProcessScrollOfMM(ItemComponent* scroll, GameObject* user);
    void ProcessScrollOfHealing(ItemComponent* scroll, GameObject* user);
    void ProcessScrollOfNeutralizePoison(ItemComponent* scroll, GameObject* user);
    void ProcessScrollOfIdentify(ItemComponent* scroll, GameObject* user);
    void ProcessScrollOfRepair(ItemComponent* scroll, GameObject* user);
    void ProcessScrollOfHiddenDetection(ItemComponent* scroll,
                                        GameObject* user,
                                        ItemBonusType type);
    void ProcessScrollOfTownPortal(ItemComponent* scroll, GameObject* user);
    void ProcessScrollOfTeleport(ItemComponent* scroll, GameObject* user);
    void ProcessScrollOfManaShield(ItemComponent* scroll, GameObject* user);
    void ProcessScrollOfRemoveCurse(ItemComponent* scroll, GameObject* user);

    void PrintUsageResult(ItemComponent* scroll, GameObject* user);

    Player* _playerRef = nullptr;

    const std::string _kNoActionText = "...but nothing happens.";

    std::vector<std::string> _scrollUseMessages;
};

#endif // SPELLSPROCESSOR_H
