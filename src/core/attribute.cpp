#include "attribute.h"
#include "util.h"

void Attribute::Set(int value)
{
  OriginalValue = value;
  CurrentValue = value;
}

void Attribute::Add(int value)
{
  CurrentValue += value;

  CurrentValue = Util::Clamp(CurrentValue, 0, OriginalValue);
}

int Attribute::Get(bool originalValue)
{
  int res = originalValue ? OriginalValue + Modifier : CurrentValue + Modifier;

  /*
  if (res < 0)
  {
    res = 0;
  }
  */

  return res;
}

// *******************************************************

Attributes::Attributes()
{
  Lvl.Set(1);
}

int Attributes::Rating()
{
  int rating = 0;

  rating += Str.OriginalValue;
  rating += Def.OriginalValue;
  rating += Mag.OriginalValue;
  rating += Res.OriginalValue;
  rating += Skl.OriginalValue;
  rating += Spd.OriginalValue;
  rating += Lvl.CurrentValue;

  return rating;
}
