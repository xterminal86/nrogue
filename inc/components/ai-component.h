#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include <map>
#include <memory>

#include "component.h"
#include "ai-model-base.h"

class AIComponent : public Component
{
  public:
    AIComponent();

    template <typename T>
    AIModelBase* AddModel()
    {
      auto up = std::make_unique<T>();

      size_t hash = typeid(T).hash_code();

      up->AIComponentRef = this;

      _aiModels[hash] = std::move(up);

      return _aiModels[hash].get();
    }

    template <typename T>
    void ChangeModel()
    {
      size_t hash = typeid(T).hash_code();

      if (_aiModels.count(hash) == 1)
      {
        CurrentModel = _aiModels[hash].get();
      }
    }

    void Update() override;

    AIModelBase* CurrentModel;

  private:
    std::map<size_t, std::unique_ptr<AIModelBase>> _aiModels;
};

#endif // AICOMPONENT_H
