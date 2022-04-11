#include "spells-database.h"

void SpellsDatabase::InitSpecific()
{
  auto& sn = GlobalConstants::SpellShortNameByType;

  RegisterSpell({
                  SpellType::NONE,
                  "Nothing",
                  sn.at(SpellType::NONE),
                  0,
                  { 0, 0 }
                });

  RegisterSpell({
                  SpellType::LIGHT,
                  "Light",
                  sn.at(SpellType::LIGHT),
                  10
                });

  RegisterSpell({
                  SpellType::STRIKE,
                  "Strike",
                  sn.at(SpellType::STRIKE),
                  25,
                  { 1, 4 },
                  Colors::WhiteColor
                });

  RegisterSpell({
                  SpellType::FROST,
                  "Frost",
                  sn.at(SpellType::FROST),
                  50,
                  { 1, 4 },
                  "#4444FF"
                });

  RegisterSpell({
                  SpellType::FIREBALL,
                  "Fireball",
                  sn.at(SpellType::FIREBALL),
                  100,
                  { 3, 10 },
                  Colors::YellowColor
                });

  RegisterSpell({
                  SpellType::LASER,
                  "Pierce",
                  sn.at(SpellType::LASER),
                  150,
                  { 10, 5 },
                  Colors::WhiteColor
                });

  RegisterSpell({
                  SpellType::LIGHTNING,
                  "Lightning",
                  sn.at(SpellType::LIGHTNING),
                  100,
                  { 3, 6 },
                  Colors::WhiteColor
                });

  RegisterSpell({
                  SpellType::MAGIC_MISSILE,
                  "Magic Missile",
                  sn.at(SpellType::MAGIC_MISSILE),
                  25,
                  { 2, 8 },
                  "#AAAAFF"
                });

  RegisterSpell({
                  SpellType::IDENTIFY,
                  "Identify",
                  sn.at(SpellType::IDENTIFY),
                  25
                });

  RegisterSpell({
                  SpellType::MAGIC_MAPPING,
                  "Magic Mapping",
                  sn.at(SpellType::MAGIC_MAPPING),
                  80
                });

  RegisterSpell({
                  SpellType::TELEPORT,
                  "Teleport",
                  sn.at(SpellType::TELEPORT),
                  100,
                  { 0, 0 },
                  Colors::WhiteColor
                });

  RegisterSpell({
                  SpellType::TOWN_PORTAL,
                  "Town Portal",
                  sn.at(SpellType::TOWN_PORTAL),
                  200
                });

  RegisterSpell({
                  SpellType::DETECT_MONSTERS,
                  "Detect Monsters",
                  sn.at(SpellType::DETECT_MONSTERS),
                  80
                });

  RegisterSpell({
                  SpellType::REMOVE_CURSE,
                  "Remove Curse",
                  sn.at(SpellType::REMOVE_CURSE),
                  120
                });

  RegisterSpell({
                  SpellType::REPAIR,
                  "Repair",
                  sn.at(SpellType::REPAIR),
                  50
                });

  RegisterSpell({
                  SpellType::HEAL,
                  "Heal",
                  sn.at(SpellType::HEAL),
                  100
                });

  RegisterSpell({
                  SpellType::NEUTRALIZE_POISON,
                  "Neutralize Poison",
                  sn.at(SpellType::NEUTRALIZE_POISON),
                  50
                });

  RegisterSpell({
                  SpellType::MANA_SHIELD,
                  "Mana Shield",
                  sn.at(SpellType::MANA_SHIELD),
                  200
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
