#ifndef ROOMS_H
#define ROOMS_H

#include <memory>

#include "dg-base.h"

#include "rect.h"

struct BSPNode
{
  Position CornerStart;
  Position CornerEnd;

  std::unique_ptr<BSPNode> Left;
  std::unique_ptr<BSPNode> Right;
};

class BSPRooms : public DGBase
{
  public:
    void Generate(const Position& mapSize,
                  const Position& splitRatio,
                  int minRoomSize);

  private:
    Position _splitRatio;

    int _minRoomArea = 0;
    int _minRoomSize = 0;

    std::vector<Position> _connectionPoints;

    std::pair<bool, double> GetSplitRatio(Rect& area);

    void Subdivide(BSPNode& parent, double ratio, bool splitX);
    void FillArea(const Rect& area, char ch = '.');

    bool DoesRoomFit(Rect& area);
    bool WasFilled(const Rect& area);

    void Traverse(BSPNode* node, int depth);
    void ConnectPoints(const Position& p1, const Position& p2);
    void PlaceDoors();

    bool IsSpotValidForDoor(const Position& p);
    bool ComparePatterns(std::vector<std::string>& p1,
                         std::vector<std::string>& p2);

    std::vector<Position> FindPlaceForDoor();
};

#endif // ROOMS_H
