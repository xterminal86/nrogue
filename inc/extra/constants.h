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
#include <ncurses.h>

enum class GameStates
{
  EXIT_GAME = -1,
  ATTACK_STATE,
  MENU_STATE,
  MAIN_STATE,
  SELECT_CLASS_STATE,
  ENTER_NAME_STATE,
  INTRO_STATE,
  INFO_STATE,
  INVENTORY_STATE,
  CONTAINER_INTERACT_STATE,
  SHOW_MESSAGES_STATE,
  LOOK_INPUT_STATE,
  INTERACT_INPUT_STATE,
  NPC_INTERACT_STATE,
  SHOPPING_STATE,
  EXITING_STATE,
  MESSAGE_BOX_STATE,
  ENDGAME_STATE
};

enum class MessageBoxType
{
  ANY_KEY = 0,
  WAIT_FOR_INPUT,
  IGNORE_INPUT
};

enum class MapType
{
  NOWHERE = -1,
  TOWN,
  MINES_1,
  MINES_2,
  MINES_3,
  CAVES_1,
  CAVES_2,
  CAVES_3,
  LOST_CITY,
  DEEP_DARK_1,
  DEEP_DARK_2,
  DEEP_DARK_3,
  ABYSS_1,
  ABYSS_2,
  ABYSS_3,
  NETHER_1,
  NETHER_2,
  NETHER_3,
  THE_END
};

enum class MonsterType
{
  HARMLESS = 0,
  RAT,
  BAT,
  SPIDER,
  SKELETON
};

enum class NPCType
{
  CLAIRE = 0,
  CLOUD,
  PHOENIX,
  MILES,
  IARSPIDER,
  TIGRA,
  STEVE,
  GIMLEY,
  MARTIN
};

enum class TraderRole
{
  NONE,
  COOK,
  BLACKSMITH,
  CLERIC,
  JUNKER
};

enum class PlayerClass
{
  SOLDIER = 0,
  THIEF,
  ARCANIST,
  CUSTOM
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
  NOTHING = 0,
  DUMMY,
  COINS,  
  HEALING_POTION,
  MANA_POTION,
  HUNGER_POTION,
  STR_POTION,
  DEF_POTION,
  MAG_POTION,
  RES_POTION,
  SKL_POTION,
  SPD_POTION,
  EXP_POTION,
  FOOD,
  POTION,
  SCROLL,
  WEAPON
};

enum class WeaponType
{
  DAGGER = 0,
  SHORT_SWORD,
  ARMING_SWORD,
  LONG_SWORD,
  GREAT_SWORD,
  STAFF
};

enum class FoodType
{
  FIRST_ELEMENT = 0,
  APPLE,
  CHEESE,
  BREAD,
  MEAT,
  RATIONS,
  IRON_RATIONS,
  LAST_ELEMENT
};

enum class ShrineType
{
  MIGHT = 0,    // raises STR or HP
  SPIRIT,       // raises MAG or MP
  TRANQUILITY,  // restores MP
  KNOWLEDGE,    // identifies items
  PERCEPTION,   // reveals BUC status
  HEALING,      // restores HP
  DESECRATED,   // random effect + receive curse
  RUINED,       // random effect + low chance to receive negative effect
  DISTURBING,   // random effect + high chance to receive negative effect
  ABYSSAL,      // random stat raise + negative effect
  FORGOTTEN,    // random effect
  POTENTIAL,    // temporary raises stats
  HIDDEN,       // random effect
  HOLY          // removes curse
};

enum class RoomLayoutRotation
{
  NONE = 0,
  CCW_90,
  CCW_180,
  CCW_270
};

/// Helper struct to reduce the writing when creating objects
struct Tile
{
  void Set(bool isBlocking,
           bool blocksSight,
           chtype image,
           const std::string& fgColor,
           const std::string& bgColor,
           const std::string& objectName,
           const std::string& fowName = "")
  {
    IsBlocking = isBlocking;
    BlocksSight = blocksSight;
    Image = image;
    FgColor = fgColor;
    BgColor = bgColor;
    ObjectName = objectName;
    FogOfWarName = fowName;
  }

  bool IsBlocking;
  bool BlocksSight;
  chtype Image;

  std::string FgColor;
  std::string BgColor;

  std::string ObjectName;
  std::string FogOfWarName;
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

  void Add(int value)
  {
    CurrentValue += value;

    // Cannot use util.h here :-(

    if (CurrentValue > OriginalValue)
    {
      CurrentValue = OriginalValue;
    }

    if (CurrentValue < 0)
    {
      CurrentValue = 0;
    }
  }

