#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <map>

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

  //
  // For "custom" class, percentage of stat raise during level up.
  //
  int RaiseProbability = -1;

  private:
    //
    // Some items may modify stat several times (e.g. dagger gives +SKL
    // but may also be magic that gives further modifier to SKL)
    //
    std::unordered_map<int64_t, std::vector<int>> _modifiersByGoId;

    int _originalValue = 0;
};

// =============================================================================

struct RangedAttribute
{
  void Reset(int initialValue);
  void SetMin(int valueToSet);
  void SetMax(int valueToSet);
  void AddMin(int valueToAdd);
  void AddMax(int valueToAdd);
  void CheckOverflow();
  void Restore();

  bool IsFull();

  Attribute& Min();
  Attribute& Max();

  int Talents = 0;

  private:
    Attribute _min;
    Attribute _max;
};

// =============================================================================

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

  //
  // Number of turns before player becomes starving
  //
  Attribute HungerRate;

  Attribute HungerSpeed;

  RangedAttribute Exp;
  Attribute Lvl;

  int Hunger      = 0;
  int ActionMeter = 0;

  //
  // Additional value for rating.
  //
  int ChallengeRating = 0;

  bool Indestructible = true;

  int Rating();

  void ResetStats();

  private:
    std::map<int, Attribute&> _attrsMap =
    {
      { 1, Str },
      { 2, Def },
      { 3, Mag },
      { 4, Res },
      { 5, Skl },
      { 6, Spd },
    };
};

#endif // ATTRIBUTE_H
