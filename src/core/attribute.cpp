#include "attribute.h"
#include "util.h"

void Attribute::Reset()
{
  _modifiersByGoId.clear();
  _originalValue = 0;
  Talents = 0;
}

// =============================================================================

void Attribute::Set(int value)
{
  _originalValue = value;
}

// =============================================================================

void Attribute::AddModifier(int64_t who, int value)
{
  _modifiersByGoId[who].push_back(value);
}

// =============================================================================

void Attribute::RemoveModifier(int64_t who)
{
  _modifiersByGoId.erase(who);
}

// =============================================================================

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

// =============================================================================

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

// =============================================================================

int Attribute::OriginalValue()
{
  return _originalValue;
}

// =============================================================================

void Attribute::Add(int value)
{
  _originalValue += value;
}

// -----------------------------------------------------------------------------

void RangedAttribute::Reset(int initialValue)
{
  _min.Reset();
  _max.Reset();

  _min.Set(initialValue);
  _max.Set(initialValue);

  Restore();
}

// =============================================================================

void RangedAttribute::SetMin(int valueToSet)
{
  _min.Set(valueToSet);
}

// =============================================================================

void RangedAttribute::SetMax(int valueToSet)
{
  _max.Set(valueToSet);
}

// =============================================================================

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

// =============================================================================

void RangedAttribute::AddMax(int valueToAdd)
{
  int res = _max.OriginalValue() + valueToAdd;
  if (res < 0)
  {
    res = 0;
  }

  _max.Set(res);

  //
  // If max value is dropped because of modifier,
  // adjust min value accordingly.
  //
  if (_min.OriginalValue() > _max.Get())
  {
    _min.Set(_max.Get());
  }
}

// =============================================================================

void RangedAttribute::Restore()
{
  _min.Set(_max.Get());
}

// =============================================================================

Attribute& RangedAttribute::Min()
{
  return _min;
}

// =============================================================================

Attribute& RangedAttribute::Max()
{
  return _max;
}

// =============================================================================

void RangedAttribute::CheckOverflow()
{
  if (_min.Get() > _max.Get())
  {
    _min.Set(_max.Get());
  }
}

// =============================================================================

bool RangedAttribute::IsFull()
{
  return (_min.Get() == _max.Get());
}

// -----------------------------------------------------------------------------

Attributes::Attributes()
{
  Lvl.Set(1);
}

// =============================================================================

int Attributes::Rating()
{
  int rating = 0;

  rating += ChallengeRating;

  rating += Str.OriginalValue();
  rating += Def.OriginalValue();
  rating += Mag.OriginalValue();
  rating += Res.OriginalValue();
  rating += Skl.OriginalValue();
  rating += Spd.OriginalValue();

  //
  // If stats are debuffed, clamp rating to 0 minimum.
  //
  if (rating < 0)
  {
    rating = 0;
  }

  return rating;
}

// =============================================================================

void Attributes::ResetStats()
{
  for (auto& kvp : _attrsMap)
  {
    kvp.second.Reset();
  }
}

#ifdef DEBUG_BUILD
StringV Attribute::Dump(const std::string& name, size_t indent)
{
  const std::string spaces(indent, ' ');

  StringV res;

  auto attrDump = DUMP_ATTR();

  auto str = Util::StringFormat(
    "%s'%s': '%d (%d), %d, %d, [%s]',",
    spaces.data(),
    name.data(),
    Get(),
    _originalValue,
    Talents,
    RaiseProbability,
    attrDump.data()
  );

  res.push_back(str);

  return res;
}

StringV RangedAttribute::Dump(const std::string& name, size_t indent)
{
  const std::string spaces(indent, ' ');

  StringV res;

  std::string totalStringMin, totalStringMax;

  auto dumpMin = _min.Dump(STRINGIFY(_min), indent + 2);
  for (auto& i : dumpMin)
  {
    totalStringMin += i;
  }

  auto dumpMax = _max.Dump(STRINGIFY(_max), indent + 2);
  for (auto& i : dumpMax)
  {
    totalStringMax += i;
  }

  res.push_back( I_OBJ_START_NAMED(spaces, name.data()) );

  res.push_back(
    Util::StringFormat(
      "%s  'data': '%d/%d, %d',",
      spaces.data(),
      _min.Get(),
      _max.Get(),
      Talents
    )
  );

  res.push_back(totalStringMin);
  res.push_back(totalStringMax);

  res.push_back( I_OBJ_END(spaces) );

  return res;
}

#define DA(name, attr, indent)          \
  {                                     \
    auto str = attr.Dump(name, indent); \
    for (auto& i : str)                 \
    {                                   \
      res.push_back(i);                 \
    }                                   \
  }

StringV Attributes::Dump(const std::string& name, size_t indent)
{
  const std::string spaces(indent, ' ');

  StringV res;

  res.push_back( I_OBJ_START_NAMED(spaces, name.data()) );

  DA(STRINGIFY(Str), Str, indent + 2)
  DA(STRINGIFY(Def), Def, indent + 2)
  DA(STRINGIFY(Mag), Mag, indent + 2)
  DA(STRINGIFY(Res), Res, indent + 2)
  DA(STRINGIFY(Skl), Skl, indent + 2)
  DA(STRINGIFY(Spd), Spd, indent + 2)

  DA(STRINGIFY(HP), HP, indent + 2)
  DA(STRINGIFY(MP), MP, indent + 2)

  DA(STRINGIFY(HungerRate), HungerRate, indent + 2)
  DA(STRINGIFY(HungerSpeed), HungerSpeed, indent + 2)

  DA(STRINGIFY(Exp), Exp, indent + 2)
  DA(STRINGIFY(Lvl), Lvl, indent + 2)

  res.push_back( I_INT(spaces, Hunger) );
  res.push_back( I_INT(spaces, ActionMeter) );
  res.push_back( I_INT(spaces, Rating()) );

  res.push_back( I_BOOL(spaces, Indestructible) );

  res.push_back( I_OBJ_END(spaces) );

  return res;
}

#endif