  int Get(bool originalValue = true)
  {
    int res = originalValue ? OriginalValue + Modifier : CurrentValue + Modifier;
    if (res < 0)
    {
      res = 0;
    }

    return res;
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

  int Hunger = 0;
  int ActionMeter = 0;

  bool Indestructible = true;

  int Rating()
  {
    int rating = 0;

    rating += Str.OriginalValue;
    rating += Def.OriginalValue;
    rating += Mag.OriginalValue;
    rating += Res.OriginalValue;
    rating += Skl.OriginalValue;
    rating += Spd.OriginalValue;

    return rating;
  }
};

enum class ItemPrefix
{
  RANDOM = 0,
  UNCURSED,
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
  ItemType ItemType_ = ItemType::DUMMY;
  ItemPrefix Prefix = ItemPrefix::UNCURSED;
  ItemRarity Rarity = ItemRarity::COMMON;

  EquipmentCategory EqCategory = EquipmentCategory::NOT_EQUIPPABLE;

  Attribute Durability;
  Attribute Damage;

  bool IsUsable = false;
  bool IsEquipped = false;
  bool IsStackable = false;
  bool IsIdentified = false;
  bool IsPrefixDiscovered = false;

  int Amount = 1;

  // !!! Use GetCost() when cost is needed !!!
  int Cost = 0;

  int GetCost()
  {
    bool hasDurability = (EqCategory == EquipmentCategory::BOOTS
                       || EqCategory == EquipmentCategory::HEAD
                       || EqCategory == EquipmentCategory::LEGS
                       || EqCategory == EquipmentCategory::NECK
                       || EqCategory == EquipmentCategory::TORSO
                       || EqCategory == EquipmentCategory::WEAPON);

    int price = 0;

    if (hasDurability)
    {
      price = Durability.CurrentValue;
    }
    else
    {
      price = Cost;
    }

    if (Prefix == ItemPrefix::BLESSED)
    {
      price *= 2;
    }
    else if (Prefix == ItemPrefix::CURSED)
    {
      price /= 2;
    }

    if (price <= 0)
    {
      price = 1;
    }

    return price;
  }

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

  static const int HPMPBarLength = 20;
  static const int AttributeMinimumRaiseChance = 15;
  static const int AttributeIncreasedRaiseStep = 25;
  static const int AwardedExperienceStep = 5;
  static const int AwardedExpDefault = 20;
  static const int AwardedExpMax = 40;
  static const int MinHitChance = 5;
  static const int MaxHitChance = 95;
  static const int DisplayAttackDelayMs = 50;
  static const int ItemSpawnMinArea = 25 * 25;
  static const int InventoryMaxNameLength = 20;
  static const int InventoryMaxSize = 20;
  static const int WeaponDurabilityLostChance = 20;

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
  static const std::string MessageBoxDefaultBgColor = "#222222";
  static const std::string MessageBoxDefaultBorderColor = "#666666";
  static const std::string MessageBoxRedBorderColor = "#880000";
  static const std::string ItemMagicColor = "#4169E1";
  static const std::string ItemRareColor = "#CCCC52";
  static const std::string ItemUniqueColor = "#A59263";

  static const std::map<TraderRole, std::string> ShopNameByType =
  {
    { TraderRole::BLACKSMITH, "Armory" },
    { TraderRole::CLERIC, "Sanctuary" },
    { TraderRole::COOK, "Grocery" },
    { TraderRole::JUNKER, "Junkyard" }
  };

  // Weighted random
  static const std::map<MonsterType, std::map<ItemType, int>> LootTable =
  {
    {
      MonsterType::RAT,
      {
        { ItemType::COINS, 1 },
        { ItemType::FOOD, 4 },
        { ItemType::NOTHING, 20 }
      }
    },
    {
      MonsterType::BAT,
      {
        { ItemType::COINS, 1 },
        { ItemType::FOOD, 4 },
        { ItemType::NOTHING, 20 }
      }
    },
    {
      MonsterType::SPIDER,
      {
        { ItemType::COINS, 2 },
        { ItemType::NOTHING, 20 }
      }
    }
  };

  static const std::map<MonsterType, std::map<FoodType, int>> FoodLootTable =
  {
    {
      MonsterType::RAT,
      {
        { FoodType::CHEESE, 4 },
        { FoodType::MEAT, 1 }
      }
    },
    {
      MonsterType::BAT,
      {
        { FoodType::MEAT, 1 }
      }
    }
  };

  static const std::map<ItemType, int> PotionsWeightTable =
  {
    { ItemType::HEALING_POTION, 50 },
    { ItemType::MANA_POTION, 50 },
    { ItemType::HUNGER_POTION, 20 },
    { ItemType::STR_POTION, 5 },
    { ItemType::DEF_POTION, 5 },
    { ItemType::MAG_POTION, 5 },
    { ItemType::RES_POTION, 5 },
    { ItemType::SKL_POTION, 5 },
    { ItemType::SPD_POTION, 5 },
    { ItemType::EXP_POTION, 10 }
  };

