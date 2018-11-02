#include "map-level-base.h"

MapLevelBase::MapLevelBase(int sizeX, int sizeY, MapType type)
{
  MapSize.X = sizeX;
  MapSize.Y = sizeY;
  MapType_ = type;

  auto levelNames = GlobalConstants::MapLevelRandomNames.at(MapType_);
  int index = RNG::Instance().RandomRange(0, levelNames.size());
  LevelName = levelNames[index];
}

void MapLevelBase::SetPlayerStartingPosition(int x, int y)
{
  PlayerStartX = x;
  PlayerStartY = y;

  int tw = Printer::Instance().TerminalWidth;
  int th = Printer::Instance().TerminalHeight;

  MapOffsetX = tw / 2 - PlayerStartX;
  MapOffsetY = th / 2 - PlayerStartY;
}
