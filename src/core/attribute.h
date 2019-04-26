#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

struct Attribute
{
  int CurrentValue = 0;
  int OriginalValue = 0;
  int Modifier = 0;
  int Talents = 0;

  void Set(int value);
  void Add(int value);

  /// Returns value including modifier
  int Get(bool originalValue = true);
};

struct Attributes
{
  Attributes();

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

  Attribute Exp;
  Attribute Lvl;

  int Hunger = 0;
  int ActionMeter = 0;

  bool Indestructible = true;

  int Rating();
};

#endif // ATTRIBUTE_H
