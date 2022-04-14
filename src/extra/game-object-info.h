#ifndef GAMEOBJECTINFO_H
#define GAMEOBJECTINFO_H

#include <string>

///
/// Helper struct to reduce the amount of writing when creating objects
///
struct GameObjectInfo
{
  void Set(bool isBlocking,
           bool blocksSight,
           int image,
           const std::string& fgColor,
           const std::string& bgColor,
           const std::string& objectName,
           const std::string& fowName = std::string());

  bool IsBlocking;
  bool BlocksSight;
  int Image;

  std::string FgColor;
  std::string BgColor;

  std::string ObjectName;
  std::string FogOfWarName;

  bool IsSet = false;
};

#endif // GAMEOBJECTINFO_H
