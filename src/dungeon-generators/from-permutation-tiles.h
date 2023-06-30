#ifndef FROMTILES_H
#define FROMTILES_H

#include "dg-base.h"

#include <stack>
#include <sstream>

class FromPermutationTiles : public DGBase
{
  public:
    void Generate(const Position& mspSize,
                  int tileSetIndex = -1,
                  bool postProcess = false,
                  bool removeBias = false);

  private:
    void CreateTileset();
    void TryToPlaceLayout(const Position& p);
    void PlaceLayout(const Position& pos, const Tile& tile);
    void AddPointsToProcess(const Position& p);
    void ReplaceSpacesWithWalls();

    std::string GetLayoutEdge(const Tile& l, const RoomEdgeEnum& along);

    Tile ReadLayout(const Position& p);
    Tile ConvertStringToTile(const std::string& line);

    bool AreLayoutsEqual(const Tile& l1, const Tile& l2);
    bool WasVisited(const Position& p);
    bool IsLayoutEmpty(const Tile& layout);
    bool CanBeConnected(const Tile& l1,
                        const Tile& l2,
                        const RoomEdgeEnum& along);

    std::vector<RoomLayoutRotation> _allRotations =
    {
      RoomLayoutRotation::CCW_90,
      RoomLayoutRotation::CCW_180,
      RoomLayoutRotation::CCW_270
    };

    int _tileArea = 0;
    int _tileEdgeLength = 0;

    std::vector<Position> _visited;
    std::stack<Position> _toProcess;

    Tileset _tileset;
    Tiles _currentTileset;
};

#endif // FROMTILES_H
