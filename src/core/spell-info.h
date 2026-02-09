#ifndef SPELLINFO_H
#define SPELLINFO_H

#include "constants.h"

struct SpellInfo
{
  SpellType SpellType_ = SpellType::NONE;
  std::string SpellName;
  std::string SpellShortName;
  int SpellBaseCost = 0;
  std::pair<int, int> SpellBaseDamage = { 0, 0 };
  uint32_t SpellProjectileColor = Colors::Magenta;

#ifdef DEBUG_BUILD
  StringV Dump(const std::string& name, size_t indent);
#endif
};

#endif // SPELLINFO_H
