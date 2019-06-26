#include "spells-database.h"

void SpellsDatabase::Init()
{
  // In case somebody will accidentally call Init() twice.
  if (!_initialized)
  {
    RegisterSpell({
    SpellType::LIGHT, "Light", "L", 10
                  });

    RegisterSpell({
    SpellType::STRIKE, "Strike", "S", 25, { 1, 4 }, "#FFFFFF"
                  });

    RegisterSpell({
    SpellType::FROST, "Frost", "F", 50, { 1, 4 }, "#4444FF"
                  });

    RegisterSpell({
    SpellType::FIREBALL, "Fireball", "Fl", 100, { 2, 8 }, "#FFFF00"
                  });

    RegisterSpell({
    SpellType::LASER, "Pierce", "P", 150, { 3, 10 }, "#FF0000"
                  });

    RegisterSpell({
    SpellType::LIGHTNING, "Lightning", "Lg", 100, { 2, 6 }, "#FFFFFF"
                  });

    RegisterSpell({
    SpellType::MAGIC_MISSILE, "Magic Missile", "MM", 25, { 1, 6 }, "#FFA500"
                  });

    RegisterSpell({
    SpellType::IDENTIFY, "Identify", "Id", 100
                  });

    RegisterSpell({
    SpellType::MAGIC_MAPPING, "Magic Mapping", "Mg", 80
                  });

    RegisterSpell({
    SpellType::TELEPORT, "Teleport", "Te", 100
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

    _initialized = true;
  }  
}

void SpellsDatabase::RegisterSpell(const SpellInfo& i)
{
  _database.emplace(i.SpellType_, i);
}

SpellInfo* SpellsDatabase::GetInfo(SpellType t)
{
  if (_database.count(t) == 1)
  {
    return &_database.at(t);
  }

  return nullptr;
}
