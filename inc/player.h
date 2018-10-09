#ifndef PLAYER_H
#define PLAYER_H

#include <ncurses.h>

#include <string>
#include <map>

#include "actor-component.h"
#include "game-object.h"

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

  private:
    void DiscoverCell(int x, int y);
    void SetAttributes();

    void SetKnightAttrs();
    void SetRogueAttrs();
    void SetArcanistAttrs();

    ActorComponent _actorComponent;

    std::map<int, PlayerClass> _classesMap =
    {
      { 0, PlayerClass::SOLDIER },
      { 1, PlayerClass::THIEF },
      { 2, PlayerClass::ARCANIST },
      { 3, PlayerClass::CUSTOM }
    };

    GameObject* _previousCell = nullptr;
    GameObject* _currentCell = nullptr;
};

#endif // PLAYER_H
