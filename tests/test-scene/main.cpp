#include "gid-generator.h"
#include "application.h"
#include "game-objects-factory.h"
#include "spells-processor.h"
#include "items-factory.h"
#include "monsters-inc.h"
#include "blackboard.h"
#include "bts-decompiler.h"
#include "map.h"
#include "timer.h"
#include "util.h"
#include "rng.h"

#ifdef DEBUG_BUILD
#include "logger.h"
#endif

int main(int argc, char* argv[])
{
  GID::Instance().Init();
  RNG::Instance().Init();

  Blackboard::Instance().Init();
  Timer::Instance().Init();

#ifdef DEBUG_BUILD
  Logger::Instance().Init();
  Logger::Instance().Prepare(false);
#endif

  BTSDecompiler::Instance().Init();

  Application::Instance().Init();

  if (!Application::Instance().IsAppReady())
  {
    ConsoleLog("There was an error during application initialization - "
               "no sense in continuing");
    return 1;
  }

  GameObjectsFactory::Instance().Init();
  ItemsFactory::Instance().Init();
  MonstersInc::Instance().Init();

  SpellsDatabase::Instance().Init();
  SpellsProcessor::Instance().Init();

  Map::Instance().Init();

  Map::Instance().LoadTestLevel();

  auto& curLvl    = Map::Instance().CurrentLevel;
  auto& playerRef = Application::Instance().PlayerInstance;

  playerRef.SetLevelOwner(curLvl);
  playerRef.Init();
  playerRef.MoveTo({ 1, 1 });
  playerRef.AddExtraItems();
  playerRef.VisibilityRadius.Set(curLvl->VisibilityRadius);

  curLvl->AdjustCamera();

  Application::Instance().ChangeState(GameStates::MAIN_STATE);

  Application::Instance().Run();
  Application::Instance().Cleanup();

  return 0;
}
