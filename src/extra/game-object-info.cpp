#include "game-object-info.h"

void GameObjectInfo::Set(bool isBlocking,
                         bool blocksSight,
                         int image,
                         const uint32_t& fgColor,
                         const uint32_t& bgColor,
                         const std::string &objectName,
                         const std::string &fowName)
{
  IsBlocking   = isBlocking;
  BlocksSight  = blocksSight;
  Image        = image;
  FgColor      = fgColor;
  BgColor      = bgColor;
  ObjectName   = objectName;
  FogOfWarName = fowName;
}

void GameObjectInfo::SetGraphics(const GraphicTileInfo& gti)
{
  Graphic = gti;
}

void GameObjectInfo::SetGraphics(GraphicTiles tile,
                                 uint32_t colorTint,
                                 double scaleFactor,
                                 uint16_t rotationDegrees,
                                 uint8_t flipMask)
{
  Graphic.Tile            = tile;
  Graphic.ColorTint       = colorTint;
  Graphic.ScaleFactor     = scaleFactor;
  Graphic.RotationDegrees = rotationDegrees;
  Graphic.FlipMask        = flipMask;
}

