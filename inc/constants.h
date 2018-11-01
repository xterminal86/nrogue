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
#include <map>
#include <functional>

enum class MonsterType
{
  RAT = 0,
  BAT,
  SPIDER,
  SKELETON
};

enum class EquipmentCategory
{
  NOT_EQUIPPABLE = 0,
  HEAD,
  NECK,
  TORSO,
  LEGS,
  BOOTS,
  WEAPON,
  RING
};

enum class ItemType
{
  DUMMY = 0,
  COINS,
  HEALING_POTION,
  MANA_POTION,
  FOOD,
  SCROLL,
  WEAPON
};

enum class ShrineType
{
  MIGHT = 0,
  SPIRIT
};

enum class RoomLayoutRotation
{
  NONE = 0,
  CCW_90,
  CCW_180,
  CCW_270
};

struct Attribute
{
  int CurrentValue = 0;
  int OriginalValue = 0;
  int Modifier = 0;
  int Talents = 0;

  void Set(int value)
  {
    OriginalValue = value;
    CurrentValue = value;    
  }

  int Get()
  {
    return OriginalValue + Modifier;
  }
};

struct Attributes
{
  Attributes()
  {
    Lvl.Set(1);
  }

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
  int ActionMeter = 0;

  bool Indestructible = true;
};

enum class ItemPrefix
{
  UNCURSED = 0,
  BLESSED,
  CURSED
};

enum class ItemRarity
{
  COMMON = 0,
  MAGIC,
  RARE
};

enum class StatsEnum
{
  STR = 0,
  DEF,
  MAG,
  RES,
  SKL,
  SPD,
  HP,
  MP
};

struct ItemData
{
  ItemType TypeOfItem = ItemType::DUMMY;
  ItemPrefix Prefix = ItemPrefix::UNCURSED;
  ItemRarity Rarity = ItemRarity::COMMON;

  EquipmentCategory EqCategory = EquipmentCategory::NOT_EQUIPPABLE;

  Attribute Durability;
  Attribute Damage;

  bool IsUsable = false;
  bool IsEquipped = false;
  bool IsStackable = false;
  bool IsIdentified = false;

  int Amount = 1;
  int Cost = 0;

  std::map<StatsEnum, int> StatBonuses =
  {
    { StatsEnum::STR, 0 },
    { StatsEnum::DEF, 0 },
    { StatsEnum::MAG, 0 },
    { StatsEnum::RES, 0 },
    { StatsEnum::SKL, 0 },
    { StatsEnum::SPD, 0 },
    { StatsEnum::HP, 0 },
    { StatsEnum::MP, 0 }
  };

  std::string IdentifiedName;
  std::string UnidentifiedName;

  std::vector<std::string> UnidentifiedDescription;
  std::vector<std::string> IdentifiedDescription;

  std::function<void(void*)> UseCallback;
  std::function<void(void*)> EquipCallback;

  size_t ItemTypeHash;
};

namespace GlobalConstants
{
  static const std::string Alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  static const int MapX = 100; // 160
  static const int MapY = 50; // 45

  static const int HPMPBarLength = 20;
  static const int AttributeMinimumRaiseChance = 15;
  static const int AttributeIncreasedRaiseStep = 25;
  static const int AwardedExperienceStep = 5;
  static const int AwardedExpDefault = 20;
  static const int AwardedExpMax = 40;
  static const int MinHitChance = 5;
  static const int MaxHitChance = 95;

  static const std::string BlackColor = "#000000";
  static const std::string WhiteColor = "#FFFFFF";
  static const std::string WallColor = "#888888";
  static const std::string GrassColor = "#006600";
  static const std::string TreeColor = "#00FF00";
  static const std::string PlayerColor = "#00FFFF";
  static const std::string MonsterColor = "#FF0000";
  static const std::string WaterColor = "#3333FF";
  static const std::string NpcColor = "#FFFF00";
  static const std::string GroundColor = "#444444";
  static const std::string RoomFloorColor = "#692E11";  
  static const std::string MountainsColor = "#666666";
  static const std::string FogOfWarColor = "#202020";
  static const std::string CoinsColor = "#FFD700";
  static const std::string DoorHighlightColor = "#444400";
  static const std::string ShrineMightColor = "#FF0000";
  static const std::string ShrineSpiritColor = "#0044FF";
  static const std::string MessageBoxDefaultBgColor = "#222222";
  static const std::string MessageBoxDefaultBorderColor = "#666666";
  static const std::string MessageBoxRedBorderColor = "#880000";
  static const std::string ItemMagicColor = "#4169E1";
  static const std::string ItemRareColor = "#CCCC52";
  static const std::string ItemUniqueColor = "#A59263";

