#ifndef TRIGGERCOMPONENT_H
#define TRIGGERCOMPONENT_H

#include <functional>

#include "component.h"
#include "enumerations.h"

struct TriggerData
{
  TriggerType Type = TriggerType::ONE_SHOT;
  std::function<bool()> Condition;
  std::function<void()> Handler;
};

class TriggerComponent : public Component
{
  public:
    TriggerComponent(TriggerType type,
                     const std::function<bool()>& condition,
                     const std::function<void()>& handler);

    void Update() override;

  private:
    bool _once = false;

    TriggerData _data;
};

#endif // TRIGGERCOMPONENT_H
