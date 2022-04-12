#ifndef TASKDRINKPOTION_H
#define TASKDRINKPOTION_H

#include "behaviour-tree.h"

class ContainerComponent;

class TaskDrinkPotion : public Node
{
  public:
    TaskDrinkPotion(GameObject* objectToControl,
                  const std::string& potionPreference);

    BTResult Run() override;

  private:
    enum class PotionPreference
    {
      ANY = 0,
      HP,
      MP
    };

    PotionPreference _potionPref;

    ContainerComponent* _inventoryRef = nullptr;

    const std::map<std::string, PotionPreference> _potionPrefByName =
    {
      { "-",  PotionPreference::ANY },
      { "HP", PotionPreference::HP  },
      { "MP", PotionPreference::MP  }
    };

    int FindPotionHP();
    int FindPotionMP();
    int FindPotionAny();

    void UsePotion(int inventoryIndex);

    void PrintLogIfNeeded(ItemComponent* ic);
};

#endif // TASKDRINKPOTION_H
