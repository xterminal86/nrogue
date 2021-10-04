#include "trigger-component.h"

#include "util.h"

TriggerComponent::TriggerComponent()
{
  _componentHash = typeid(*this).hash_code();
}

void TriggerComponent::Setup(const TriggerData& data)
{
  _data = data;
}

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
