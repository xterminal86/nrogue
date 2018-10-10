#ifndef ACTORCOMPONENT_H
#define ACTORCOMPONENT_H

#include "component.h"

struct Attribute
{
  int CurrentValue;
  int OriginalValue;
  int Modifier;
  bool IsTalent = false;
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

  int Exp;
  int Level;

  int Hunger = 1000;
};

class ActorComponent : public Component
{
  public:
    void Update() override {}

  Attributes Attrs;
};

#endif // ACTORCOMPONENT_H
