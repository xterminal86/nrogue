#ifndef ITEMUSEHANDLERS_H
#define ITEMUSEHANDLERS_H

class ItemComponent;

namespace ItemUseHandlers
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

#endif // ITEMUSEHANDLERS_H
