#ifndef TASKDRINKPOTION_H
#define TASKDRINKPOTION_H

#include "ai-model-base.h"

class ContainerComponent;

class TaskDrinkPotion : public Node
{
  public:
    TaskDrinkPotion(GameObject* objectToControl,
                    ScriptParamNames ref);

    BTResult Run() override;

  private:
    ContainerComponent* _inventoryRef = nullptr;

    int FindPotionHP();
    int FindPotionMP();
    int FindPotionAny();

    void UsePotion(int inventoryIndex);

    void PrintLogIfNeeded(ItemComponent* ic);

    ScriptParamNames _potionPref = ScriptParamNames::ANY;
};

#endif // TASKDRINKPOTION_H
