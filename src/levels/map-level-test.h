#ifndef MAPLEVELTEST_H
#define MAPLEVELTEST_H

#include "map-level-base.h"

class MapLevelTest : public MapLevelBase
{
  public:
    MapLevelTest(int sizeX, int sizeY, MapType type, int dungeonLevel);

    void PrepareMap() override;
    void DisplayWelcomeText() override;

  protected:
    void CreateLevel() override;

    void CreateCommonObjects(int x, int y, char image) override;

  private:
    void CreateStuff();
};

#endif // MAPLEVELTEST_H
