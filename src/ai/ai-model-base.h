#ifndef AI_MODEL_BASE_H
#define AI_MODEL_BASE_H

#include <stack>
#include <memory>
#include <cstddef>
#include <ctime>

//#include "ai-state-base.h"
#include "behaviour-tree.h"

class AIComponent;
class Player;

class AIModelBase
{
  public:
    AIModelBase();
    virtual ~AIModelBase() = default;

    virtual void ConstructAI();
    virtual void Update();

    AIComponent* AIComponentRef = nullptr;

    bool IsAgressive = false;

    int AgroRadius = 0;

  protected:
    size_t _hash;
    Player* _playerRef = nullptr;    

    std::unique_ptr<Root> _root;

    Selector* GetIdleSelector();
};

#endif
