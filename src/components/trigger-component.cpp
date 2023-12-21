#include "trigger-component.h"

#include "game-object.h"
#include "util.h"

TriggerComponent::TriggerComponent(TriggerType type,
                                   const std::function<bool()>& condition,
                                   const std::function<void()>& handler)
{
  _data.Type      = type;
  _data.Condition = condition;
  _data.Handler   = handler;
}

// =============================================================================

void TriggerComponent::Update()
{
  if (Util::IsFunctionValid(_data.Condition))
  {
    if (_data.Condition())
    {
      if (Util::IsFunctionValid(_data.Handler))
      {
        if (_data.Type == TriggerType::ONE_SHOT)
        {
          if (!_once)
          {
            _data.Handler();
            _once = true;
            OwnerGameObject->IsDestroyed = true;
          }
        }
        else
        {
          _data.Handler();
        }
      }
    }
  }
}
