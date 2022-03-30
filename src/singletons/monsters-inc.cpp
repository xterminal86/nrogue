#include "monsters-inc.h"

#include "map.h"
#include "items-factory.h"

#include "ai-component.h"
#include "equipment-component.h"
#include "container-component.h"
#include "item-component.h"
#include "loot-generators.h"

#include "ai-idle.h"
#include "ai-monster-basic.h"
#include "ai-monster-bat.h"
#include "ai-monster-vampire-bat.h"
#include "ai-monster-spider.h"
#include "ai-monster-shelob.h"
#include "ai-monster-troll.h"
#include "ai-monster-herobrine.h"
#include "ai-monster-mad-miner.h"

void MonstersInc::InitSpecific()
{
}

GameObject* MonstersInc::CreateRat(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  'r',
                                  Colors::MonsterColor);

  go->ObjectName = "Feral Rat";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 30;

  // Sets Occupied flag for _currentCell
  go->Move(0, 0);

  go->IsLiving = true;

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBasic* aimb = ai->AddModel<AIMonsterBasic>();
  aimb->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(8);
  aimb->ConstructAI();

  ai->ChangeModel<AIMonsterBasic>();

  // Set attributes
  if (randomize)
  {
    // TODO: determine proper difficulty scaling for monsters

    //int pl = _playerRef->Attrs.Lvl.Get();
    int difficulty = GetDifficulty();

    go->Attrs.Str.Talents = 1;
    go->Attrs.Spd.Talents = 1;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp(2);
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();

    /*
    int randomStr = RNG::Instance().RandomRange(0 * difficulty, 1 * difficulty);
    int randomDef = RNG::Instance().RandomRange(0, 1 * difficulty);
    int randomSkl = RNG::Instance().RandomRange(0, 1 * difficulty);
    int randomHp = RNG::Instance().RandomRange(1 * difficulty, 5 * difficulty);
    int randomSpd = RNG::Instance().RandomRange(0, 2 * difficulty);

    go->Attrs.Lvl.Set(difficulty);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
    */
  }

  go->GenerateLootFunction = std::bind(&LootGenerators::Rat, go);

  return go;
}

GameObject* MonstersInc::CreateBat(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  'b',
                                  Colors::MonsterColor);
  go->ObjectName = "Flying Bat";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 20;

  go->IsLiving = true;

  // Sets Occupied flag for _currentCell
  go->Move(0, 0);

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBat* aimb = ai->AddModel<AIMonsterBat>();
  aimb->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(16);
  aimb->ConstructAI();

  ai->ChangeModel<AIMonsterBat>();

  // Set attributes
  if (randomize)
  {
    //int pl = _playerRef->Attrs.Lvl.Get();
    int difficulty = GetDifficulty();

    go->Attrs.Def.Talents = 3;
    go->Attrs.Spd.Talents = 1;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp();
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();

    /*
    int randomStr = RNG::Instance().RandomRange(0 * difficulty, 1 * difficulty);
    int randomDef = RNG::Instance().RandomRange(0, 1 * difficulty);
    int randomSkl = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomHp = RNG::Instance().RandomRange(1 * difficulty, 3 * difficulty);
    int randomSpd = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);

    go->Attrs.Lvl.Set(difficulty);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
    */
  }

  ItemBonusStruct ibs;

  ibs.Type = ItemBonusType::LEVITATION;
  ibs.Id = go->ObjectId();

  go->AddEffect(ibs);

  return go;
}

GameObject* MonstersInc::CreateVampireBat(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  'b',
                                  Colors::MonsterColor);
  go->ObjectName = "Red Bat";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 20;

  go->IsLiving = false;

  go->Move(0, 0);

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterVampireBat* aimb = ai->AddModel<AIMonsterVampireBat>();
  aimb->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(16);
  aimb->ConstructAI();

  ai->ChangeModel<AIMonsterVampireBat>();

  // Set attributes
  if (randomize)
  {
    //int pl = _playerRef->Attrs.Lvl.Get();
    int difficulty = GetDifficulty();

    go->Attrs.Def.Talents = 3;
    go->Attrs.Spd.Talents = 1;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp();
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();
  }

  return go;
}

GameObject* MonstersInc::CreateSpider(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  's',
                                  Colors::MonsterColor);
  go->ObjectName = "Cave Spider";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 30;

  go->IsLiving = true;

  go->Move(0, 0);

  AIComponent* ai = go->AddComponent<AIComponent>();

  // ===========================================================================
  AIMonsterSpider* aims = ai->AddModel<AIMonsterSpider>();
  aims->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(12);
  aims->ConstructAI();
  // ===========================================================================

  ai->ChangeModel<AIMonsterSpider>();

  // Set attributes
  if (randomize)
  {
    //int pl = _playerRef->Attrs.Lvl.Get();
    int difficulty = GetDifficulty();

    go->Attrs.Str.Talents = 2;
    go->Attrs.Def.Talents = 2;
    go->Attrs.Spd.Talents = 1;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp(4);
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();

    /*
    int randomStr = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomDef = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomSkl = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);
    int randomHp = RNG::Instance().RandomRange(2 * difficulty, 6 * difficulty);
    int randomSpd = RNG::Instance().RandomRange(1 * difficulty, 2 * difficulty);

    go->Attrs.Lvl.Set(difficulty);

    go->Attrs.Str.Set(randomStr);
    go->Attrs.Def.Set(randomDef);
    go->Attrs.HP.Set(randomHp);
    go->Attrs.Spd.Set(randomSpd);
    go->Attrs.Skl.Set(randomSkl);
    */
  }

  return go;
}

