#include "start-game-state.h"

#include "application.h"
#include "printer.h"
#include "map.h"
#include "map-level-town.h"

#include "items-factory.h"

void StartGameState::HandleInput()
{
  Game::gPrnt.Clear();
  Game::gPrnt.Render();

  Game::gMap.LoadTown();

  auto& curLvl    = Game::gMap.CurrentLevel;
  auto& playerRef = Game::gApp.PlayerInstance;

  //
  // Some NPCs contain bonus lines
  // depending on selected player character class and stats,
  // as well as other initializations (e.g. food cost in shops
  // depends on player hunger rate which is determined by selected class)
  // so we need to initialize player first.
  //
  playerRef.SetLevelOwner(curLvl);
  playerRef.Init();
  playerRef.MoveTo({ 5, 8 });
  playerRef.AddExtraItems();
  playerRef.VisibilityRadius.Set(curLvl->VisibilityRadius);

  curLvl->AdjustCamera();

  MapLevelTown* mlt = static_cast<MapLevelTown*>(curLvl);
  mlt->CreateNPCs();

  Game::gApp.ChangeState(GameStates::MAIN_STATE);
}

// =============================================================================

void StartGameState::Update(bool forceUpdate)
{
}
