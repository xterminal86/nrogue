#ifndef SPELLSDATABASE_H
#define SPELLSDATABASE_H

#include "singleton.h"
#include "constants.h"

struct SpellInfo
{
  SpellType SpellType_ = SpellType::NONE;
  std::string SpellName;
  std::string SpellShortName;  
  int SpellBaseCost = 0;
  std::pair<int, int> SpellBaseDamage;
  std::string SpellProjectileColor;
};

class SpellsDatabase : public Singleton<SpellsDatabase>
{
  public:
    void Init() override;

    void RegisterSpell(const SpellInfo& i);

    SpellInfo* GetInfo(SpellType t);

  private:    
    std::map<SpellType, SpellInfo> _database;
};

#endif // SPELLSDATABASE_H
