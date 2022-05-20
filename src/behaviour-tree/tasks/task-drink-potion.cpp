#include "task-drink-potion.h"

#include "container-component.h"
#include "item-component.h"
#include "map.h"
#include "printer.h"
#include "player.h"

TaskDrinkPotion::TaskDrinkPotion(GameObject* objectToControl,
                                 PotionPreference ref)
  : Node(objectToControl)
{
  _potionPref = ref;
  _inventoryRef = _objectToControl->GetComponent<ContainerComponent>();
}

BTResult TaskDrinkPotion::Run()
{
  if (_inventoryRef == nullptr)
  {
    return BTResult::Failure;
  }

  int itemIndex = -1;

  switch (_potionPref)
  {
    case PotionPreference::HP:
      itemIndex = FindPotionHP();
      break;

    case PotionPreference::MP:
      itemIndex = FindPotionMP();
      break;

    case PotionPreference::ANY:
      itemIndex = FindPotionAny();
      break;
  }

  if (itemIndex == -1)
  {
    return BTResult::Failure;
  }

  UsePotion(itemIndex);

  _objectToControl->FinishTurn();

  return BTResult::Success;
}

void TaskDrinkPotion::UsePotion(int inventoryIndex)
{
  auto DestroyItem = [this, inventoryIndex]()
  {
    auto it = _inventoryRef->Contents.begin();
    _inventoryRef->Contents.erase(it + inventoryIndex);
  };

  ItemComponent* ic = _inventoryRef->Contents[inventoryIndex]->GetComponent<ItemComponent>();
  ic->Use(_objectToControl);

  //
  // Object is no longer valid after it is erased from collection
  // so we have to assemble all object-related strings beforehand.
  //
  PrintLogIfNeeded(ic);

  if (ic->Data.IsStackable)
  {
    ic->Data.Amount--;

    if (ic->Data.Amount == 0)
    {
      DestroyItem();
    }
  }
  else
  {
    DestroyItem();
  }
}

void TaskDrinkPotion::PrintLogIfNeeded(ItemComponent* ic)
{
  auto curLvl = Map::Instance().CurrentLevel;
  auto curTile = curLvl->MapArray[_objectToControl->PosX][_objectToControl->PosY].get();

  if (curTile->Visible)
  {
    std::string objName = ic->Data.UnidentifiedName;

    if (_playerRef->RecallItem(ic) != Strings::UnidentifiedEffectText)
    {
      objName = ic->OwnerGameObject->ObjectName;
    }

    auto msg = Util::StringFormat("%s drinks %s",
                                  _objectToControl->ObjectName.data(),
                                  objName.data());

    Printer::Instance().AddMessage(msg);
  }
}

int TaskDrinkPotion::FindPotionHP()
{
  int itemIndex = -1;

  for (size_t i = 0; i < _inventoryRef->Contents.size(); i++)
  {
    ItemComponent* ic = _inventoryRef->Contents[i]->GetComponent<ItemComponent>();
    if (ic->Data.ItemType_ == ItemType::HEALING_POTION
     && ic->Data.IsIdentified)
    {
      itemIndex = i;
      break;
    }
  }

  return itemIndex;
}

int TaskDrinkPotion::FindPotionMP()
{
  int itemIndex = -1;

  for (size_t i = 0; i < _inventoryRef->Contents.size(); i++)
  {
    ItemComponent* ic = _inventoryRef->Contents[i]->GetComponent<ItemComponent>();
    if (ic->Data.ItemType_ == ItemType::MANA_POTION
     && ic->Data.IsIdentified)
    {
      itemIndex = i;
      break;
    }
  }

  return itemIndex;
}

int TaskDrinkPotion::FindPotionAny()
{
  int itemIndex = -1;

  std::vector<int> itemIndices;

  for (size_t i = 0; i < _inventoryRef->Contents.size(); i++)
  {
    ItemComponent* ic = _inventoryRef->Contents[i]->GetComponent<ItemComponent>();
    if (ic->Data.ItemType_ == ItemType::POTION)
    {
      itemIndices.push_back(i);
    }
  }

  if (!itemIndices.empty())
  {
    itemIndex = RNG::Instance().RandomRange(0, itemIndices.size());
  }

  return itemIndex;
}

