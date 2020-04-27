#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <unordered_map>
#include <vector>

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

  // For "custom" class, percentage of stat raise during level up
  int RaiseProbability = -1;

  private:
    // Some items may modify stat several times (e.g. dagger gives +SKL
    // but may also be magic that gives further modifier to SKL)
    std::unordered_map<int64_t, std::vector<int>> _modifiersByGoId;

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

  // number of turns before player becomes starving
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
