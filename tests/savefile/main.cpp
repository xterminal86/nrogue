#if 0
#include "serializer.h"

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

bool LoadGame = false;

void SaveGameTest()
{
  // ---------------------------------------------------------------------------

  Game::gGid.Init();
  Game::gRng.Init();

  Game::gBB.Init();
  Game::gTimer.Init();

#ifdef DEBUG_BUILD
  Game::gLogger.Init();
  Game::gLogger.Prepare(false);
#endif

  Game::gBts.Init();

  Game::gApp.Init();

  Game::gGOF.Init();
  Game::gIF.Init();
  Game::gMI.Init();

  Game::gSD.Init();
  Game::gSP.Init();

  Game::gMap.Init();

  // ---------------------------------------------------------------------------

  //
  // Serialization of 40k simple Test objects from SerializeObjects() test
  // (which is 200x200, currently the largest in-game map possible) takes around
  // 7 seconds. Given the fact that GameObject is much more complex than test
  // Test class, and given the fact that we have 28 maps, this starting to look
  // not so good. We need to deal with this issue somehow.
  // We're going to assume that several objects are not going to change much,
  // or at least they're not expected to have any special behaviour: objects
  // like walls and water, for example. So we'll serialize these common objects
  // (walls, floor, lava, water etc., basically everything inside
  // MapLevelBase::CreateCommonObjects()) as char array, basically in MapRaw
  // format.
  // Then, every special game object / actor is serialized as needed.
  // Of course, theoretically you could attach components to walls, water and
  // so on, but we're not going to account for that and deal with it if need
  // arises (and when we get there).
  //

  Game::gMap.LoadLevel();

  auto& curLvl    = Game::gMap.CurrentLevel;
  auto& playerRef = Game::gApp.PlayerInstance;

  playerRef.SetLevelOwner(curLvl);
  playerRef.Init();
  playerRef.MoveTo({ 1, 1 });
  playerRef.AddExtraItems();
  playerRef.VisibilityRadius.Set(curLvl->VisibilityRadius);

  curLvl->AdjustCamera();

  Game::gApp.ChangeState(GameStates::MAIN_STATE);

  // ---------------------------------------------------------------------------

  Game::gApp.Run();
  Game::gApp.Cleanup();
}

// =============================================================================

int main(int argc, char* argv[])
{
  LoadGame = (argc > 1);

  Game::gRng.Init();

  if (!LoadGame)
  {
    SaveGameTest();
  }
  else
  {
    NRS save;
    NRS::LoadResult res = save.Load(Strings::SaveFileName);
    if (res != NRS::LoadResult::LOAD_OK)
    {
      printf("load failed, reason: %s\n", NRS::LoadResultToString(res));
    }
    else
    {
      printf("%s\n", save.ToPrettyString().data());
      printf("%s\n", save.DumpObjectStructureToString().data());
    }
  }

  return 0;
}
#endif

// TODO: replace old implementation with singletons.
int main(int argc, char* argv[])
{
  return 0;
}


