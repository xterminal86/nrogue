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
#include "item-component.h"
#include "position.h"
#include "dijkstra-map.h"

class AIComponent;

class Player : public GameObject
{
  public:
    Player() = default;

    void Init();

    bool Move(int dx, int dy);

    void CheckVisibility();
    bool TryToAttack(int dx, int dy);

    int SelectedClass;

    PlayerClass GetClass();

    std::string& GetClassName();

    void Draw();
    void MeleeAttack(GameObject* go, bool alwaysHit = false);
    void RangedAttack(GameObject* what, ItemComponent* with);
    void MagicAttack(GameObject* what, ItemComponent* with);
    void ReceiveDamage(GameObject* from, int amount, bool isMagical, bool godMode = false, bool suppressLog = false);
    void WaitForTurn();
    void ProcessHunger();
    void FinishTurn();

    bool IsAlive();

    ContainerComponent Inventory;

    std::string Name = "Nameless One";

    std::map<EquipmentCategory, std::vector<ItemComponent*>> EquipmentByCategory;

    void AwardExperience(int amount);
    void LevelUpSilent();
    void LevelUp(int baseHpOverride = -1) override;
    void LevelDown();

    void ApplyBonuses(ItemComponent* itemRef);
    void UnapplyBonuses(ItemComponent* itemRef);
    void RememberItem(ItemComponent* itemRef, const std::string& effect);
    void AddExtraItems();

    bool HasSkill(PlayerSkills skillToCheck);
    bool AreEnemiesInRange();
    bool RecallItem(ItemComponent* itemRef);

    int Money = 0;

    bool IsStarving = false;

    std::map<PlayerSkills, int> SkillLevelBySkill;

    std::map<std::string, int> TotalKills;

    DijkstraMap DistanceField;

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
    void ProcessEffects();
    void ProcessAttack(ItemComponent* weapon, GameObject* defender, int damageToInflict);
    void ProcessMagicAttack(GameObject* target, ItemComponent* weapon, int damage, bool againstRes);
    void ProcessTeleport(GameObject* target, ItemComponent* weapon);

    bool WeaponLosesDurability();
    bool ShouldBreak(ItemComponent* ic);
    bool PassByNPC(GameObject* actor);

    void BreakItem(ItemComponent* ic, bool suppressMessage = false);

    void ProcessEffectsPlayer();
    void ProcessItemsEffects();

    void SwitchPlaces(AIComponent* other);

    int CalculateDamageValue(ItemComponent* weapon, GameObject* defender, bool meleeAttackWithRangedWeapon);
    bool WasHitLanded(GameObject* defender);

    bool IsGameObjectBorder(GameObject* go);

    bool DamageArmor(GameObject* from, int amount);

    void ApplyBonus(ItemComponent* itemRef, const ItemBonusStruct& bonus);
    void UnapplyBonus(ItemComponent* itemRef, const ItemBonusStruct& bonus);

    int GetDamageAbsorbtionValue(bool magic);
    std::vector<ItemComponent*> GetItemsWithBonus(const ItemBonusType& bonusType);

    int _starvingTimeout = 0;
    int _useIdentifiedMapSortingIndex = 0;

    std::vector<std::string> GetPrettyLevelUpText();

    void KnockBack(GameObject* go, int tiles);

    std::string ProcessMagicalDamage(GameObject* from, int& amount);
    std::string ProcessPhysicalDamage(GameObject* from, int& amount);

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

    // std::map automatically sorts by key, so in case of string key,
    // it's lexicographical sorting
    //
    // That's why I couldn't figure out for a while why my values in the map
    // are suddenly in the wrong order during for loop.
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

    friend class SpellsProcessor;
    friend class AttackState;
    friend class ServiceState;
    friend class InfoState;
};

#endif // PLAYER_H
