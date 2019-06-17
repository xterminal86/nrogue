#include "effect.h"

Effect::Effect(EffectType t, int p, int to, bool cumulative, bool ignoresArmor)
{
  Type = t;
  Power = p;
  Duration = to;
  Cumulative = cumulative;
  ApplicationIgnoresArmor = ignoresArmor;
}

bool Effect::operator< (const Effect& rhs) const
{
  return (Type < rhs.Type);
}
