#include "monsters-inc.h"

#include "map.h"
#include "items-factory.h"

#include "ai-component.h"
#include "ai-npc.h"
#include "equipment-component.h"
#include "container-component.h"
#include "item-component.h"
#include "loot-generators.h"
#include "timed-destroyer-component.h"

#include "ai-idle.h"
#include "ai-monster-basic.h"
#include "ai-monster-bat.h"
#include "ai-monster-vampire-bat.h"
#include "ai-monster-spider.h"
#include "ai-monster-shelob.h"
#include "ai-monster-troll.h"
#include "ai-monster-herobrine.h"
#include "ai-monster-mad-miner.h"
#include "ai-monster-kobold.h"
#include "ai-monster-wraith.h"

void MonstersInc::InitSpecific()
{
}

//
// TODO: determine proper difficulty scaling for monsters
//
GameObject* MonstersInc::CreateMonster(int x, int y, GameObjectType monsterType)
{
  GameObject* go = nullptr;

  switch (monsterType)
  {
    case GameObjectType::RAT:
      go = CreateRat(x, y);
      break;

    case GameObjectType::BAT:
      go = CreateBat(x, y);
      break;

    case GameObjectType::VAMPIRE_BAT:
      go = CreateVampireBat(x, y);
      break;

    case GameObjectType::SPIDER:
      go = CreateSpider(x, y);
      break;

    case GameObjectType::HEROBRINE:
      go = CreateHerobrine(x, y);
      break;

    case GameObjectType::TROLL:
      go = CreateTroll(x, y);
      break;

    case GameObjectType::MAD_MINER:
      go = CreateMadMiner(x, y);
      break;

    case GameObjectType::SHELOB:
      go = CreateShelob(x, y);
      break;

    case GameObjectType::KOBOLD:
      go = CreateKobold(x, y);
      break;

    case GameObjectType::ZOMBIE:
      go = CreateZombie(x, y);
      break;

    case GameObjectType::SKELETON:
      go = CreateSkeleton(x, y);
      break;

    case GameObjectType::WRAITH:
      go = CreateWraith(x, y);
      break;

    case GameObjectType::STALKER:
      go = CreateStalker(x, y);
      break;

    default:
      DebugLog("CreateMonster(): monster type %i is not implemented!", monsterType);
      break;
  }

  if (go != nullptr)
  {
    go->Type = monsterType;
    go->PosX = x;
    go->PosY = y;
  }

  return go;
}

