#ifndef GAMEOBJECTINFO_H
#define GAMEOBJECTINFO_H

#include "constants.h"
#include "structs.h"

///
/// Helper struct to reduce the amount of writing when creating objects.
///
struct GameObjectInfo
{
  void Set(bool isBlocking,
           bool blocksSight,
           int image,
           const uint32_t& fgColor,
           const uint32_t& bgColor,
           const std::string& objectName,
           const std::string& fowName);

  void SetGraphics(const GraphicTileInfo& gti);
  void SetGraphics(GraphicTiles tile,
                   uint32_t colorTint = Colors::White,
                   double scaleFactor = 1.0,
                   uint16_t rotationDegrees = 0,
                   uint8_t flipMask = 0x0);

  bool IsBlocking  = false;
  bool BlocksSight = false;
  int Image        = '?';

  GraphicTileInfo Graphic;

  uint32_t FgColor = Colors::White;
  uint32_t BgColor = Colors::Magenta;

  std::string ObjectName;
  std::string FogOfWarName;
};

#endif // GAMEOBJECTINFO_H
