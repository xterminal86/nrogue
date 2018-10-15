#ifndef ACTORCOMPONENT_H
#define ACTORCOMPONENT_H

#include "component.h"

struct Attribute
{
  int CurrentValue = 0;
  int OriginalValue = 0;
  int Modifier = 0;
  bool IsTalent = false;

  void Set(int value)
  {
    CurrentValue = value;
    OriginalValue = value;
  }

  int Get()
  {
    return CurrentValue + Modifier;
  }
};

struct Attributes
{
  Attribute Str;
  Attribute Def;
  Attribute Mag;
  Attribute Res;
  Attribute Skl;
  Attribute Spd;

  Attribute HP;
  Attribute MP;

  // number of turns before Hunger decrements by HungerSpeed
  Attribute HungerRate;
  Attribute HungerSpeed;

  int Exp = 0;
  int Level = 1;

  int Hunger = 1000;
};

class ActorComponent : public Component
{
  public:
    void Update() override {}

  Attributes Attrs;
};

#endif // ACTORCOMPONENT_H
