#ifndef MAPLEVELDEEPDARK_H
#define MAPLEVELDEEPDARK_H

#include "map-level-base.h"

class MapLevelDeepDark : public MapLevelBase
{
  public:
    MapLevelDeepDark(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap(MapLevelBase* levelOwner) override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;
    void CreateSpecialLevel() override;
    void ConstructFromBuilder(LevelBuilder& lb) override;
};

#endif // MAPLEVELDEEPDARK_H
