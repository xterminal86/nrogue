#ifndef EFFECT_H
#define EFFECT_H

enum class EffectType
{
  NONE = 0,
  ILLUMINATED,
  PARALYZED,
  POISONED,
  REGEN,
  BURNING,
  FROZEN,
  INFRAVISION,
  LEVITATE
};

struct Effect
{
  Effect() = default;

  Effect(EffectType t, int p, int to)
  {
    Type = t;
    Power = p;
    Timeout = to;
  }

  EffectType Type;
  int Power = 0;
  int Timeout = 0;
};

#endif // EFFECT_H
