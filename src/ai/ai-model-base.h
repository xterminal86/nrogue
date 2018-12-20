#ifndef AI_MODEL_BASE_H
#define AI_MODEL_BASE_H

#include <cstddef>

class AIComponent;

class AIModelBase
{
  public:
    virtual ~AIModelBase() {}

    virtual void Update() {}

    AIComponent* AIComponentRef;

    bool IsAgressive = false;

  protected:
    size_t _hash;
};

#endif