  static const std::map<ItemType, std::string> StatNameByPotionType =
  {
    { ItemType::STR_POTION, "STR" },
    { ItemType::DEF_POTION, "DEF" },
    { ItemType::MAG_POTION, "MAG" },
    { ItemType::RES_POTION, "RES" },
    { ItemType::SKL_POTION, "SKL" },
    { ItemType::SPD_POTION, "SPD" },
  };

  static const std::map<std::string, ItemType> PotionTypeByStatName =
  {
    { "STR", ItemType::STR_POTION },
    { "DEF", ItemType::DEF_POTION },
    { "MAG", ItemType::MAG_POTION },
    { "RES", ItemType::RES_POTION },
    { "SKL", ItemType::SKL_POTION },
    { "SPD", ItemType::SPD_POTION }
  };

  static const std::map<WeaponType, std::string> WeaponNameByType =
  {
    { WeaponType::DAGGER, "Dagger" },
    { WeaponType::SHORT_SWORD, "Short Sword" },
    { WeaponType::ARMING_SWORD, "Arming Sword" },
    { WeaponType::LONG_SWORD, "Longsword" },
    { WeaponType::GREAT_SWORD, "Great Sword" },
    { WeaponType::STAFF, "Battle Staff" }
  };

  static const std::map<FoodType, std::pair<std::string, int>> FoodHungerPercentageByName =
  {
    { FoodType::APPLE, { "Apple", 15 } },
    { FoodType::BREAD, { "Bread", 30 } },
    { FoodType::CHEESE, { "Cheese", 40 } },
    { FoodType::MEAT, { "Meat", 50 } },
    { FoodType::RATIONS, { "Rations", 75 } },
    { FoodType::IRON_RATIONS, { "Iron Rations", 100 } }
  };

  static const std::map<ShrineType, std::string> ShrineSaintByType =
  {
    { ShrineType::MIGHT, "St. George the Soldier" },
    { ShrineType::SPIRIT, "St. Mary the Mother" },
    { ShrineType::KNOWLEDGE, "St. Nestor the Scribe" },
    { ShrineType::PERCEPTION, "St. Justin the Philosopher" },
    { ShrineType::HEALING, "St. Luke the Healer" }
  };

  static const std::map<ShrineType, std::string> ShrineNameByType =
  {
    { ShrineType::MIGHT, "Shrine of Might" },
    { ShrineType::SPIRIT, "Shrine of Spirit" },
    { ShrineType::TRANQUILITY, "Shrine of Tranquility" },
    { ShrineType::KNOWLEDGE, "Shrine of Knowledge" },
    { ShrineType::PERCEPTION, "Shrine of Perception" },
    { ShrineType::HEALING, "Shrine of Healing" },
    { ShrineType::FORGOTTEN, "Forgotten Shrine" },
    { ShrineType::ABYSSAL, "Abyssal Shrine" },
    { ShrineType::DESECRATED, "Desecrated Shrine" },
    { ShrineType::DISTURBING, "Disturbing Shrine" },
    { ShrineType::RUINED, "Ruined Shrine" },
    { ShrineType::POTENTIAL, "Shrine of Potential" },
    { ShrineType::HIDDEN, "Hidden Shrine" },
    { ShrineType::HOLY, "Holy Shrine" }
  };

  static const std::map<ShrineType, std::pair<std::string, std::string>> ShrineColorsByType =
  {
    { ShrineType::MIGHT, { "#FF0000", "#888888" } },
    { ShrineType::SPIRIT, { "#0088FF", "#888888" } },
    { ShrineType::TRANQUILITY, { "#0088FF", "#888888" } },
    { ShrineType::KNOWLEDGE, { "#44FF44", "#888888" } },
    { ShrineType::PERCEPTION, { "#FFFFFF", "#888888" } },
    { ShrineType::HEALING, { "#FF0000", "#888888" } },
    { ShrineType::FORGOTTEN, { "#FFFFFF", "#888888" } },
    { ShrineType::ABYSSAL, { "#FF8000", "#880000" } },
    { ShrineType::DESECRATED, { "#888800", "#440000" } },
    { ShrineType::DISTURBING, { "#660000", "#888888" } },
    { ShrineType::RUINED, { "#666666", "#000000" } },
    { ShrineType::POTENTIAL, { "#FF0000", "#888888" } },
    { ShrineType::HIDDEN, { "#666666", "#000000" } },
    { ShrineType::HOLY, { "#FFFF00", "#888888" } },
  };

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

