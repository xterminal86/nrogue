#ifndef MAPLEVELDEEPDARK_H
#define MAPLEVELDEEPDARK_H

#include "map-level-base.h"

class MapLevelDeepDark : public MapLevelBase
{
  public:
    MapLevelDeepDark(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap() override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;
    void CreateSpecialLevel() override;

    void CreateCommonObjects(int x, int y, char image) override;
};

#endif // MAPLEVELDEEPDARK_H
