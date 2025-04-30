#include "map-level-endgame.h"

#include "application.h"

MapLevelEndgame::MapLevelEndgame(int sizeX,
                                 int sizeY,
                                 MapType type,
                                 int dungeonLevel)
  : MapLevelBase(sizeX, sizeY, type, dungeonLevel)
{
  _specialLevel =
  {
    "########",
    "#......#",
    "#.<..>.#",
    "#......#",
    "########"
  };

  //
  // Note that x and y are swapped to correspond to "world" dimensions.
  //
  int sx = _specialLevel[0].length();
  int sy = _specialLevel.size();

  MapSize.Set(sx, sy);
}

// =============================================================================

void MapLevelEndgame::PrepareMap()
{
  MysteriousForcePresent = true;

  MapLevelBase::PrepareMap();

  CreateLevel();
}

// =============================================================================

void MapLevelEndgame::DisplayWelcomeText()
{
  std::vector<std::string> msg =
  {
    { HIDE("You don't know where you are anymore...") }
  };

  Application::Instance().ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
                                         Strings::TripleQuestionMarks,
                                         msg);
}

// =============================================================================

void MapLevelEndgame::CreateLevel()
{
  // TODO: create level from _specialLevel here
}

// =============================================================================

void MapLevelEndgame::CreateCommonObjects(int x, int y, char image)
{
}

