#ifndef BLOBTILES_H
#define BLOBTILES_H

#include <unordered_map>

#include "dg-base.h"

#include "room-helper.h"

class BlobTiles : public DGBase
{
  public:
    void Generate(int mapSizeX, int mapSizeY);

    void ForCustomDebugStuff() override;

  private:
    std::vector<std::vector<StringsArray2D>> _tilesetToUse;

    const std::vector<StringsArray2D> _tilesetBase =
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
    void PlaceTile(int x, int y, const StringsArray2D& tile);

    bool CheckEdge(int x, int y,
                   RoomEdgeEnum edge,
                   const StringsArray2D& tileToCheck);

    StringsArray2D GetMapChunkAround(int x, int y);

    #ifdef DEBUG_BUILD
    void PrintMap(int curX, int curY);
    void PrintTile(const StringsArray2D& tile);
    #endif
};

#endif // BLOBTILES_H
