#include "start-game-state.h"

#include "application.h"
#include "printer.h"
#include "map.h"
#include "map-level-town.h"

#include "items-factory.h"

void StartGameState::HandleInput()
{
  //
  // Potion colors and scroll names for current game session are generated
  // randomly before start.
  //
  ItemsFactory::Instance().Reset();

  Printer::Instance().Clear();
  Printer::Instance().Render();

  Map::Instance().LoadTown();

  auto& curLvl    = Map::Instance().CurrentLevel;
  auto& playerRef = Application::Instance().PlayerInstance;

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

  Application::Instance().ChangeState(GameStates::MAIN_STATE);
}

// =============================================================================

void StartGameState::Update(bool forceUpdate)
{
}
