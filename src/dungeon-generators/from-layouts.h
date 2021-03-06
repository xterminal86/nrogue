#ifndef FROMLAYOUTS_H
#define FROMLAYOUTS_H

#include <stack>

#include "dg-base.h"

#include "room-helper.h"

// FIXME: needs further improvement (low / discard priority)
class FromLayouts : public DGBase
{
  public:
    void Generate(const std::vector<RoomForLevel>& possibleRooms, int startX, int startY, int mapSizeX, int mapSizeY);

    std::vector<RoomHelper> MapChunks;

  private:
    std::vector<RoomLayoutRotation> _allDegrees =
    {
      RoomLayoutRotation::NONE,
      RoomLayoutRotation::CCW_90,
      RoomLayoutRotation::CCW_180,
      RoomLayoutRotation::CCW_270
    };

    int _roomsCount = 0;

    std::stack<RoomHelper> _rooms;

    std::vector<std::vector<MapCell>> _visitedCells;
    std::vector<RoomForLevel> _roomsForLevel;

    RoomLayout SelectRoom();

    void TryToAddRoomTo(const RoomHelper& currentRoom, RoomEdgeEnum side);
    void VisitCells(const RoomHelper& room);
    void ConvertChunksToMapRaw();

    bool IsAreaVisited(const Position& start, int roomSize);

    std::vector<RoomHelper> GetRoomsForLayout(const RoomLayout& layout, RoomEdgeEnum side);
    std::vector<RoomLayout> SelectRooms();
};

#endif // FROMLAYOUTS_H
