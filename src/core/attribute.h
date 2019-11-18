#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <unordered_map>

struct Attribute
{  
  void Reset();
  void Set(int value);
  void Add(int value);
  void AddModifier(int64_t who, int value);
  void RemoveModifier(int64_t who);

  int GetModifiers();
  int Get();
  int OriginalValue();

  int Talents = 0;

  private:
    std::unordered_map<int64_t, int> _modifiersByGoId;

    int _originalValue = 0;
};

struct RangedAttribute
{
  void Reset(int initialValue);
  void SetMin(int valueToSet);
  void SetMax(int valueToSet);
  void AddMin(int valueToAdd);
  void AddMax(int valueToAdd);
  void CheckOverflow();
  void Restore();

  Attribute& Min();
  Attribute& Max();

  int Talents = 0;

  private:
    Attribute _min;
    Attribute _max;
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

  RangedAttribute HP;
  RangedAttribute MP;

  // number of turns before Hunger decrements by HungerSpeed
  Attribute HungerRate;

  Attribute HungerSpeed;

  RangedAttribute Exp;
  Attribute Lvl;

  int Hunger = 0;
  int ActionMeter = 0;

  bool Indestructible = true;

  int Rating();
};

#endif // ATTRIBUTE_H
