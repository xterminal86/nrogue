#ifndef EFFECT_H
#define EFFECT_H

#include <map>
#include <vector>

enum class EffectType
{
  NONE = 0,  
  MANA_SHIELD,
  ILLUMINATED,  // Visibility radius increased
  PARALYZED,    // Cannot move or act
  POISONED,     // Take damage after each turn
  REGEN,        // Gain HP each turn
  REFLECT,      // Hurl offensive (NOTE: not only?) spell back at the attacker
  BURNING,      // Take damage each turn, gain illuminated
  FROZEN,       // SPD penalty
  TELEPATHY,    // See actors in visibility radius through walls
  BLINDNESS,    // Reduced visibility radius
  INVISIBLE,    // Monsters can't see you (unless they can via telepathy)  
  LEVITATE      // Nuff said
};

static const std::map<EffectType, std::string> EffectNameByType =
{
  { EffectType::MANA_SHIELD, "Shi" },
  { EffectType::ILLUMINATED, "Lgt" },
  { EffectType::PARALYZED,   "Par" },
  { EffectType::POISONED,    "Psd" },
  { EffectType::REGEN,       "Reg" },
  { EffectType::REFLECT,     "Ref" },
  { EffectType::BURNING,     "Bur" },
  { EffectType::FROZEN,      "Frz" },
  { EffectType::TELEPATHY,   "Tel" },
  { EffectType::BLINDNESS,   "Bli" },
  { EffectType::INVISIBLE,   "Hid" },
  { EffectType::LEVITATE,    "Fly" },
};

static const std::vector<EffectType> PositiveEffects =
{
  EffectType::MANA_SHIELD,
  EffectType::ILLUMINATED,
  EffectType::REGEN,
  EffectType::REFLECT,
  EffectType::TELEPATHY,
  EffectType::INVISIBLE,
  EffectType::LEVITATE
};

static const std::vector<EffectType> NegativeEffects =
{
  EffectType::PARALYZED,
  EffectType::POISONED,
  EffectType::BURNING,
  EffectType::FROZEN,
  EffectType::BLINDNESS
};

struct Effect
{
  Effect() = default;

  Effect(EffectType t, int p, int to, bool cumulative);

  bool operator< (const Effect& rhs) const;

  EffectType Type = EffectType::NONE;

  // Taken into account for calculating amount of
  // powah effect does (e.g. damage from poison)
  //
  // (NOTE: can affect Timeout as well?)
  int Power = 0;

  // After this amount of turns passed, effect autoremoves itself.
  // -1 means it lasts until removed explicitly.
  int Duration = 0;

  bool Cumulative = false;

  // Item that gives this effect or -1 of none
  int64_t ObjectId = -1;
};

#endif // EFFECT_H