GameObject* MonstersInc::CreateNPC(int x,
                                   int y,
                                   NPCType npcType,
                                   bool standing,
                                   ServiceType serviceType)
{
  char img = '@';

  #ifdef USE_SDL
  img = GlobalConstants::CP437IndexByType[NameCP437::FACE_2];
  #endif

  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, img, Colors::WhiteColor);

  go->IsLiving = true;
  go->Type = GameObjectType::NPC;

  go->Move(0, 0);

  AIComponent* aic = go->AddComponent<AIComponent>();
  AINPC* ainpc = aic->AddModel<AINPC>();
  ainpc->Init(npcType, standing, serviceType);

  uint32_t goColor = (ainpc->Data.IsMale)
                    ? Colors::WhiteColor
                    : Colors::MagentaColor;

  go->FgColor = goColor;

  aic->ChangeModel<AINPC>();

  return go;
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

  // Set attributes
  if (randomize)
  {
    //int pl = _playerRef->Attrs.Lvl.Get();
    int difficulty = GetDifficulty();

    difficulty = Util::Instance().Clamp(difficulty, 1, 3);

    go->Attrs.Str.Talents = 1;
    go->Attrs.Spd.Talents = 1;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp(2);
    }

    //
    // NOTE: if HP is not set to > 0,
    // game object update and melee attack will fail.
    //
    go->Attrs.HP.Restore();

    go->Attrs.MP.Restore();
  }

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBasic* aimb = ai->AddModel<AIMonsterBasic>();
  aimb->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(8);
  aimb->ConstructAI();
  ai->ChangeModel<AIMonsterBasic>();
  // ===========================================================================

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

  // Set attributes
  if (randomize)
  {
    //int pl = _playerRef->Attrs.Lvl.Get();
    int difficulty = GetDifficulty();

    difficulty = Util::Instance().Clamp(difficulty, 1, 5);

    go->Attrs.Def.Talents = 3;
    go->Attrs.Spd.Talents = 1;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp();
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();
  }

  ItemBonusStruct ibs;

  ibs.Type       = ItemBonusType::LEVITATION;
  ibs.Id         = go->ObjectId();
  ibs.Persistent = true;

  go->AddEffect(ibs);

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBat* aimb = ai->AddModel<AIMonsterBat>();
  aimb->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(16);
  aimb->ConstructAI();
  ai->ChangeModel<AIMonsterBat>();
  // ===========================================================================

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

  // Set attributes
  if (randomize)
  {
    //int pl = _playerRef->Attrs.Lvl.Get();
    int difficulty = GetDifficulty();

    difficulty = Util::Instance().Clamp(difficulty, 1, 5);

    go->Attrs.Def.Talents = 3;
    go->Attrs.Spd.Talents = 3;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp();
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();
  }

  ItemBonusStruct ibs;

  ibs.Type       = ItemBonusType::LEVITATION;
  ibs.Id         = go->ObjectId();
  ibs.Persistent = true;

  go->AddEffect(ibs);

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterVampireBat* aimb = ai->AddModel<AIMonsterVampireBat>();
  aimb->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(16);
  aimb->ConstructAI();
  ai->ChangeModel<AIMonsterVampireBat>();
  // ===========================================================================

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

  // Set attributes
  if (randomize)
  {
    //int pl = _playerRef->Attrs.Lvl.Get();
    int difficulty = GetDifficulty();

    difficulty = Util::Instance().Clamp(difficulty, 1, 7);

    go->Attrs.Str.Talents = 2;
    go->Attrs.Def.Talents = 2;
    go->Attrs.Spd.Talents = 1;

    for (int i = 0; i < difficulty; i++)
    {
      go->LevelUp(4);
    }

    go->Attrs.HP.Restore();
    go->Attrs.MP.Restore();
  }

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterSpider* aims = ai->AddModel<AIMonsterSpider>();
  aims->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(12);
  aims->ConstructAI();
  ai->ChangeModel<AIMonsterSpider>();
  // ===========================================================================

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

  // Set attributes
  if (randomize)
  {
    //int pl = _playerRef->Attrs.Lvl.Get();
    int difficulty = GetDifficulty();

    difficulty = Util::Instance().Clamp(difficulty, 1, 10);

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

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterTroll* aims = ai->AddModel<AIMonsterTroll>();
  aims->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(1);
  aims->ConstructAI();
  ai->ChangeModel<AIMonsterTroll>();
  // ===========================================================================

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

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();
  EquipmentComponent* ec = go->AddComponent<EquipmentComponent>(cc);

  GameObject* pickaxe = ItemsFactory::Instance().CreateBlockBreakerPickaxe();
  GameObject* armor = ItemsFactory::Instance().CreateRandomArmor(ArmorType::PADDING);

  //
  // First, add objects to inventory so that we aquire the naked pointer
  //
  cc->Add(pickaxe);
  cc->Add(armor);

  ItemComponent* pickIC = pickaxe->GetComponent<ItemComponent>();
  ItemComponent* armorIC = armor->GetComponent<ItemComponent>();

  //
  // Then equip them
  //
  ec->Equip(pickIC);
  ec->Equip(armorIC);

  //
  // Everything else in inventory will be dropped on kill
  //
  GameObject* gem = ItemsFactory::Instance().CreateGem(0, 0, GemType::RANDOM, 100);
  cc->Add(gem);

  //
  // TODO: add some more plot-related dummy items
  //

  go->Attrs.ChallengeRating = GlobalConstants::AwardedExpBoss;

  go->Attrs.Str.Talents = 3;
  go->Attrs.Skl.Talents = 2;
  go->Attrs.Spd.Talents = 1;

  for (int i = 0; i <= 5; i++)
  {
    int hpToAdd = RNG::Instance().RandomRange(4, 6);
    go->LevelUp(hpToAdd);
  }

  go->Attrs.HP.Restore();
  go->Attrs.MP.Restore();

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  ai->OwnerGameObject->VisibilityRadius.Set(12);
  // ---------------------------------------------------------------------------
  AIMonsterHerobrine* aims = ai->AddModel<AIMonsterHerobrine>();
  aims->ConstructAI();
  // ---------------------------------------------------------------------------
  AIIdle* aii = ai->AddModel<AIIdle>();
  aii->ConstructAI();
  // ---------------------------------------------------------------------------
  ai->ChangeModel<AIIdle>();
  // ===========================================================================

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


  ContainerComponent* cc = go->AddComponent<ContainerComponent>();
  EquipmentComponent* ec = go->AddComponent<EquipmentComponent>(cc);

  GameObject* pick = ItemsFactory::Instance().CreateRandomMeleeWeapon(WeaponType::PICKAXE);
  GameObject* armor = ItemsFactory::Instance().CreateRandomArmor(ArmorType::PADDING);

  cc->Add(pick);
  cc->Add(armor);

  ItemComponent* pickIC = pick->GetComponent<ItemComponent>();
  ItemComponent* armorIC = armor->GetComponent<ItemComponent>();

  ec->Equip(pickIC);
  ec->Equip(armorIC);

  go->Attrs.Str.Talents = 1;
  go->Attrs.Skl.Talents = 1;
  go->Attrs.Spd.Talents = 1;

  int difficulty = GetDifficulty();

  difficulty = Util::Instance().Clamp(difficulty, 1, 10);

  for (int i = 0; i < difficulty; i++)
  {
    go->LevelUp();
  }

  go->Attrs.HP.Restore();
  go->Attrs.MP.Restore();

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterMadMiner* aim = ai->AddModel<AIMonsterMadMiner>();
  aim->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(6);
  aim->ConstructAI();
  ai->ChangeModel<AIMonsterMadMiner>();
  // ===========================================================================

  go->GenerateLootFunction = std::bind(&LootGenerators::MadMiner, go);

  return go;
}

