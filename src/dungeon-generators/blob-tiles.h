#ifndef BLOBTILES_H
#define BLOBTILES_H

#include <unordered_map>

#include "dg-base.h"

class BlobTiles : public DGBase
{
  public:
    void Generate(int mapSizeX, int mapSizeY,
                  int tileSizeFactor,
                  int wallsSizeFactor,
                  bool postProcess);

    void ForCustomDebugStuff() override;

  private:
    std::vector<std::vector<StringV>> _tilesetToUse;

    std::vector<StringV> _tilesetBaseAdjusted;

    const std::vector<StringV> _tilesetBase =
    {
      // 0
      {
        "###",
        "###",
        "###"
      },
      // 1
      {
        "#.#",
        "#.#",
        "###"
      },
      // 2
      {
        "#.#",
        "#..",
        "###"
      },
      // 3
      {
        "#..",
        "#..",
        "###"
      },
      // 4
      {
        "#.#",
        "#.#",
        "#.#"
      },
      // 5
      {
        "#.#",
        "#..",
        "#.#"
      },
      // 6
      {
        "#..",
        "#..",
        "#.#"
      },
      // 7
      {
        "#.#",
        "#..",
        "#.."
      },
      // 8
      {
        "#..",
        "#..",
        "#.."
      },
      // 9
      {
        "#.#",
        "...",
        "#.#"
      },
      // 10
      {
        "#..",
        "...",
        "#.#"
      },
      // 11
      {
        "#..",
        "...",
        "#.."
      },
      // 12
      {
        "#..",
        "...",
        "..#"
      },
      // 13
      {
        "#..",
        "...",
        "..."
      },
      // 14
      {
        "...",
        "...",
        "..."
      },
    };

    int _tileSize = 3;

    int _wallsSizeFactor = 1;
    int _tileSizeFactor  = 1;

    bool _postProcess = false;

    void PrepareBaseTiles();
    void PrintTiles();
    void PlaceTile(int x, int y, const StringV& tile);
    void EnlargeTile(StringV& tile);
    void ExtendWalls(StringV& tile);
    void TryToRemoveLoneBlocks();
    void TryToRemoveDeadEndCorridors();

    bool CheckEdge(int x, int y,
                   RoomEdgeEnum edge,
                   const StringV& tileToCheck);

    bool AreChunksEqual(const StringV& chunk1, const StringV& chunk2);

    StringV GetMapChunkAround(int x, int y);

    #ifdef DEBUG_BUILD
    void PrintMap(int curX, int curY);
    void PrintTile(const StringV& tile);
    #endif
};

#endif // BLOBTILES_H
