#include "game-object-info.h"

void GameObjectInfo::Set(bool isBlocking,
                         bool blocksSight,
                         int image,
                         const std::string &fgColor,
                         const std::string &bgColor,
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

  IsSet = true;
}
