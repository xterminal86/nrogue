#ifndef TRIGGERCOMPONENT_H
#define TRIGGERCOMPONENT_H

#include "component.h"

#include <functional>

enum class TriggerType
{
  ONE_SHOT = 0,
  CONSTANT
};

struct TriggerData
{
  TriggerType Type = TriggerType::ONE_SHOT;
  std::function<bool()> Condition;
  std::function<void()> Handler;
};

class TriggerComponent : public Component
{
  public:
    TriggerComponent();

    void Setup(const TriggerData& data);

    void Update() override;

  private:
    bool _once = false;

    TriggerData _data;
};

#endif // TRIGGERCOMPONENT_H