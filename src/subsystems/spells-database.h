#ifndef SPELLSDATABASE_H
#define SPELLSDATABASE_H

#include "constants.h"
#include "spell-info.h"

class SpellsDatabase
{
  public:
    void RegisterSpell(const SpellInfo& i);

    SpellInfo* GetSpellInfoFromDatabase(SpellType t);

    void Init();

  private:
    std::unordered_map<SpellType, SpellInfo> _database;

    bool _initialized = false;
};

#endif // SPELLSDATABASE_H
