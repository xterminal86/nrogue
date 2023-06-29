#ifndef BLOBTILES_H
#define BLOBTILES_H

#include <unordered_map>

#include "dg-base.h"

class BlobTiles : public DGBase
{
  public:
    void Generate(int mapSizeX, int mapSizeY);

    void ForCustomDebugStuff() override;

  private:
    std::vector<std::vector<StringV>> _tilesetToUse;

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

    //
    // Might be customizable in the future.
    //
    const int _tileSize = _tilesetBase[0].size();

    void PrintTiles();
    void PlaceTile(int x, int y, const StringV& tile);

    bool CheckEdge(int x, int y,
                   RoomEdgeEnum edge,
                   const StringV& tileToCheck);

    StringV GetMapChunkAround(int x, int y);

    #ifdef DEBUG_BUILD
    void PrintMap(int curX, int curY);
    void PrintTile(const StringsArray2D& tile);
    #endif
};

#endif // BLOBTILES_H
