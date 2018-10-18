#ifndef CONSTANTS_H
#define CONSTANTS_H

// 262, 259, 339, 260, 350, 261, 360, 258, 338

#if defined(__unix__) || defined(__linux__)

  #define NUMPAD_7  262
  #define NUMPAD_8  259
  #define NUMPAD_9  339
  #define NUMPAD_4  260
  #define NUMPAD_5  350
  #define NUMPAD_6  261
  #define NUMPAD_1  360
  #define NUMPAD_2  258
  #define NUMPAD_3  338

  // ********************

  #define VK_BACKSPACE 127
  #define VK_ENTER     10

#else

  #define NUMPAD_7  KEY_A1
  #define NUMPAD_8  KEY_A2
  #define NUMPAD_9  KEY_A3
  #define NUMPAD_4  KEY_B1
  #define NUMPAD_5  KEY_B2
  #define NUMPAD_6  KEY_B3
  #define NUMPAD_1  KEY_C1
  #define NUMPAD_2  KEY_C2
  #define NUMPAD_3  KEY_C3

  // ********************

  #define VK_BACKSPACE 8
  #define VK_ENTER     10

#endif

#include <vector>
#include <string>

enum class ItemType
{
  COINS = 0,
  FOOD,
  SCROLL
};

enum class RoomLayoutRotation
{
  CCW_90 = 0,
  CCW_180,
  CCW_270
};

struct Attribute
{
  int CurrentValue = 0;
  int OriginalValue = 0;
  int Modifier = 0;
  bool IsTalent = false;

  void Set(int value)
  {
    CurrentValue = value;
    OriginalValue = value;
  }

  int Get()
  {
    return CurrentValue + Modifier;
  }
};

struct Attributes
{
  Attribute Str;
  Attribute Def;
  Attribute Mag;
  Attribute Res;
  Attribute Skl;
  Attribute Spd;

  Attribute HP;
  Attribute MP;

  // number of turns before Hunger decrements by HungerSpeed
  Attribute HungerRate;
  Attribute HungerSpeed;

  Attribute Exp;
  Attribute Lvl;

  int Hunger = 1000;
};

namespace GlobalConstants
{
  static std::string Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  static const int MapX = 100; // 160
  static const int MapY = 100; // 45

  static const std::string BlackColor = "#000000";
  static const std::string WallColor = "#888888";
  static const std::string TreeColor = "#00FF00";
  static const std::string PlayerColor = "#00FFFF";
  static const std::string NpcColor = "#FFFF00";
  static const std::string GroundColor = "#444444";
  static const std::string RoomFloorColor = "#692E11";
  static const std::string PavingStoneColor = "#333333";
  static const std::string MountainsColor = "#666666";
  static const std::string FogOfWarColor = "#202020";
  static const std::string CoinsColor = "#FFD700";
  static const std::string DoorHighlightColor = "#444400";

  static const std::vector<std::string> RandomNames =
  {    
    "Kornel Kisielewicz",
    "Darren Grey",
    "Thomas Biskup",
    "Glenn Wichman",
    "Michael Toy",
    "Ken Arnold",
    "Jon Lane",
    "Mike Stephenson",
    "Markus Persson"
  };

  static const std::vector<std::vector<std::string>> RoomLayouts =
  {
    // Common house
    {
      "##+##",
      "#...#",
      "#...#",
      "#...#",
      "#####"
    },
    // Church
    {
      "###-#######-###",
      "#..h.h.h.h....#",
      "+..h.h.h.h....|",
      "#........../..#",
      "+..h.h.h.h....|",
      "#..h.h.h.h....#",
      "###-#######-###"
    },
    // Trader
    {
      "#########",
      "#.......#",
      "+.......#",
      "#.......#",
      "#########"
    },
    // Dungeon rooms
    {
      "##.##",
      "#...#",
      "..#..",
      "#...#",
      "##.##"
    },
    {
      "##.##",
      "#...#",
      "..#..",
      "#...#",
      "#####"
    },
    {
      "#.#.#",
      ".....",
      "#.#.#",
      ".....",
      "#.#.#"
    },
    // Shrine
    {
      "#####",
      "#...#",
      "+./.#",
      "#...#",
      "#####"
    }
  };
}

#endif
