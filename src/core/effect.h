#ifndef EFFECT_H
#define EFFECT_H

#include <map>

enum class EffectType
{
  NONE = 0,  
  ILLUMINATED,  // Visibility radius increased
  PARALYZED,    // Cannot move or act
  POISONED,     // Take damage after each turn
  REGEN,        // Gain HP each turn
  REFLECT,      // Hurl offensive (NOTE: not only?) spell back at the attacker
  BURNING,      // Take damage each turn, gain illuminated
  FROZEN,       // SPD penalty
  INFRAVISION,  // See actors in visibility radius through walls
  BLINDNESS,    // Reduced visibility radius
  INVISIBLE,    // Monsters can't see you (unless they can via infravision)
  LEVITATE      // Nuff said
};

static const std::map<EffectType, std::string> EffectNameByType =
{
  { EffectType::ILLUMINATED, "Lgt" },
  { EffectType::PARALYZED,   "Par" },
  { EffectType::POISONED,    "Psd" },
  { EffectType::REGEN,       "Reg" },
  { EffectType::REFLECT,     "Ref" },
  { EffectType::BURNING,     "Bur" },
  { EffectType::FROZEN,      "Frz" },
  { EffectType::INFRAVISION, "Ivs" },
  { EffectType::BLINDNESS,   "Bli" },
  { EffectType::INVISIBLE,   "Hid" },
  { EffectType::LEVITATE,    "Lev" },
};

struct Effect
{
  Effect() = default;

  Effect(EffectType t, int p, int to);

  EffectType Type = EffectType::NONE;

  // Taken into account for calculating amount of
  // powah effect does (e.g. damage from poison)
  //
  // (NOTE: can affect Timeout as well?)
  int Power = 0;

  // After this amount of turns passed, effect autoremoves itself.
  // -1 means it lasts until removed explicitly.
  int Duration = 0;
};

#endif // EFFECT_H