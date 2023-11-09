#ifndef GAMEOBJECTINFO_H
#define GAMEOBJECTINFO_H

#include <string>

#ifdef __MINGW64__
#include <stdint.h>
#endif

#include "constants.h"

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
           const std::string& fowName = std::string());

  bool IsBlocking  = false;
  bool BlocksSight = false;
  int Image        = '?';

  uint32_t FgColor = Colors::WhiteColor;
  uint32_t BgColor = Colors::MagentaColor;

  std::string ObjectName;
  std::string FogOfWarName;

  bool IsSet = false;
};

#endif // GAMEOBJECTINFO_H
