#ifndef AI_MODEL_BASE_H
#define AI_MODEL_BASE_H

#include <stack>
#include <memory>
#include <cstddef>

#include "ai-state-base.h"

class AIComponent;
class Player;

class AIModelBase
{
  public:
    AIModelBase();
    virtual ~AIModelBase() = default;

    virtual void Update();

    AIComponent* AIComponentRef = nullptr;

    bool IsAgressive = false;

    int AgroRadius = 0;

    template <typename T>
    inline void ChangeAIState()
    {
      PopAIState();
      PushAIState<T>();
    }

    template <typename T>
    inline void PushAIState()
    {
      _aiStates.push(std::unique_ptr<T>(new T(this)));
      _currentState = _aiStates.top().get();
      _currentState->Enter();
    }

    inline void PopAIState()
    {
      if (_currentState != nullptr)
      {
        _currentState->Exit();
      }

      if (!_aiStates.empty())
      {
        _aiStates.pop();
      }
    }

  protected:    
    size_t _hash;
    std::stack<std::unique_ptr<AIStateBase>> _aiStates;

    AIStateBase* _currentState = nullptr;
    Player* _playerRef = nullptr;

    bool IsPlayerVisible();
    bool IsPlayerInRange();    
};

#endif
