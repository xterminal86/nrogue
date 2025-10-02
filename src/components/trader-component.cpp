#include "trader-component.h"

#include "items-factory.h"
#include "rng.h"

#include "util.h"
#include "ai-npc.h"

TraderComponent::TraderComponent()
{
}

// =============================================================================

void TraderComponent::Init(TraderRole traderType,
                           int stockRefreshTurns,
                           int maxItems)
{
  _traderType = traderType;
  _stockRefreshTurns = stockRefreshTurns;
  _maxItems = maxItems;

  RefreshStock();
}

// =============================================================================

void TraderComponent::RefreshStock()
{
  int min = _maxItems / 2;
  if (min == 0)
  {
    min = 1;
  }

  _itemsToCreate = Game::gRng.RandomRange(min, _maxItems + 1);

  Items.clear();

  CreateItems();
}

// =============================================================================

void TraderComponent::Update()
{
  _stockResetCounter++;

  if (_stockResetCounter > _stockRefreshTurns)
  {
    RefreshStock();
    _stockResetCounter = 0;
  }
}

// =============================================================================

void TraderComponent::CreateItems()
{
  switch (_traderType)
  {
    case TraderRole::CLERIC:
      CreateClericItems();
      break;

    case TraderRole::COOK:
      CreateCookItems();
      break;

    case TraderRole::JUNKER:
      CreateJunkerItems();
      break;

    case TraderRole::BLACKSMITH:
      CreateBlacksmithItems();
      break;
  }
}

// =============================================================================

void TraderComponent::CreateClericItems()
{
  std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
  std::string npcName = NpcRef->Data.Name;
  ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

  static ItemsFactory& factory = Game::gIF;

  for (int i = 0; i < _itemsToCreate; i++)
  {
    auto itemPair   = Util::WeightedRandom(_clericItemsWeights);
    auto prefixPair = Util::WeightedRandom(_clericPrefixWeights);

    GameObject* go = nullptr;

    switch (itemPair.first)
    {
      case ItemType::POTION:
      {
        auto potionPair = Util::WeightedRandom(_clericPotionWeights);
        switch (potionPair.first)
        {
          case PotionType::HEALING_POTION:
            go = factory.CreateHealingPotion(prefixPair.first);
            break;

          case PotionType::MANA_POTION:
            go = factory.CreateManaPotion(prefixPair.first);
            break;

          case PotionType::NP_POTION:
            go = factory.CreateNeutralizePoisonPotion(prefixPair.first);
            break;

          case PotionType::CW_POTION:
            go = factory.CreateCWPotion(prefixPair.first);
            break;

          case PotionType::RA_POTION:
            go = factory.CreateRAPotion(prefixPair.first);
            break;
        }
      }
      break;


      case ItemType::RETURNER:
        go = factory.CreateReturner(0, 0, -1, prefixPair.first);
        break;

      case ItemType::WAND:
        go = factory.CreateRandomWand(prefixPair.first);
        break;

      case ItemType::SCROLL:
        go = factory.CreateRandomScroll(prefixPair.first);
        break;

      case ItemType::ACCESSORY:
        go = factory.CreateRandomAccessory(0, 0, prefixPair.first, true);
        break;
    }

    Items.push_back(std::unique_ptr<GameObject>(go));
  }
}

// =============================================================================

void TraderComponent::CreateCookItems()
{
  std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
  std::string npcName = NpcRef->Data.Name;
  ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

  for (int i = 0; i < _itemsToCreate; i++)
  {
    auto itemPair   = Util::WeightedRandom(_cookItemsWeights);
    auto prefixPair = Util::WeightedRandom(_cookPrefixWeights);

    GameObject* go = Game::gIF.CreateFood(0,
                                                         0,
                                                         itemPair.first,
                                                         prefixPair.first,
                                                         true);

    Items.push_back(std::unique_ptr<GameObject>(go));
  }
}

// =============================================================================

void TraderComponent::CreateJunkerItems()
{
  std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
  std::string npcName = NpcRef->Data.Name;
  ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

  for (int i = 0; i < _itemsToCreate; i++)
  {
    GameObject* go = Game::gIF.CreateRandomItem(0, 0,
                                                 {
                                                   ItemType::COINS,
                                                   ItemType::FOOD
                                                 });
    if (go != nullptr)
    {
      Items.push_back(std::unique_ptr<GameObject>(go));
    }
  }
}

// =============================================================================

void TraderComponent::CreateBlacksmithItems()
{
  std::string shopName = GlobalConstants::ShopNameByType.at(_traderType);
  std::string npcName = NpcRef->Data.Name;
  ShopTitle = Util::StringFormat(" %s's %s ", npcName.data(), shopName.data());

  for (int i = 0; i < _itemsToCreate; i++)
  {
    auto itemPair   = Util::WeightedRandom(_blacksmithItemsWeights);
    auto prefixPair = Util::WeightedRandom(_blacksmithPrefixWeights);

    GameObject* go = nullptr;

    switch (itemPair.first)
    {
      case ItemType::WEAPON:
        go = Game::gIF.CreateRandomWeapon(prefixPair.first);
        break;

      case ItemType::REPAIR_KIT:
        go = Game::gIF.CreateRepairKit(0, 0, 30, prefixPair.first);
        break;

      case ItemType::ARMOR:
      {
        auto armorPair = Util::WeightedRandom(_blacksmithArmorWeights);
        go = Game::gIF.CreateArmor(0, 0, armorPair.first, prefixPair.first);
      }
      break;

      case ItemType::ARROWS:
      {
        int flag = Game::gRng.RandomRange(0, 2);
        ArrowType arrowsType = (flag == 0)
                              ? ArrowType::ARROWS
                              : ArrowType::BOLTS;
        int amount = Game::gRng.RandomRange(10, 21);
        go = Game::gIF.CreateArrows(0, 0, arrowsType, prefixPair.first, amount);
      }
      break;

      case ItemType::ACCESSORY:
        go = Game::gIF.CreateRandomAccessory(0, 0, prefixPair.first, true);
        break;
    }

    if (go != nullptr)
    {
      Items.push_back(std::unique_ptr<GameObject>(go));
    }
  }
}

// =============================================================================

TraderRole TraderComponent::Type()
{
  return _traderType;
}