GameObject* MonstersInc::CreateKobold(int x, int y)
{
  char img = 'k';

  GameObject* go = new GameObject(Map::Instance().CurrentLevel, x, y, img, Colors::MonsterColor);
  go->ObjectName = "Kobold";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 30;

  go->MoveTo(x, y);

  ContainerComponent* cc = go->AddComponent<ContainerComponent>(6);
  EquipmentComponent* ec = go->AddComponent<EquipmentComponent>(cc);

  auto weaponType = Util::Instance().Rolld100(50) ?
                    WeaponType::DAGGER :
                    WeaponType::SHORT_SWORD;

  GameObject* sword = ItemsFactory::Instance().CreateRandomMeleeWeapon(weaponType);

  if (Util::Instance().Rolld100(30))
  {
    ItemPrefix prefix = Util::Instance().Rolld100(25)
                      ? ItemPrefix::BLESSED
                      : ItemPrefix::UNCURSED;

    GameObject* potion = ItemsFactory::Instance().CreateHealingPotion(prefix);
    cc->Add(potion);
  }

  cc->Add(sword);

  ItemComponent* swordIC = sword->GetComponent<ItemComponent>();

  ec->Equip(swordIC);

  go->Attrs.Skl.Talents = 1;
  go->Attrs.Spd.Talents = 2;

  int difficulty = GetDifficulty();

  difficulty = Util::Instance().Clamp(difficulty, 1, 10);

  for (int i = 0; i < difficulty; i++)
  {
    go->LevelUp();
  }

  go->Attrs.HP.Restore();
  go->Attrs.MP.Restore();

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterKobold* aim = ai->AddModel<AIMonsterKobold>();
  aim->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(8);
  aim->ConstructAI();
  ai->ChangeModel<AIMonsterKobold>();
  // ===========================================================================

  go->GenerateLootFunction = std::bind(&LootGenerators::Kobold, go);

  return go;
}