GameObject* MonstersInc::CreateTroll(int x, int y, bool randomize)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  'T',
                                  Colors::MonsterColor);
  go->ObjectName = "Troll";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 3;

  go->IsLiving = true;

  go->Move(0, 0);

  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterTroll* aims = ai->AddModel<AIMonsterTroll>();
  aims->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(1);
  aims->ConstructAI();

  ai->ChangeModel<AIMonsterTroll>();

  // Set attributes
  if (randomize)
  {
    //int pl = _playerRef->Attrs.Lvl.Get();
    int difficulty = GetDifficulty();

    go->Attrs.Str.Set(4);

    go->Attrs.Str.Talents = 3;
    go->Attrs.HP.Talents  = 3;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp();
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();

    int str = go->Attrs.Str.Get() * 2;
    go->Attrs.Str.Set(str);

    go->Attrs.Skl.Set(0);
    go->Attrs.Spd.Set(0);
  }

  return go;
}

GameObject* MonstersInc::CreateHerobrine(int x, int y)
{
  char img = '@';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::FACE_2];
  #endif

  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  img,
                                  Colors::MonsterColor);
  go->ObjectName = "Herobrine";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 30;

  go->MoveTo(x, y);

  AIComponent* ai = go->AddComponent<AIComponent>();

  // ===========================================================================
  AIMonsterHerobrine* aims = ai->AddModel<AIMonsterHerobrine>();
  aims->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(12);
  aims->ConstructAI();
  // ===========================================================================
  AIIdle* aii = ai->AddModel<AIIdle>();
  aii->ConstructAI();
  // ===========================================================================

  ai->ChangeModel<AIIdle>();

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();

  GameObject* pickaxe = ItemsFactory::Instance().CreateUniquePickaxe();
  cc->Add(pickaxe);

  GameObject* gem = ItemsFactory::Instance().CreateGem(0, 0, GemType::ORANGE_AMBER);
  cc->Add(gem);

  go->Attrs.Str.Talents = 3;
  go->Attrs.Skl.Talents = 3;
  go->Attrs.Def.Talents = 1;
  go->Attrs.Spd.Talents = 2;

  for (int i = 0; i < 8; i++)
  {
    int hpToAdd = RNG::Instance().RandomRange(4, 6);
    go->LevelUp(hpToAdd);
  }

  go->Attrs.HP.Restore();
  go->Attrs.MP.Restore();

  return go;
}

GameObject* MonstersInc::CreateMadMiner(int x, int y)
{
  char img = '@';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::FACE_2];
  #endif

  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, img, Colors::MonsterColor);
  go->ObjectName = "Mad Miner";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 30;

  go->MoveTo(x, y);

  AIComponent* ai = go->AddComponent<AIComponent>();

  // ===========================================================================
  AIMonsterMadMiner* aim = ai->AddModel<AIMonsterMadMiner>();
  aim->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(6);
  aim->ConstructAI();
  // ===========================================================================

  ai->ChangeModel<AIMonsterMadMiner>();

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();
  GameObject* pick = ItemsFactory::Instance().CreateWeapon(1, 1, WeaponType::PICKAXE);
  ItemComponent* ic = pick->GetComponent<ItemComponent>();

  cc->Add(pick);

  EquipmentComponent* ec = go->AddComponent<EquipmentComponent>(cc);
  ec->Equip(ic);

  go->Attrs.Str.Talents = 1;
  go->Attrs.Skl.Talents = 1;
  go->Attrs.Spd.Talents = 1;

  int difficulty = GetDifficulty();

  for (int i = 0; i < difficulty; i++)
  {
    go->LevelUp();
  }

  go->Attrs.HP.Restore();
  go->Attrs.MP.Restore();

  go->GenerateLootFunction = std::bind(&LootGenerators::MadMiner, go);

  return go;
}

GameObject* MonstersInc::CreateShelob(int x, int y)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  'S',
                                  Colors::MonsterUniqueColor);
  go->ObjectName = "Shelob";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 15;

  go->MoveTo(x, y);

  AIComponent* ai = go->AddComponent<AIComponent>();

  // ===========================================================================
  AIMonsterShelob* aim = ai->AddModel<AIMonsterShelob>();
  aim->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(6);
  aim->ConstructAI();
  // ===========================================================================

  ai->ChangeModel<AIMonsterShelob>();

  go->Attrs.Str.Talents = 2;
  go->Attrs.Skl.Talents = 2;
  go->Attrs.Spd.Talents = 3;

  int difficulty = GetDifficulty();

  for (int i = 0; i < difficulty; i++)
  {
    go->LevelUp(4);
  }

  go->Attrs.HP.Restore();
  go->Attrs.MP.Restore();

  go->GenerateLootFunction = std::bind(&LootGenerators::Shelob, go);

  return go;
}

// =============================================================================

int MonstersInc::GetDifficulty()
{
  int dl = Map::Instance().CurrentLevel->DungeonLevel;
  int difficulty = dl;
  int diffOffset = RNG::Instance().RandomRange(0, 4);

  difficulty += diffOffset;

  return difficulty;
}
