#ifndef MONSTERSINC_H
#define MONSTERSINC_H

#include "singleton.h"

class GameObject;

class MonstersInc : public Singleton<MonstersInc>
{
  public:
    GameObject* CreateRat(int x, int y, bool randomize = true);
    GameObject* CreateBat(int x, int y, bool randomize = true);
    GameObject* CreateVampireBat(int x, int y, bool randomize = true);
    GameObject* CreateSpider(int x, int y, bool randomize = true);
    GameObject* CreateTroll(int x, int y, bool randomize = true);
    GameObject* CreateHerobrine(int x, int y);
    GameObject* CreateMadMiner(int x, int y);
    GameObject* CreateShelob(int x, int y);

  protected:
    void InitSpecific() override;

  private:
    int GetDifficulty();
};

#endif // MONSTERSINC_H
