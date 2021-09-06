#ifndef ITEMUSECALLBACKS_H
#define ITEMUSECALLBACKS_H

class ItemComponent;

namespace ItemUseCallbacks
{
  bool HealingPotionUseHandler(ItemComponent* item);
  bool NeutralizePoisonPotionUseHandler(ItemComponent* item);
  bool ManaPotionUseHandler(ItemComponent* item);
  bool HungerPotionUseHandler(ItemComponent* item);
  bool ExpPotionUseHandler(ItemComponent* item);
  bool StatPotionUseHandler(ItemComponent* item);
  bool ReturnerUseHandler(ItemComponent* item);
  bool RepairKitUseHandler(ItemComponent* item);
  bool ScrollUseHandler(ItemComponent* item);
  bool FoodUseHandler(ItemComponent* item);
}

#endif // ITEMUSECALLBACKS_H
