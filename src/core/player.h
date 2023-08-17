#ifndef PLAYER_H
#define PLAYER_H

#ifndef USE_SDL
#include <ncurses.h>
#endif

#include <string>
#include <map>

#include "constants.h"
#include "game-object.h"
#include "container-component.h"
#include "equipment-component.h"
#include "item-component.h"
#include "position.h"
#include "potential-field.h"

class AIComponent;

class Player : public GameObject
{
  public:
    Player() = default;

    void Init();

    bool Move(int dx, int dy);

    void CheckVisibility();

    bool TryToMeleeAttack(int dx, int dy);
    bool IsSwimming();

    int SelectedClass;

    PlayerClass GetClass();

    std::string& GetClassName();

    void Draw();
    void MeleeAttack(GameObject* what, bool alwaysHit = false);
    void RangedAttack(GameObject* what, ItemComponent* with);
    void MagicAttack(GameObject* what, ItemComponent* with);
    bool ReceiveDamage(GameObject* from,
                       int amount,
                       bool isMagical,
                       bool ignoreArmor,
                       bool directDamage,
                       bool suppressLog) override;
    void WaitForTurn();
    void ProcessHunger();
    void FinishTurn();

    void SetDestroyed();

    ContainerComponent* Inventory;
    EquipmentComponent* Equipment;

    std::string Name = "Nameless One";

    void AwardExperience(int amount) override;
    void LevelUpSilent();
    void LevelUp(int baseHpOverride = -1) override;
    void LevelDown() override;
    void LevelDownSilent();

    void AddExtraItems();

    void SetAttackDir(const Position& dir);
    void SetKnockBackDir(const Position& dir);

    bool HasSkill(PlayerSkills skillToCheck);

    bool IsStarving = false;

    std::unordered_map<PlayerSkills, int> SkillLevelBySkill;

    std::unordered_map<std::string, int> TotalKills;

    PotentialField DistanceField;

    #ifdef DEBUG_BUILD
    bool ToggleFogOfWar = false;
    bool GodMode = false;
    #endif

  private:
    void DiscoverCell(int x, int y);
    void SetAttributes();
    void SetDefaultEquipment();
    void SetDefaultItems();
    void SetDefaultSkills();

    void SetSoldierDefaultItems();
    void SetThiefDefaultItems();
    void SetArcanistDefaultItems();

    void SetSoldierAttrs();
    void SetThiefAttrs();
    void SetArcanistAttrs();
    void SetCustomClassAttrs();

    void ProcessKill(GameObject* monster);

    void ProcessStarvation();
    void ProcessMeleeAttack(ItemComponent* weapon, GameObject* defender, int damageToInflict);
    void ProcessMagicAttack(GameObject* target, ItemComponent* weapon, int damage, bool againstRes);

    bool PassByNPC(GameObject* actor);

    void ProcessEffectsPlayer();

    void SwitchPlaces(AIComponent* other);

    void ApplyStarvingPenalties();
    void UnapplyStarvingPenalties();

    bool IsGameObjectBorder(GameObject* go);

    void PrintLevelUpResultsToLog(bool reallyUp);

    int _starvingTimeout = 0;

    std::vector<std::string> GetPrettyLevelUpText();

    std::unordered_map<int, PlayerClass> _classesMap =
    {
      { 0, PlayerClass::SOLDIER  },
      { 1, PlayerClass::THIEF    },
      { 2, PlayerClass::ARCANIST },
      { 3, PlayerClass::CUSTOM   }
    };

    std::unordered_map<int, std::string> _classesName =
    {
      { 0, "Soldier"  },
      { 1, "Thief"    },
      { 2, "Arcanist" },
      { 3, "Unknown"  }
    };

    //
    // std::map automatically sorts by key, so in case of string key,
    // it's lexicographical sorting.
    //
    // That's why I couldn't figure out for a while why my values in the map
    // are suddenly in the wrong order during for loop.
    //
    // ...yeah, I know, right?
    //
    const std::map<PlayerStats, std::pair<std::string, Attribute&>> _mainAttributesByStatName =
    {
      { PlayerStats::STR, { "STR", Attrs.Str } },
      { PlayerStats::DEF, { "DEF", Attrs.Def } },
      { PlayerStats::MAG, { "MAG", Attrs.Mag } },
      { PlayerStats::RES, { "RES", Attrs.Res } },
      { PlayerStats::SKL, { "SKL", Attrs.Skl } },
      { PlayerStats::SPD, { "SPD", Attrs.Spd } }
    };

    std::map<PlayerStats, std::pair<std::string, int>> _statRaisesMap =
    {
      { PlayerStats::STR, { "STR", 0 } },
      { PlayerStats::DEF, { "DEF", 0 } },
      { PlayerStats::MAG, { "MAG", 0 } },
      { PlayerStats::RES, { "RES", 0 } },
      { PlayerStats::SKL, { "SKL", 0 } },
      { PlayerStats::SPD, { "SPD", 0 } },
      { PlayerStats::HP,  { "HP",  0 } },
      { PlayerStats::MP,  { "MP",  0 } }
    };

    Position _attackDir;
    Position _knockBackDir;

    friend class SpellsProcessor;
    friend class ServiceState;
    friend class InfoState;
};

#endif // PLAYER_H
