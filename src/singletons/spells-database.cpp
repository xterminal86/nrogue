#include "spells-database.h"

void SpellsDatabase::InitSpecific()
{
  RegisterSpell({
  SpellType::NONE, "Nothing", "N", 0, { 0, 0 }
                });

  RegisterSpell({
  SpellType::LIGHT, "Light", "L", 10
                });

  RegisterSpell({
  SpellType::STRIKE, "Strike", "S", 25, { 1, 4 }, Colors::WhiteColor
                });

  RegisterSpell({
  SpellType::FROST, "Frost", "F", 50, { 1, 4 }, "#4444FF"
                });

  RegisterSpell({
  SpellType::FIREBALL, "Fireball", "Fl", 100, { 2, 8 }, Colors::YellowColor
                });

  // Wand of piercing damage is actually (min + max)
  // and these values will be affected separately
  // by item quality and BUC status.
  RegisterSpell({
  SpellType::LASER, "Pierce", "P", 150, { 10, 10 }, Colors::WhiteColor
                });

  RegisterSpell({
  SpellType::LIGHTNING, "Lightning", "Lg", 100, { 2, 6 }, Colors::WhiteColor
                });

  RegisterSpell({
  SpellType::MAGIC_MISSILE, "Magic Missile", "Mg", 25, { 1, 6 }, "#AAAAFF"
                });

  RegisterSpell({
  SpellType::IDENTIFY, "Identify", "Id", 25
                });

  RegisterSpell({
  SpellType::MAGIC_MAPPING, "Magic Mapping", "MM", 80
                });

  RegisterSpell({
  SpellType::TELEPORT, "Teleport", "Te", 100, { 0, 0 }, Colors::WhiteColor
                });

  RegisterSpell({
  SpellType::TOWN_PORTAL, "Town Portal", "TP", 200
                });

  RegisterSpell({
  SpellType::DETECT_MONSTERS, "Detect Monsters", "DM", 80
                });

  RegisterSpell({
  SpellType::REMOVE_CURSE, "Remove Curse", "RC", 120
                });

  RegisterSpell({
  SpellType::REPAIR, "Repair", "R", 50
                });

  RegisterSpell({
  SpellType::HEAL, "Heal", "H", 100
                });

  RegisterSpell({
  SpellType::NEUTRALIZE_POISON, "Neutralize Poison", "NP", 50
                });

  RegisterSpell({
  SpellType::MANA_SHIELD, "Mana Shield", "MS", 200
                });
}

void SpellsDatabase::RegisterSpell(const SpellInfo& i)
{
  _database.emplace(i.SpellType_, i);
}

SpellInfo* SpellsDatabase::GetSpellInfoFromDatabase(SpellType t)
{
  if (_database.count(t) == 1)
  {
    return &_database.at(t);
  }

  return nullptr;
}
