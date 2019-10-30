#ifndef TASKCHASEPLAYER_H
#define TASKCHASEPLAYER_H

#include "behaviour-tree.h"

#include "pathfinder.h"

class Player;

class TaskChasePlayer : public Node
{
  public:    
    TaskChasePlayer(GameObject* objectToControl);

    BTResult Run() override;

  private:
    Player* _playerRef;
};

#endif // TASKCHASEPLAYER_H
