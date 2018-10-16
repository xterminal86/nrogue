#ifndef PLAYER_H
#define PLAYER_H

#include <ncurses.h>

#include <string>
#include <map>

#include "actor-component.h"
#include "game-object.h"
#include "container-component.h"

class Player
{
  public:

    enum class PlayerClass
    {
      SOLDIER = 0,
      THIEF,
      ARCANIST,
      CUSTOM
    };

    void Init();
    void Draw();
    void CheckVisibility();
    bool Move(int dx, int dy);

    int PosX;
    int PosY;
    chtype Avatar;
    std::string Color;
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

    void SubtractActionMeter()
    {
      ActionMeter -= 100;

      if (ActionMeter < 0)
      {
        ActionMeter = 0;
      }
    }

    int ActionMeter = 0;

    ActorComponent Actor;
    ContainerComponent Inventory;

    std::string Name = "Nameless One";

  private:
    void DiscoverCell(int x, int y);
    void SetAttributes();

    void SetSoldierAttrs();
    void SetThiefAttrs();
    void SetArcanistAttrs();

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
