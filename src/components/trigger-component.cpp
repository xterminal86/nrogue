#include "trigger-component.h"

#include "util.h"

TriggerComponent::TriggerComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void TriggerComponent::Setup(TriggerType type,
                             const std::function<bool()>& condition,
                             const std::function<void()>& handler)
{
  _type = type;
  _condition = condition;
  _handler = handler;
}

void TriggerComponent::Update()
{
  if (Util::IsFunctionValid(_condition))
  {
    if (_condition())
    {
      if (Util::IsFunctionValid(_handler))
      {
        if (_type == TriggerType::ONE_SHOT)
        {
          if (!_once)
          {
            _handler();
            _once = true;
          }
        }
        else
        {
          _handler();
        }
      }
    }
  }
}
