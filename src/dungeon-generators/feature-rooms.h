#ifndef FEATUREROOMS_H
#define FEATUREROOMS_H

#include "dg-base.h"
#include "constants.h"

class FeatureRooms : public DGBase
{
  using RoomLayout = std::vector<std::vector<std::string>>;

  public:
    void Generate(const Position& mapSize,
                  const Position& roomSizes,
                  const FeatureRoomsWeights& weightsMap,
                  uint8_t doorPlacementChance,
                  int maxIterations);

  private:
    bool CreateEmptyRoom(const Position& start,
                         const Position& size,
                         RoomEdgeEnum dir,
                         char ground = '.');

    Position GetRandomRoomSize();
    Position GetOffsetsForDirection(RoomEdgeEnum dir);
    RoomEdgeEnum GetCarveDirectionForDeadend(Position pos);

    void FillWithEmpty(const Position& start,
                       const Position& end);

    bool CreateShrine(const Position& start,
                      RoomEdgeEnum dir,
                      ShrineType type);

    bool CreateDiamondRoom(const Position& start,
                           int size,
                           RoomEdgeEnum dir);

    bool CreateRoundRoom(const Position& start,
                         int radius,
                         RoomEdgeEnum dir);

    bool PlaceLayout(const Position& start,
                     StringV& layout,
                     RoomEdgeEnum dir,
                     bool demonize = false);

    void CreateStartingRoom();

    std::vector<Position> GetValidCellsToCarveFrom();

    bool IsCellValid(const Position& pos);
    bool IsAreaValid(const Position& start, const Position& end);

    bool TryToCreateRoom(const Position& doorPos,
                         const Position& newRoomStartPos,
                         RoomEdgeEnum direction,
                         FeatureRoomType roomType);

    void DemonizeLayout(StringV& layout);

    std::pair<Position, Position> CenterRoomAlongDir(const Position& start, int size, RoomEdgeEnum dir);

    FeatureRoomsWeights _weightsMap;

    Position _roomSizes;

    std::unordered_map<FeatureRoomType, int> _generatedSoFar;
    std::unordered_map<FeatureRoomType, int> _roomWeightByType;

    const std::unordered_map<FeatureRoomType, std::vector<StringV>> _specialRoomLayoutByType =
    {
      { FeatureRoomType::GARDEN,   GlobalConstants::GardenLayouts   },
      { FeatureRoomType::POND,     GlobalConstants::PondLayouts     },
      { FeatureRoomType::FOUNTAIN, GlobalConstants::FountainLayouts }
    };
};

#endif // FEATUREROOMS_H
