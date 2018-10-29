#ifndef PLAYER_H
#define PLAYER_H

#include <ncurses.h>

#include <string>
#include <map>

#include "constants.h"
#include "game-object.h"
#include "container-component.h"
#include "item-component.h"

class Player : public GameObject
{
  public:

    enum class PlayerClass
    {
      SOLDIER = 0,
      THIEF,
      ARCANIST,
      CUSTOM
    };

    const int kInventorySize = 20;

    Player() {}

    void Init();

    bool Move(int dx, int dy);

    void CheckVisibility();
    bool TryToAttack(int dx, int dy);

    int VisibilityRadius;

    int SelectedClass;

    PlayerClass GetClass()
    {
      return _classesMap[SelectedClass];
    }

    std::string& GetClassName()
    {
      return _classesName[SelectedClass];
    }

    void Draw();
    void Attack(GameObject* go);
    void ReceiveDamage(GameObject* from, int amount);
    void WaitForTurn();

    ContainerComponent Inventory;

    std::string Name = "Nameless One";

    std::map<EquipmentCategory, std::vector<ItemComponent*>> EquipmentByCategory;

    void AwardExperience(int amount);
    void LevelUp();

  private:
    void DiscoverCell(int x, int y);
    void SetAttributes();
    void SetDefaultEquipment();

    void SetSoldierAttrs();
    void SetThiefAttrs();
    void SetArcanistAttrs();

    bool CanRaiseAttribute(Attribute& attr);
    void ProcessKill(GameObject* monster);

    std::map<int, PlayerClass> _classesMap =
    {
      { 0, PlayerClass::SOLDIER },
      { 1, PlayerClass::THIEF },
      { 2, PlayerClass::ARCANIST },
      { 3, PlayerClass::CUSTOM }
    };

    std::map<int, std::string> _classesName =
    {
      { 0, "Soldier" },
      { 1, "Thief" },
      { 2, "Arcanist" },
      { 3, "Unknown" }
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
      { "HP", 0 },
      { "MP", 0 }
    };

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;
};

#endif // PLAYER_H
