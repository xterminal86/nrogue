#ifdef DEBUG_BUILD

#include "spell-info.h"

#include "util.h"

StringV SpellInfo::Dump(const std::string& name, size_t indent)
{
  const std::string spaces(indent, ' ');

  StringV res;

  res.push_back( I_OBJ_START_NAMED(spaces, name.data()) );

  res.push_back( I_INT(spaces, SpellType_) );
  res.push_back( I_STR(spaces, SpellName) );
  res.push_back( I_STR(spaces, SpellShortName) );
  res.push_back( I_INT(spaces, SpellBaseCost) );
  res.push_back( I_INT_NAMED(spaces, "SpellBaseDamage (min)", SpellBaseDamage.first) );
  res.push_back( I_INT_NAMED(spaces, "SpellBaseDamage (max)", SpellBaseDamage.second) );
  res.push_back( I_CLR(spaces, SpellProjectileColor) );

  res.push_back( I_OBJ_END(spaces) );

  return res;
}

#endif