GameObject* MonstersInc::CreateShelob(int x, int y)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  's',
                                  Colors::MonsterUniqueColor);
  go->ObjectName = "Shelob";
  go->Attrs.Indestructible = false;
  go->HealthRegenTurns = 15;

  go->MoveTo(x, y);

  go->Attrs.Str.Talents = 2;
  go->Attrs.Skl.Talents = 2;
  go->Attrs.Spd.Talents = 1;

  go->Attrs.ChallengeRating = GlobalConstants::AwardedExpMiniboss;

  int difficulty = GetDifficulty();

  for (int i = 0; i < difficulty; i++)
  {
    go->LevelUp(4);
  }

  go->Attrs.HP.Restore();
  go->Attrs.MP.Restore();

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterShelob* aim = ai->AddModel<AIMonsterShelob>();
  aim->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(6);
  aim->ConstructAI();
  ai->ChangeModel<AIMonsterShelob>();
  // ===========================================================================

  go->GenerateLootFunction = std::bind(&LootGenerators::Shelob, go);

  return go;
}

GameObject* MonstersInc::CreateZombie(int x, int y)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  'Z',
                                  Colors::MonsterColor);

  go->ObjectName = "Zombie";
  go->Attrs.Indestructible = false;
  go->IsLiving = false;

  go->MoveTo(x, y);

  go->Attrs.Str.Talents = 3;
  go->Attrs.HP.Talents  = 3;

  int difficulty = GetDifficulty();

  difficulty = Util::Instance().Clamp(difficulty, 1, 10);

  for (int i = 0; i < difficulty; i++)
  {
    go->LevelUp();
  }

  int hp = go->Attrs.HP.Max().OriginalValue();
  go->Attrs.HP.Reset(hp * 2);

  go->OnDestroy = [this](GameObject* go)
  {
    //
    // This lambda is called in go's destructor, so we can't
    // capture go's address anywhere, because it could become
    // invalid due to relocation after subsequent PlaceActor() into
    // CurrentLevel->ActorGameObjects, and we can't capture
    // go for TimedDestroyedComponent, because it will be deleted
    // after this lambda.
    //
    if (Util::Instance().Rolld100(80))
    {
      GameObject* remains = nullptr;
      auto pos = go->GetPosition();
      auto objs = Map::Instance().GetGameObjectsAtPosition(pos.X, pos.Y);
      for (auto& i : objs)
      {
        if (i->RemainsOf == go->ObjectId())
        {
          remains = i;
          break;
        }
      }

      if (remains != nullptr)
      {
        int timeout = go->Attrs.HP.Max().OriginalValue() * 10;
        Position pos = go->GetPosition();
        Attributes attrs = go->Attrs;
        remains->AddComponent<TimedDestroyerComponent>(timeout,
        [this, pos, attrs]()
        {
          GameObject* reanimated = CreateZombie(pos.X, pos.Y);
          reanimated->Attrs = attrs;
          reanimated->Attrs.HP.Reset(reanimated->Attrs.HP.Max().OriginalValue());
          Map::Instance().PlaceActor(reanimated);
        });
      }
    }
  };

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBasic* aim = ai->AddModel<AIMonsterBasic>();
  aim->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(10);
  aim->ConstructAI();
  ai->ChangeModel<AIMonsterBasic>();
  // ===========================================================================

  return go;
}

GameObject* MonstersInc::CreateSkeleton(int x, int y)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  'S',
                                  Colors::MonsterColor);

  go->ObjectName = "Skeleton";
  go->Attrs.Indestructible = false;
  go->IsLiving = false;

  go->MoveTo(x, y);

  go->Attrs.Spd.Talents = 1;
  go->Attrs.Skl.Talents = 3;

  int difficulty = GetDifficulty();

  difficulty = Util::Instance().Clamp(difficulty, 1, 12);

  for (int i = 0; i < difficulty; i++)
  {
    go->LevelUp(1);
  }

  go->Attrs.HP.Restore();

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();
  EquipmentComponent* ec = go->AddComponent<EquipmentComponent>(cc);

  WeaponType weaponType = Util::Instance().Rolld100(50)
                        ? WeaponType::SHORT_SWORD
                        : WeaponType::ARMING_SWORD;

  GameObject* weapon = ItemsFactory::Instance().CreateRandomMeleeWeapon(weaponType);
  GameObject* armor  = ItemsFactory::Instance().CreateRandomArmor();

  cc->Add(weapon);
  cc->Add(armor);

  ItemComponent* weaponIC = weapon->GetComponent<ItemComponent>();
  ItemComponent* armorIC  = armor->GetComponent<ItemComponent>();

  ec->Equip(weaponIC);
  ec->Equip(armorIC);

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBasic* aim = ai->AddModel<AIMonsterBasic>();
  aim->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(6);
  aim->ConstructAI();
  ai->ChangeModel<AIMonsterBasic>();
  // ===========================================================================

  return go;
}