  static const std::map<std::string, std::vector<std::string>> PotionColors =
  {
    { "Red Potion", { "#FFFFFF", "#FF0000" } },
    { "Green Potion", { "#FFFFFF", "#00FF00" } },
    { "Radiant Potion", { "#666666", "#FFFF88" } },
    { "Morbid Potion", { "#FFFFFF", "#660000" } },
    { "Blue Potion", { "#FFFFFF", "#0000FF" } },
    { "Yellow Potion", { "#000000", "#FFFF00" } },
    { "Clear Potion", { "#000000", "#CCCCCC" } },
    { "Black Potion", { "#FFFFFF", "#000000" } },
    { "Cyan Potion", { "#FFFFFF", "#00FFFF" } },
    { "Magenta Potion", { "#FFFFFF", "#FF00FF" } }
  };

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
    // Common houses
    {
      "##+##",
      "#...#",
      "#...#",
      "#...#",
      "##-##"
    },
    {
      "#+#####",
      "#..#..#",
      "|..+..|",
      "#..#..#",
      "#######"
    },
    {
      "###+###",
      "#.....#",
      "#.....#",
      "|..#+##",
      "#..#..#",
      "#..#..#",
      "#######"
    },
    // Rich residents
    {
      "####+######-###",
      "#.......#.....#",
      "|.......+.....|",
      "#.......#.....#",
      "##+###+##.....#",
      "#...#...#.....#",
      "#...#...#.....|",
      "#...#...#.....#",
      "##-###-####-###"
    },
    {
      "#########-#####",
      "#......#......#",
      "|......#......|",
      "#......#......#",
      "##+########+###",
      "#   #ggggggggg#",
      "#   #g#  #  #g#",
      "+    g wwwww g#",
      "#   #g wwwww g#",
      "+    g wwwww g#",
      "#   #g#  #  #g#",
      "#   #ggggggggg#",
      "##+########+###",
      "#......#......#",
      "|......#......|",
      "#......#......#",
      "#########-#####"
    },
    {
      "####+##########",
      "#ggg ggg#.....#",
      "#gFg gFg#.....|",
      "#ggg ggg|.....#",
      "#ggg ggg|.....#",
      "#gFg gFg#.....|",
      "#ggg ggg#.....#",
      "####+######+###",
      "#.......#.....#",
      "#.......#.....#",
      "|.......+.....|",
      "#.......#.....#",
      "#.......#.....#",
      "###############"
    },
    // Castle
    {
      "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwww",
      "w###-###wwwwwwwwwwwwwww###-###w",
      "w#     #wwwwwwwwwwwwwww#     #w",
      "w#     #################     #w",
      "w|                           |w",
      "w#     #################     #w",
      "w#     #.#######.......#     #w",
      "w### #+#.#hh   S.......#+# ###w",
      "www# #...+   / S.........# #www",
      "www# #...#hh   S..######## #www",
      "www# #...#######..#     ## #www",
      "www# #............#     ## #www",
      ".#######..........+     ## #www",
      ".+.....+...ggg....+     ## #www",
      ".+.....+...gFg....#     ## #www",
      ".#######...ggg....#     ## #www",
      "www# #............######## #www",
      "www# #...................# #www",
      "www# #..##+###+###+##....# #www",
      "www# #..#   #   #   #....# #www",
      "w### #+##   #   #   #..#+# ###w",
      "w#     ##############..#     #w",
      "w#     #################     #w",
      "w|                           |w",
      "w#     #################     #w",
      "w#     #wwwwwwwwwwwwwww#     #w",
      "w###-###wwwwwwwwwwwwwww###-###w",
      "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwww",
    },
    // Church
    {
      "................####-####........",
      "................#       #........",
      "................#       #........",
      "................|       |........",
      "................#       #........",
      "................#       #........",
      "####-#######-#######+#######-####",
      "#    h h h h h h        #       #",
      "+    h h h h h h        #       |",
      "#                       +   /   |",
      "+    h h h h h h        #       |",
      "#    h h h h h h        #       #",
      "####-#######-#######+#######-####",
      "................#       #........",
      "................#       #........",
      "................|       |........",
      "................#       #........",
      "................#       #........",
      "................####-####........",
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
    {
      "###.#",
      ".....",
      "#.#.#",
      "..#..",
      "#.#.#"
    },
    {
      "#.###",
      ".....",
      "###.#",
      "..#..",
      "#.#.#"
    },
    {
      "#.#.#",
      "#...#",
      "###.#",
      ".....",
      "#.#.#"
    },
    {
      "###.#",
      ".....",
      "#.#.#",
      "#...#",
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
