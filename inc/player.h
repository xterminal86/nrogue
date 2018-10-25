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

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;
};

#endif // PLAYER_H