GameObject* MonstersInc::CreateWraith(int x, int y)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  'W',
                                  Colors::None,
                                  Colors::None);

  go->ObjectName = "Wraith";
  go->Attrs.Indestructible = false;
  go->IsLiving = false;
  go->Corporeal = false;

  go->MoveTo(x, y);

  int difficulty = GetDifficulty();

  for (int i = 0; i < difficulty; i++)
  {
    go->LevelUp();
  }

  go->Attrs.ResetStats();

  go->Attrs.HP.Reset(1);

  ItemBonusStruct inv;
  inv.BonusValue = 1;
  inv.Duration = -1;
  inv.Type = ItemBonusType::INVISIBILITY;
  inv.Id = go->ObjectId();
  inv.Persistent = true;

  ItemBonusStruct fly;
  fly.BonusValue = 1;
  fly.Duration = -1;
  fly.Type = ItemBonusType::LEVITATION;
  fly.Id = go->ObjectId();
  fly.Persistent = true;

  go->AddEffect(inv);
  go->AddEffect(fly);

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterWraith* aim = ai->AddModel<AIMonsterWraith>();
  aim->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(20);
  aim->ConstructAI();
  ai->ChangeModel<AIMonsterWraith>();
  // ===========================================================================

  return go;
}

GameObject* MonstersInc::CreateStalker(int x, int y)
{
  GameObject* go = new GameObject(Map::Instance().CurrentLevel,
                                  x,
                                  y,
                                  'G',
                                  Colors::MonsterColor);

  go->ObjectName = "Gollum";
  go->Attrs.Indestructible = false;
  go->IsLiving = true;

  go->MoveTo(x, y);

  go->Attrs.ChallengeRating = GlobalConstants::AwardedExpBoss;

  int difficulty = GetDifficulty();

  for (int i = 0; i < difficulty; i++)
  {
    go->LevelUp();
  }

  go->Attrs.HP.Restore();

  ContainerComponent* cc = go->AddComponent<ContainerComponent>();
  EquipmentComponent* ec = go->AddComponent<EquipmentComponent>(cc);

  GameObject* weapon = ItemsFactory::Instance().CreateRandomMeleeWeapon(WeaponType::DAGGER, ItemPrefix::CURSED);
  GameObject* ring = ItemsFactory::Instance().CreateOneRing();

  cc->Add(weapon);
  cc->Add(ring);

  ItemComponent* weaponIC = weapon->GetComponent<ItemComponent>();
  ItemComponent* ringIC = ring->GetComponent<ItemComponent>();

  ec->Equip(weaponIC);
  ec->Equip(ringIC);

  // FIXME: proper AI

  // ===========================================================================
  AIComponent* ai = go->AddComponent<AIComponent>();
  AIMonsterBasic* aim = ai->AddModel<AIMonsterBasic>();
  aim->AIComponentRef->OwnerGameObject->VisibilityRadius.Set(6);
  aim->ConstructAI();
  ai->ChangeModel<AIMonsterBasic>();
  // ===========================================================================

  return go;
}

// *****************************************************************************

int MonstersInc::GetDifficulty()
{
  int dl = Map::Instance().CurrentLevel->DungeonLevel;
  int difficulty = dl;
  int diffOffset = RNG::Instance().RandomRange(0, 4);

  difficulty += diffOffset;

  return difficulty;
}
