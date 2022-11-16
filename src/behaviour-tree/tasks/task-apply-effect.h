#ifndef TASKAPPLYEFFECT_H
#define TASKAPPLYEFFECT_H

#include "behaviour-tree.h"
#include "item-data.h"

class TaskApplyEffect : public Node
{
  public:
    TaskApplyEffect(GameObject* objectToControl,
                    const ItemBonusStruct& effectToApply);

    BTResult Run() override;

  private:
    ItemBonusStruct _effectToApply;
};

#endif // TASKAPPLYEFFECT_H
