#include "effect.h"

Effect::Effect(EffectType t, int p, int to, bool cumulative, const std::string& extraInfo)
{
  Type = t;
  Power = p;
  Duration = to;
  Cumulative = cumulative;
  ExtraInfo = extraInfo;
}

bool Effect::operator< (const Effect& rhs) const
{
  return (Type < rhs.Type);
}
