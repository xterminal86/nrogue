#ifndef AI_MODEL_BASE_H
#define AI_MODEL_BASE_H

#include <cstddef>

class AIComponent;
class Player;

class AIModelBase
{
  public:
    AIModelBase();
    virtual ~AIModelBase() = default;

    virtual void Update() {}

    AIComponent* AIComponentRef;

    bool IsAgressive = false;

    int AgroRadius = 0;

  protected:    
    size_t _hash;

    Player* _playerRef;

    bool IsPlayerVisible();
    bool IsPlayerInRange();
};

#endif
