#ifndef AICOMPONENT_H
#define AICOMPONENT_H

#include <map>
#include <memory>

#include "component.h"
#include "ai-model-base.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

class AIComponent : public Component
{
  public:
    AIComponent();

    template <typename T>
    T* AddModel()
    {
      auto up = std::make_unique<T>();

      size_t hash = typeid(T).hash_code();

      up->AIComponentRef = this;

      _aiModels[hash] = std::move(up);

      auto pointer = _aiModels[hash].get();
      return static_cast<T*>(pointer);
    }

    template <typename T>
    T* GetModel()
    {
      size_t hash = typeid(T).hash_code();

      if (_aiModels.count(hash) == 1)
      {
        auto pointer = _aiModels[hash].get();
        return static_cast<T*>(pointer);
      }

      return nullptr;
    }

    template <typename T>
    void ChangeModel()
    {
      size_t hash = typeid(T).hash_code();

      if (_aiModels.count(hash) == 1)
      {
        CurrentModel = _aiModels[hash].get();
      }
      else
      {
        auto str = Util::StringFormat("%s no such AI model - %s!", __PRETTY_FUNCTION__, typeid(T).name());
        LogPrint(str, true);

        DebugLog("%s\n", str.data());
      }
    }

    void Update() override;

    AIModelBase* CurrentModel = nullptr;

  private:
    std::unordered_map<size_t, std::unique_ptr<AIModelBase>> _aiModels;
};

#endif // AICOMPONENT_H
