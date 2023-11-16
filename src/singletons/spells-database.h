#ifndef SPELLSDATABASE_H
#define SPELLSDATABASE_H

#include "singleton.h"
#include "constants.h"

class ItemComponent;

struct SpellInfo
{
  SpellType SpellType_ = SpellType::NONE;
  std::string SpellName;
  std::string SpellShortName;
  int SpellBaseCost = 0;
  std::pair<int, int> SpellBaseDamage = { 0, 0 };
  uint32_t SpellProjectileColor = Colors::MagentaColor;
};

class SpellsDatabase : public Singleton<SpellsDatabase>
{
  public:
    void RegisterSpell(const SpellInfo& i);

    SpellInfo* GetSpellInfoFromDatabase(SpellType t);

  protected:
    void InitSpecific() override;

  private:
    std::unordered_map<SpellType, SpellInfo> _database;
};

#endif // SPELLSDATABASE_H
