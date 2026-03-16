#ifndef STRUCTS_H
#define STRUCTS_H

#include "constants.h"

// =============================================================================

#ifdef USE_SDL
struct TileColor
{
  uint32_t R = 0;
  uint32_t G = 0;
  uint32_t B = 0;
};

// =============================================================================

struct TileInfo
{
  int X = 0;
  int Y = 0;
};
#else
struct FBPixel
{
  size_t ColorPairHash;
  int Character;
};
#endif

// =============================================================================

struct GameLogMessageData
{
  std::string Message;

  uint32_t FgColor = Colors::White;
  uint32_t BgColor = Colors::Black;
};

// =============================================================================

struct GraphicTileInfo
{
  GraphicTiles Tile = GraphicTiles::NONE;
  uint32_t ColorTint = Colors::White;
  double ScaleFactor = 1.0;
  uint16_t RotationDegrees = 0;
  // Bitmask of SDL_RenderFlip values. uint8_t here so that it can compile in
  // both build variants.
  uint8_t FlipMask = 0x0;  
};

// =============================================================================

struct NColor
{
  short ColorIndex;
  short R;
  short G;
  short B;
};

// =============================================================================

struct ColorPair
{
  NColor FgColor;
  NColor BgColor;
  short PairIndex;
};

#endif // STRUCTS_H