  static const std::map<MapType, std::vector<std::string>> MapLevelNames =
  {
    { MapType::TOWN,    { "Village of Darwin",
                          "Town of Tristram",
                          "Outpost of Fargoal",
                          "Resort of Protvino",
                          "Commune of Minetown",
                          "Settlement of Punchtree" } },

    // Abandoned Mines
    { MapType::MINES_1,     { "Mine Entrance" } },
    { MapType::MINES_2,     { "Forsaken Prospects" } },
    { MapType::MINES_3,     { "Deep Mines" } },
    // Caves of Circe
    { MapType::CAVES_1,     { "Caves of Circe" } },
    { MapType::CAVES_2,     { "Corridors of Time" } },
    { MapType::CAVES_3,     { "Windy Tunnels" } },
    // Lost City
    { MapType::LOST_CITY,   { "Lost City" } },
    // Deep Dark
    { MapType::DEEP_DARK_1, { "Embrace of Darkness" } },
    { MapType::DEEP_DARK_2, { "Deep Dark" } },
    { MapType::DEEP_DARK_3, { "Starless Night" } },
    // Stygian Abyss
    { MapType::ABYSS_1,     { "Plains of Desolation" } },
    { MapType::ABYSS_2,     { "Soul Pastures" } },
    { MapType::ABYSS_3,     { "Stygian Abyss" } },
    // Nether
    { MapType::NETHER_1,    { "Blazing Gates" } },
    { MapType::NETHER_2,    { "River of Flame" } },
    { MapType::NETHER_3,    { "The Hearth" } },
    // The End
    { MapType::THE_END,     { "???" } }
  };

  static const std::vector<std::vector<std::string>> DungeonRooms =
  {
    // 0
    {
      "##.##",
      "#...#",
      "..#..",
      "#...#",
      "##.##"
    },
    // 1
    {
      "##.##",
      "#...#",
      "..#..",
      "#...#",
      "#####"
    },
    // 2
    {
      "#.#.#",
      ".....",
      "#.#.#",
      ".....",
      "#.#.#"
    },
    // 3
    {
      "###.#",
      ".....",
      "#.#.#",
      "..#..",
      "#.#.#"
    },
    // 4
    {
      "#.###",
      ".....",
      "###.#",
      "..#..",
      "#.#.#"
    },
    // 5
    {
      "#.#.#",
      "#...#",
      "###.#",
      ".....",
      "#.#.#"
    },
    // 6
    {
      "###.#",
      ".....",
      "#.#.#",
      "#...#",
      "#.#.#"
    },
    // 7
    {
      "#####",
      "....#",
      "###.#",
      "..#.#",
      "..#.#"
    },
    // 8
    {
      "#####",
      "#...#",
      "#.#.#",
      "#.#.#",
      "#.#.#"
    },
    // 9
    {
      "#####",
      "#####",
      ".....",
      "#####",
      "#####"
    },
    // 10
    {
      "#.###",
      "#.#..",
      "#.#.#",
      "#...#",
      "#####"
    },
    // 11
    {
      "##.##",
      "#...#",
      "#.#.#",
      ".....",
      "##.##"
    },
    // 12
    {
      ".....",
      ".###.",
      ".###.",
      ".###.",
      "....."
    },
    // 13
    {
      ".....",
      "..#..",
      ".###.",
      "..#..",
      "....."
    },
    // 14
    {
      ".....",
      ".....",
      ".....",
      ".....",
      "....."
    }
  };

  static const std::vector<std::vector<std::string>> ShrineLayouts =
  {
    // 0
    {
      "#####",
      "#...#",
      "+./.#",
      "#...#",
      "#####"
    },
    // 1
    {
      "#g g#",
      "gg gg",
      "  /  ",
      "gg gg",
      "#g g#"
    },
    // 2
    {
      " www ",
      "ww ww",
      "w / w",
      "ww ww",
      " w w "
    },
    // 3
    {
      "#.#..",
      ".  .#",
      "+./.#",
      "#.  .",
      "..###"
    },
    // 4
    {
      "lllll",
      "ll ll",
      "l / l",
      "ll ll",
      "ll ll"
    }
  };

  static const std::vector<std::vector<std::string>> SpecialRooms =
  {
    // Common houses
    // 0
    {
      "##+##",
      "#...#",
      "#...#",
      "#...#",
      "##-##"
    },
    // 1
    {
      "#+#####",
      "#..#..#",
      "|..+..|",
      "#..#..#",
      "#######"
    },
    // 2
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
    // 3
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
    // 4
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
    // 5
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
    // 6
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
    // 7
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
    // 8
    {
      "#########",
      "#.......#",
      "+.......#",
      "#.......#",
      "#########"
    }
  };
}

#endif
