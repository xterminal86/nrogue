#include "map-level-test.h"

#include "player.h"
#include "game-object-info.h"
#include "game-objects-factory.h"

MapLevelTest::MapLevelTest(int sizeX, int sizeY, MapType type, int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
}

// =============================================================================

void MapLevelTest::PrepareMap()
{
  MapLevelBase::PrepareMap();
  CreateLevel();
}

// =============================================================================

void MapLevelTest::DisplayWelcomeText()
{
}

// =============================================================================

void MapLevelTest::CreateStuff()
{
  GameObject* go = GameObjectsFactory::Instance().CreateChest(5, 5, false);
  PlaceGameObject(go);

  PlaceDoor(10, 10, false);
  PlaceDoor(10, 11, true);

  PlaceShrine({ 13, 13 }, ShrineType::HEALING);

  // ========================= CREATE 20 RANDOM MELEE WEAPONS ==================

  /*
  for (int i = 0; i < 20; i++)
  {
    GameObject* go = ItemsFactory::Instance().CreateRandomMeleeWeapon();
    go->PosX = 1 + i;
    go->PosY = 1;
    PlaceGameObject(go);
  }
  */

  // ============================ CREATE ITEM WITH BONUS =======================

  /*
  ItemBonusStruct bs;
  bs.BonusValue = 2;
  bs.Type = ItemBonusType::KNOCKBACK;
  bs.FromItem = true;

  GameObject* go = ItemsFactory::Instance().CreateRangedWeapon(1, 1, RangedWeaponType::XBOW, ItemPrefix::UNCURSED, ItemQuality::NORMAL, { bs });
  ItemComponent* ic = go->GetComponent<ItemComponent>();
  ic->Data.Damage.SetMin(1);
  ic->Data.Damage.SetMax(1);
  PlaceGameObject(go);

  GameObject* bolts = ItemsFactory::Instance().CreateArrows(1, 1, ArrowType::BOLTS, ItemPrefix::UNCURSED, 10);
  PlaceGameObject(bolts);

  GameObject* dag = ItemsFactory::Instance().CreateWeapon(1, 1, WeaponType::DAGGER, ItemPrefix::UNCURSED, ItemQuality::NORMAL, { bs });
  PlaceGameObject(dag);
  */

  /*
  ItemBonusStruct b;
  b.BonusValue = 1;
  b.Period = -1;
  b.Type = ItemBonusType::LEVITATION;
  b.FromItem = true;

  GameObject* go = ItemsFactory::Instance().CreateAccessory(1, 1, EquipmentCategory::RING, { b }, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
  PlaceGameObject(go);
  */

  // =================================== RANDOM WANDS ==========================

  /*
  for (size_t x = 1; x < 16; x++)
  {
    auto wand = ItemsFactory::Instance().CreateRandomWand();
    ItemComponent* ic = wand->GetComponent<ItemComponent>();
    ic->Data.IsIdentified = true;
    wand->PosX = x;
    wand->PosY = 1;
    PlaceGameObject(wand);
  }
  */

  // =================================== RINGS =================================

  /*
  ItemQuality q = ItemQuality::EXCEPTIONAL;

  std::unordered_map<ItemQuality, int> multByQ =
  {
    { ItemQuality::DAMAGED,      1 },
    { ItemQuality::FLAWED,       2 },
    { ItemQuality::NORMAL,       3 },
    { ItemQuality::FINE,         4 },
    { ItemQuality::EXCEPTIONAL,  5 },
  };

  for (int i = 0; i < 10; i++)
  {
    int min = 20 + 5 * (multByQ[q] - 1);
    int max = 21 + 20 * multByQ[q];
    int percentage = RNG::Instance().RandomRange(min, max);

    ItemBonusStruct bs;
    bs.Type = ItemBonusType::THORNS;
    bs.Id = _playerRef->ObjectId();
    bs.Duration = -1;
    bs.BonusValue = percentage;

    auto ring = GameObjectsFactory::Instance().CreateAccessory(1 + i, 1, EquipmentCategory::RING, { bs }, ItemPrefix::UNCURSED, q);
    PlaceGameObject(ring);
  }
  */

  /*
  ItemBonusStruct bs;
  bs.Type = ItemBonusType::REGEN;
  bs.BonusValue = 1;
  bs.Period = 5;

  auto ring1 = GameObjectsFactory::Instance().CreateAccessory(1, 1, EquipmentCategory::RING, { bs }, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
  auto ring2 = GameObjectsFactory::Instance().CreateAccessory(1, 1, EquipmentCategory::RING, { bs }, ItemPrefix::UNCURSED, ItemQuality::NORMAL);
  PlaceGameObject(ring1);
  PlaceGameObject(ring2);
  */
}

// =============================================================================

void MapLevelTest::CreateLevel()
{
  _playerRef->IgnoreMe = true;

  VisibilityRadius = 16;
  MonstersRespawnTurns = GlobalConstants::MonstersRespawnTimeout;

  CreateGround('.',
               Colors::ShadesOfGrey::Four,
               Colors::BlackColor,
               Strings::TileNames::GroundText);

  CreateBorders('#',
                Colors::WhiteColor,
                Colors::ShadesOfGrey::Six,
                Strings::TileNames::RocksText);

  for (int i = 0; i < 3; i++)
  {
    PlaceWall(3,
              3 + i,
              '#',
              Colors::WhiteColor,
              Colors::ShadesOfGrey::Six,
              Strings::TileNames::RocksText);
  }

  CreateStuff();
}

// =============================================================================

void MapLevelTest::CreateCommonObjects(int x, int y, char image)
{
}
