#ifndef TRIGGERCOMPONENT_H
#define TRIGGERCOMPONENT_H

#include "component.h"

#include <functional>

enum class TriggerType
{
  ONE_SHOT = 0,
  CONSTANT
};

class TriggerComponent : public Component
{
  public:
    TriggerComponent();

    void Setup(TriggerType type,
               const std::function<bool()>& condition,
               const std::function<void()>& handler);

    void Update() override;

  private:
    TriggerType _type = TriggerType::ONE_SHOT;
    bool _once = false;

    std::function<bool()> _condition;
    std::function<void()> _handler;
};

#endif // TRIGGERCOMPONENT_H
