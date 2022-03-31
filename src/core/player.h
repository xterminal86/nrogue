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

    bool TryToAttack(int dx, int dy);
    bool IsSwimming();

    int SelectedClass;

    PlayerClass GetClass();

    std::string& GetClassName();

    void Draw();
    void MeleeAttack(GameObject* go, bool alwaysHit = false);
    void RangedAttack(GameObject* what, ItemComponent* with);
    void MagicAttack(GameObject* what, ItemComponent* with);
    void ReceiveDamage(GameObject* from,
                       int amount,
                       bool isMagical,
                       bool directDamage = false,
                       bool godMode = false,
                       bool suppressLog = false);
    void WaitForTurn();
    void ProcessHunger();
    void FinishTurn();

    bool IsAlive();

    ContainerComponent* Inventory;
    EquipmentComponent* Equipment;

    std::string Name = "Nameless One";

    void AwardExperience(int amount) override;
    void LevelUpSilent();
    void LevelUp(int baseHpOverride = -1) override;
    void LevelDown() override;

    void RememberItem(ItemComponent* itemRef, const std::string& effect);
    void AddExtraItems();

    void SetAttackDir(const Position& dir);
    void SetKnockBackDir(const Position& dir);

    bool HasSkill(PlayerSkills skillToCheck);
    bool AreEnemiesInRange();
    bool RecallItem(ItemComponent* itemRef);

    int Money = 0;

    bool IsStarving = false;

    std::map<PlayerSkills, int> SkillLevelBySkill;

    std::map<std::string, int> TotalKills;

    PotentialField DistanceField;

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
    void ProcessTeleport(GameObject* target, ItemComponent* weapon);

    bool WeaponLosesDurability();
    bool ShouldBreak(ItemComponent* ic);
    bool PassByNPC(GameObject* actor);

    void BreakItem(ItemComponent* ic, bool suppressMessage = false);

    void ProcessEffectsPlayer();
    void ProcessItemsEffects();

    void SwitchPlaces(AIComponent* other);

    bool WasHitLanded(GameObject* defender);

    bool IsGameObjectBorder(GameObject* go);

    int _starvingTimeout = 0;
    int _useIdentifiedMapSortingIndex = 0;

    std::vector<std::string> GetPrettyLevelUpText();

    std::map<int, std::pair<std::string, std::string>> _useIdentifiedItemsByIndex;

    std::map<int, PlayerClass> _classesMap =
    {
      { 0, PlayerClass::SOLDIER  },
      { 1, PlayerClass::THIEF    },
      { 2, PlayerClass::ARCANIST },
      { 3, PlayerClass::CUSTOM   }
    };

    std::map<int, std::string> _classesName =
    {
      { 0, "Soldier"  },
      { 1, "Thief"    },
      { 2, "Arcanist" },
      { 3, "Unknown"  }
    };

    //
    // std::map automatically sorts by key, so in case of string key,
    // it's lexicographical sorting
    //
    // That's why I couldn't figure out for a while why my values in the map
    // are suddenly in the wrong order during for loop.
    //
    // ...yeah, I know, right?
    //
    std::map<int, std::pair<std::string, Attribute&>> _mainAttributes =
    {
      { 0, { "STR", Attrs.Str } },
      { 1, { "DEF", Attrs.Def } },
      { 2, { "MAG", Attrs.Mag } },
      { 3, { "RES", Attrs.Res } },
      { 4, { "SKL", Attrs.Skl } },
      { 5, { "SPD", Attrs.Spd } }
    };

    std::map<std::string, int> _statRaisesMap =
    {
      { "STR", 0 },
      { "DEF", 0 },
      { "MAG", 0 },
      { "RES", 0 },
      { "SKL", 0 },
      { "SPD", 0 },
      { "HP",  0 },
      { "MP",  0 }
    };

    Position _attackDir;
    Position _knockBackDir;

    friend class SpellsProcessor;
    friend class ServiceState;
    friend class InfoState;
};

#endif // PLAYER_H
