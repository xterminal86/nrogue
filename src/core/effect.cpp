#include "effect.h"

Effect::Effect(EffectType t, int p, int to, bool cumulative)
{
  Type = t;
  Power = p;
  Duration = to;
  Cumulative = cumulative;
}

bool Effect::operator< (const Effect& rhs) const
{
  return (Type < rhs.Type);
}
