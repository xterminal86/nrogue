#include "attribute.h"
#include "util.h"

void Attribute::Reset()
{
  _modifiersByGoId.clear();
  _originalValue = 0;
  Talents = 0;
}

void Attribute::Set(int value)
{
  _originalValue = value;
}

void Attribute::AddModifier(int64_t who, int value)
{
  _modifiersByGoId[who].push_back(value);
}

void Attribute::RemoveModifier(int64_t who)
{
  _modifiersByGoId.erase(who);
}

int Attribute::Get()
{
  int res = _originalValue;

  for (auto& kvp : _modifiersByGoId)
  {
    for (auto& v : kvp.second)
    {
      res += v;
    }
  }

  return res;
}

int Attribute::GetModifiers()
{
  int res = 0;

  for (auto& kvp : _modifiersByGoId)
  {
    for (auto& v : kvp.second)
    {
      res += v;
    }
  }

  return res;
}

int Attribute::OriginalValue()
{
  return _originalValue;
}

void Attribute::Add(int value)
{
  _originalValue += value;
}

// *****************************************************************************

void RangedAttribute::Reset(int initialValue)
{
  _min.Reset();
  _max.Reset();

  _min.Set(initialValue);
  _max.Set(initialValue);

  Restore();
}

void RangedAttribute::SetMin(int valueToSet)
{
  _min.Set(valueToSet);
}

void RangedAttribute::SetMax(int valueToSet)
{
  _max.Set(valueToSet);
}

void RangedAttribute::AddMin(int valueToAdd)
{
  int res = _min.OriginalValue() + valueToAdd;
  if (res > _max.Get())
  {
    res = _max.Get();
  }
  else if (res < 0)
  {
    res = 0;
  }

  _min.Set(res);
}

void RangedAttribute::AddMax(int valueToAdd)
{
  int res = _max.OriginalValue() + valueToAdd;
  if (res < 0)
  {
    res = 0;
  }

  _max.Set(res);

  // If max value is dropped because of modifier,
  // adjust min value accordingly.
  if (_min.OriginalValue() > _max.Get())
  {
    _min.Set(_max.Get());
  }
}

void RangedAttribute::Restore()
{
  _min.Set(_max.Get());
}

Attribute& RangedAttribute::Min()
{
  return _min;
}

Attribute& RangedAttribute::Max()
{
  return _max;
}

void RangedAttribute::CheckOverflow()
{
  if (_min.Get() > _max.Get())
  {
    _min.Set(_max.Get());
  }
}

bool RangedAttribute::IsFull()
{
  return (_min.Get() == _max.Get());
}

// *****************************************************************************

Attributes::Attributes()
{
  Lvl.Set(1);
}

int Attributes::Rating()
{
  int rating = 0;

  rating += Str.Get();
  rating += Def.Get();
  rating += Mag.Get();
  rating += Res.Get();
  rating += Skl.Get();
  rating += Spd.Get();

  return rating;
}

void Attributes::ResetStats()
{
  for (auto& kvp : _attrsMap)
  {
    kvp.second.Reset();
  }
}
