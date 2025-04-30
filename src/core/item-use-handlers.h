#ifndef ITEMUSEHANDLERS_H
#define ITEMUSEHANDLERS_H

#include "enumerations.h"

class ItemComponent;
class GameObject;

namespace ItemUseHandlers
{
  UseResult HealingPotionUseHandler(ItemComponent* item, GameObject* user);
  UseResult NeutralizePoisonPotionUseHandler(ItemComponent* item,
                                             GameObject* user);
  UseResult ManaPotionUseHandler(ItemComponent* item, GameObject* user);
  UseResult JuicePotionUseHandler(ItemComponent* item, GameObject* user);
  UseResult CWPotionUseHandler(ItemComponent* item, GameObject* user);
  UseResult RAPotionUseHandler(ItemComponent* item, GameObject* user);
  UseResult ExpPotionUseHandler(ItemComponent* item, GameObject* user);
  UseResult StatPotionUseHandler(ItemComponent* item, GameObject* user);
  UseResult ReturnerUseHandler(ItemComponent* item, GameObject* user);
  UseResult RepairKitUseHandler(ItemComponent* item, GameObject* user);
  UseResult ScrollUseHandler(ItemComponent* item, GameObject* user);
  UseResult FoodUseHandler(ItemComponent* item, GameObject* user);
}

#endif // ITEMUSEHANDLERS_H
