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
  RETURNER_STATE,
  REPAIR_STATE,
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
  MINES_4,
  MINES_5,
  CAVES_1,
  CAVES_2,
  CAVES_3,
  CAVES_4,
  CAVES_5,
  LOST_CITY,
  DEEP_DARK_1,
  DEEP_DARK_2,
  DEEP_DARK_3,
  DEEP_DARK_4,
  DEEP_DARK_5,
  ABYSS_1,
  ABYSS_2,
  ABYSS_3,
  ABYSS_4,
  ABYSS_5,
  NETHER_1,
  NETHER_2,
  NETHER_3,
  NETHER_4,
  NETHER_5,
  THE_END
};

enum class MonsterType
{
  HARMLESS = 0,
  RAT,
  BAT,
  SPIDER,
  SKELETON,
  ZOMBIE,
  LICH,
  HUMAN,
  KOBOLD,
  HEROBRINE
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
  MARTIN,
  CASEY,
  MAYA,
  GRISWOLD
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

enum class PlayerSkills
{
  // Use mana to recharge a wand (starting, Arcanist)
  RECHARGE = 0,
  // Use repair kit to repair a weapon (starting, Soldier)
  REPAIR,
  // Show enemy stats (currently by pressing Enter in Look Mode)
  AWARENESS
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
  SHIELD,
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
  GEM,
  RETURNER,
  WEAPON,
  ARMOR,
  WAND,
  REPAIR_KIT
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

enum class ArmorType
{
  PADDING = 0,
  LEATHER,
  MAIL,
  PLATE
};

enum class FoodType
{
  FIRST_ELEMENT = 0,
  APPLE,  
  CHEESE,  
  BREAD,
  FISH,
  PIE,
  MEAT,
  TIN,
  RATIONS,
  IRON_RATIONS,
  LAST_ELEMENT
};

enum class GemType
{
  RANDOM = -1,
  WORTHLESS_GLASS = 0,
  RED_GARNET,
  RED_RUBY,
  ORANGE_JACINTH,
  ORANGE_AMBER,
  YELLOW_CITRINE,
  GREEN_JADE,
  GREEN_EMERALD,
  BLUE_SAPPHIRE,
  BLUE_AQUAMARINE,
  PURPLE_AMETHYST,
  PURPLE_FLUORITE,
  WHITE_OPAL,
  WHITE_DIAMOND,
  BLACK_JETSTONE,
  BLACK_OBSIDIAN
};

enum class SpellType
{
  // 1 target, DEF
  STRIKE = 0,
  // 1 target, RES
  MAGIC_MISSILE,
  // 1 target, RES, slow
  FROST,
  // area damage, RES
  FIREBALL,
  // chains (self-damage possible), RES
  LIGHTNING,
  // pierces through, DEF
  LASER,
  // increase visibility radius, temporary
  LIGHT,
  IDENTIFY,
  MAGIC_MAPPING,
  TELEPORT,
  // shows monsters (not all?), temporary
  DETECT_MONSTERS,
  REMOVE_CURSE,
  HEAL,
  NEUTRALIZE_POISON,
  // nuff said
  MANA_SHIELD
};

enum class WandMaterials
{
  YEW = 0,
  IVORY,
  EBONY,
  ONYX,
  GLASS,
  COPPER,
  GOLDEN
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

enum class RoomEdgeEnum
{
  NORTH = 0,
  EAST,
  SOUTH,
  WEST
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

    /*
    if (res < 0)
    {
      res = 0;
    }
    */

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

  int IgnoreArmorPercentage = 0;

  // Kinda out of place hack
  std::pair<MapType, std::pair<int, int>> ReturnerPosition =
  {
    MapType::NOWHERE, { -1, -1 }
  };

  bool IsUsable = false;
  bool IsEquipped = false;
  bool IsStackable = false;
  bool IsIdentified = false;
  bool IsPrefixDiscovered = false;
  bool IsChargeable = false;

  int Amount = 1;

  // !!! Use GetCost() when cost is needed !!!
  int Cost = 0;

  bool HasDurability()
  {
    return (EqCategory == EquipmentCategory::BOOTS
         || EqCategory == EquipmentCategory::HEAD
         || EqCategory == EquipmentCategory::LEGS
         || EqCategory == EquipmentCategory::NECK
         || EqCategory == EquipmentCategory::TORSO
         || EqCategory == EquipmentCategory::WEAPON);
  }

  int GetCost()
  {
    bool hasDurability = HasDurability();

    int price = 0;

    if (hasDurability)
    {
      price = Durability.CurrentValue;
    }
    else
    {
      price = Cost;
    }

    if (ItemType_ == ItemType::FOOD)
    {
      price *= 0.01f;
    }

    if (ItemType_ == ItemType::GEM)
    {
      price = (IsIdentified) ? Cost : 20;
    }

    if (IsStackable || IsChargeable)
    {
      price *= Amount;
    }

    if (ItemType_ == ItemType::RETURNER && !IsIdentified)
    {
      price = 20;
    }

    if (IsIdentified)
    {
      if (Prefix == ItemPrefix::BLESSED)
      {
        price *= 2;
      }
      else if (Prefix == ItemPrefix::CURSED)
      {
        price *= 0.5f;
      }
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
    { StatsEnum::HP,  0 },
    { StatsEnum::MP,  0 }
  };

  std::map<StatsEnum, int> StatRequirements =
  {
    { StatsEnum::STR, 0 },
    { StatsEnum::DEF, 0 },
    { StatsEnum::MAG, 0 },
    { StatsEnum::RES, 0 },
    { StatsEnum::SKL, 0 },
    { StatsEnum::SPD, 0 },
    { StatsEnum::HP,  0 },
    { StatsEnum::MP,  0 }
  };

  // BUC status + object name [+ suffix]
  // Used in inspection window
  std::string IdentifiedName;

  // ? + object name + ?
  std::string UnidentifiedName;

  std::vector<std::string> UnidentifiedDescription;
  std::vector<std::string> IdentifiedDescription;

  std::function<bool(void*)> UseCallback;
  std::function<void(void*)> EquipCallback;

  size_t ItemTypeHash;
};

namespace GlobalConstants
{
  static const std::string AlphabetUppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static const std::string AlphabetLowercase = "abcdefghijklmnopqrstuvwxyz";
  static const std::string Numbers = "0123456789";

  static const int MaxNameLength = 24;
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

  static const std::string BlackColor = "#000000";
  static const std::string WhiteColor = "#FFFFFF";
  static const std::string MarbleColor = "#FFE0E0";
  static const std::string WallColor = "#888888";
  static const std::string GrassColor = "#006600";
  static const std::string TreeColor = "#00FF00";
  static const std::string PlayerColor = "#00FFFF";
  static const std::string MonsterColor = "#FF0000";
  static const std::string ShallowWaterColor = "#9999FF";
  static const std::string DeepWaterColor = "#3333FF";
  static const std::string NpcColor = "#FFFF00";
  static const std::string GroundColor = "#444444";
  static const std::string IronColor = "#CBCDCD";
  static const std::string RoomFloorColor = "#692E11";
  static const std::string WoodColor = "#DCB579";
  static const std::string ChestColor = "#A0793D";
  static const std::string MountainsColor = "#666666";
  static const std::string FogOfWarColor = "#202020";
  static const std::string CoinsColor = "#FFD700";
  static const std::string DoorHighlightColor = "#666600";
  static const std::string MessageBoxDefaultBgColor = "#222222";
  static const std::string MessageBoxDefaultBorderColor = "#666666";
  static const std::string MessageBoxRedBorderColor = "#880000";
  static const std::string ItemMagicColor = "#4169E1";
  static const std::string ItemRareColor = "#CCCC52";
  static const std::string ItemUniqueColor = "#A59263";

  static const std::map<PlayerSkills, std::string> SkillNameByType =
  {
    { PlayerSkills::REPAIR,    "Repair" },
    { PlayerSkills::RECHARGE,  "Recharge" },
    { PlayerSkills::AWARENESS, "Awareness" },
  };

  static const std::map<TraderRole, std::string> ShopNameByType =
  {
    { TraderRole::BLACKSMITH, "Armory"    },
    { TraderRole::CLERIC,     "Sanctuary" },
    { TraderRole::COOK,       "Grocery"   },
    { TraderRole::JUNKER,     "Junkyard"  }
  };

  // Weighted random
  static const std::map<MonsterType, std::map<ItemType, int>> LootTable =
  {
    {
      MonsterType::RAT,
      {
        { ItemType::COINS,    1 },
        { ItemType::FOOD,     4 },
        { ItemType::NOTHING, 20 }
      }
    },
    {
      MonsterType::BAT,
      {
        { ItemType::COINS,    1 },
        { ItemType::FOOD,     4 },
        { ItemType::NOTHING, 20 }
      }
    },
    {
      MonsterType::SPIDER,
      {
        { ItemType::COINS,    2 },
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
        { FoodType::MEAT,   1 }
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
    { ItemType::MANA_POTION,    50 },
    { ItemType::HUNGER_POTION,  20 },
    { ItemType::STR_POTION,     1  },
    { ItemType::DEF_POTION,     1  },
    { ItemType::MAG_POTION,     1  },
    { ItemType::RES_POTION,     1  },
    { ItemType::SKL_POTION,     1  },
    { ItemType::SPD_POTION,     1  },
    { ItemType::EXP_POTION,     5  }
  };

  static const std::map<ItemType, std::string> PotionNameByType =
  {
    { ItemType::HEALING_POTION, "Healing Potion" },
    { ItemType::MANA_POTION,    "Mana Potion"    },
    { ItemType::HUNGER_POTION,  "Hunger Potion"  },
    { ItemType::STR_POTION,     "STR Potion"     },
    { ItemType::DEF_POTION,     "DEF Potion"     },
    { ItemType::MAG_POTION,     "MAG Potion"     },
    { ItemType::RES_POTION,     "RES Potion"     },
    { ItemType::SKL_POTION,     "SKL Potion"     },
    { ItemType::SPD_POTION,     "SPD Potion"     },
    { ItemType::EXP_POTION,     "EXP Potion"     }
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
    { WeaponType::DAGGER,       "Dagger"       },
    { WeaponType::SHORT_SWORD,  "Short Sword"  },
    { WeaponType::ARMING_SWORD, "Arming Sword" },
    { WeaponType::LONG_SWORD,   "Longsword"    },
    { WeaponType::GREAT_SWORD,  "Great Sword"  },
    { WeaponType::STAFF,        "Battle Staff" }
  };

  static const std::map<ArmorType, std::string> ArmorNameByType =
  {
    { ArmorType::PADDING, "Gambeson"         },
    { ArmorType::LEATHER, "Leather Lammelar" },
    { ArmorType::MAIL,    "Mail Hauberk"     },
    { ArmorType::PLATE,   "Coat of Plates"   }
  };

  static const std::map<ArmorType, int> ArmorDurabilityByType =
  {
    { ArmorType::PADDING, 30 },
    { ArmorType::LEATHER, 60 },
    { ArmorType::MAIL,   120 },
    { ArmorType::PLATE,  240 }
  };

  static const std::map<FoodType, std::pair<std::string, int>> FoodHungerPercentageByName =
  {
    { FoodType::APPLE,        { "Apple",        10  } },
    { FoodType::BREAD,        { "Bread",        20  } },
    { FoodType::FISH,         { "Fish",         20  } },
    { FoodType::CHEESE,       { "Cheese",       30  } },
    { FoodType::PIE,          { "Cream Pie",    40  } },
    { FoodType::MEAT,         { "Meat",         50  } },
    { FoodType::TIN,          { "Canned Food",  60  } },
    { FoodType::RATIONS,      { "Rations",      75  } },
    { FoodType::IRON_RATIONS, { "Iron Rations", 100 } }
  };

  static const std::map<GemType, std::pair<std::string, std::string>> GemColorByType =
  {
    { GemType::BLACK_JETSTONE,  { "#FFFFFF", "#000000" } },
    { GemType::BLACK_OBSIDIAN,  { "#FFFFFF", "#000000" } },
    { GemType::BLUE_AQUAMARINE, { "#FFFFFF", "#0000FF" } },
    { GemType::BLUE_SAPPHIRE,   { "#FFFFFF", "#0000FF" } },
    { GemType::GREEN_EMERALD,   { "#FFFFFF", "#008800" } },
    { GemType::GREEN_JADE,      { "#FFFFFF", "#008800" } },
    { GemType::ORANGE_AMBER,    { "#000000", "#FF9900" } },
    { GemType::ORANGE_JACINTH,  { "#000000", "#FF9900" } },
    { GemType::PURPLE_AMETHYST, { "#FFFFFF", "#800080" } },
    { GemType::PURPLE_FLUORITE, { "#FFFFFF", "#800080" } },
    { GemType::RED_GARNET,      { "#FFFFFF", "#AA0000" } },
    { GemType::RED_RUBY,        { "#FFFFFF", "#AA0000" } },
    { GemType::WHITE_DIAMOND,   { "#000000", "#FFFFFF" } },
    { GemType::WHITE_OPAL,      { "#000000", "#FFFFFF" } },
    { GemType::YELLOW_CITRINE,  { "#000000", "#FFFF00" } }
  };

  static const std::map<GemType, std::string> GemColorNameByType =
  {
    { GemType::BLACK_JETSTONE,  "Black"  },
    { GemType::BLACK_OBSIDIAN,  "Black"  },
    { GemType::BLUE_AQUAMARINE, "Blue"   },
    { GemType::BLUE_SAPPHIRE,   "Blue"   },
    { GemType::GREEN_EMERALD,   "Green"  },
    { GemType::GREEN_JADE,      "Green"  },
    { GemType::ORANGE_AMBER,    "Orange" },
    { GemType::ORANGE_JACINTH,  "Orange" },
    { GemType::PURPLE_AMETHYST, "Purple" },
    { GemType::PURPLE_FLUORITE, "Purple" },
    { GemType::RED_GARNET,      "Red"    },
    { GemType::RED_RUBY,        "Red"    },
    { GemType::WHITE_DIAMOND,   "White"  },
    { GemType::WHITE_OPAL,      "White"  },
    { GemType::YELLOW_CITRINE,  "Yellow" }
  };

  static const std::map<GemType, std::string> GemNameByType =
  {
    { GemType::WORTHLESS_GLASS, "Worthless Glass" },
    { GemType::BLACK_JETSTONE,  "Jetstone"        },
    { GemType::BLACK_OBSIDIAN,  "Obsidian"        },
    { GemType::BLUE_AQUAMARINE, "Aquamarine"      },
    { GemType::BLUE_SAPPHIRE,   "Sapphire"        },
    { GemType::GREEN_EMERALD,   "Emerald"         },
    { GemType::GREEN_JADE,      "Jade"            },
    { GemType::ORANGE_AMBER,    "Amber"           },
    { GemType::ORANGE_JACINTH,  "Jacinth"         },
    { GemType::PURPLE_AMETHYST, "Amethyst"        },
    { GemType::PURPLE_FLUORITE, "Fluorite"        },
    { GemType::RED_GARNET,      "Garnet"          },
    { GemType::RED_RUBY,        "Ruby"            },
    { GemType::WHITE_DIAMOND,   "Diamond"         },
    { GemType::WHITE_OPAL,      "Opal"            },
    { GemType::YELLOW_CITRINE,  "Citrine"         }
  };

  static const std::map<GemType, int> GemCostByType =
  {
    // 0
    { GemType::WORTHLESS_GLASS, 0   },
    // 200
    { GemType::BLACK_OBSIDIAN,  20  },
    // 300
    { GemType::GREEN_JADE,      30  },
    // 400
    { GemType::PURPLE_FLUORITE, 40  },
    // 600
    { GemType::PURPLE_AMETHYST, 60  },
    // 700
    { GemType::RED_GARNET,      70  },
    // 800
    { GemType::WHITE_OPAL,      80  },
    // 850
    { GemType::BLACK_JETSTONE,  85  },
    // 1000
    { GemType::ORANGE_AMBER,    100 },
    // 1500
    { GemType::YELLOW_CITRINE,  150 },
    // 1500
    { GemType::BLUE_AQUAMARINE, 150 },
    // 2500
    { GemType::GREEN_EMERALD,   250 },
    // 3000
    { GemType::BLUE_SAPPHIRE,   300 },
    // 3250
    { GemType::ORANGE_JACINTH,  325 },
    // 3500
    { GemType::RED_RUBY,        350 },
    // 4000
    { GemType::WHITE_DIAMOND,   400 }
  };

  static const std::map<WandMaterials, int> WandCapacityByMaterial =
  {
    { WandMaterials::YEW,    100 },
    { WandMaterials::IVORY,  150 },
    { WandMaterials::EBONY,  200 },
    { WandMaterials::ONYX,   250 },
    { WandMaterials::GLASS,  400 },
    { WandMaterials::COPPER, 600 },
    { WandMaterials::GOLDEN, 800 }
  };

  // Divide wand capacity by this value to get amount of charges,
  // then randomize if needed.
  static const std::map<SpellType, int> WandSpellCapacityCostByType =
  {
    { SpellType::STRIKE,        25  },
    { SpellType::FROST,         50  },
    { SpellType::FIREBALL,      100 },
    { SpellType::LASER,         150 },
    { SpellType::LIGHTNING,     100 },
    { SpellType::MAGIC_MISSILE, 25  }
  };

  static const std::map<ShrineType, std::string> ShrineSaintByType =
  {
    { ShrineType::MIGHT,      "St. George the Soldier"     },
    { ShrineType::SPIRIT,     "St. Mary the Mother"        },
    { ShrineType::KNOWLEDGE,  "St. Nestor the Scribe"      },
    { ShrineType::PERCEPTION, "St. Justin the Philosopher" },
    { ShrineType::HEALING,    "St. Luke the Healer"        }
  };

  static const std::map<ShrineType, std::string> ShrineNameByType =
  {
    { ShrineType::MIGHT,        "Shrine of Might"       },
    { ShrineType::SPIRIT,       "Shrine of Spirit"      },
    { ShrineType::TRANQUILITY,  "Shrine of Tranquility" },
    { ShrineType::KNOWLEDGE,    "Shrine of Knowledge"   },
    { ShrineType::PERCEPTION,   "Shrine of Perception"  },
    { ShrineType::HEALING,      "Shrine of Healing"     },
    { ShrineType::FORGOTTEN,    "Forgotten Shrine"      },
    { ShrineType::ABYSSAL,      "Abyssal Shrine"        },
    { ShrineType::DESECRATED,   "Desecrated Shrine"     },
    { ShrineType::DISTURBING,   "Disturbing Shrine"     },
    { ShrineType::RUINED,       "Ruined Shrine"         },
    { ShrineType::POTENTIAL,    "Shrine of Potential"   },
    { ShrineType::HIDDEN,       "Hidden Shrine"         },
    { ShrineType::HOLY,         "Holy Shrine"           }
  };

  static const std::map<ShrineType, std::pair<std::string, std::string>> ShrineColorsByType =
  {
    { ShrineType::MIGHT,       { "#FF0000", "#888888" } },
    { ShrineType::SPIRIT,      { "#0088FF", "#888888" } },
    { ShrineType::TRANQUILITY, { "#0088FF", "#888888" } },
    { ShrineType::KNOWLEDGE,   { "#44FF44", "#888888" } },
    { ShrineType::PERCEPTION,  { "#FFFFFF", "#888888" } },
    { ShrineType::HEALING,     { "#FF0000", "#888888" } },
    { ShrineType::FORGOTTEN,   { "#FFFFFF", "#888888" } },
    { ShrineType::ABYSSAL,     { "#FF8000", "#880000" } },
    { ShrineType::DESECRATED,  { "#888800", "#440000" } },
    { ShrineType::DISTURBING,  { "#660000", "#888888" } },
    { ShrineType::RUINED,      { "#666666", "#000000" } },
    { ShrineType::POTENTIAL,   { "#FF0000", "#888888" } },
    { ShrineType::HIDDEN,      { "#666666", "#000000" } },
    { ShrineType::HOLY,        { "#FFFF00", "#888888" } },
  };

  static const std::map<std::string, std::vector<std::string>> PotionColorsByName =
  {
    { "Red Potion",     { "#FFFFFF", "#FF0000" } },
    { "Green Potion",   { "#FFFFFF", "#00FF00" } },
    { "Radiant Potion", { "#666666", "#FFFF88" } },
    { "Morbid Potion",  { "#FFFFFF", "#660000" } },
    { "Blue Potion",    { "#FFFFFF", "#0000FF" } },
    { "Yellow Potion",  { "#000000", "#FFFF00" } },
    { "Clear Potion",   { "#000000", "#CCCCCC" } },
    { "Black Potion",   { "#FFFFFF", "#000000" } },
    { "Cyan Potion",    { "#FFFFFF", "#00FFFF" } },
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
    { MapType::MINES_3,     { "Abandoned Mines" } },
    { MapType::MINES_4,     { "Deep Mines" } },
    { MapType::MINES_5,     { "Crumbling Tunnels" } },
    // Caves of Circe
    { MapType::CAVES_1,     { "Catacombs" } },
    { MapType::CAVES_2,     { "Caves of Circe" } },
    { MapType::CAVES_3,     { "Corridors of Time" } },
    { MapType::CAVES_4,     { "Windy Tunnels" } },
    { MapType::CAVES_5,     { "Vertigo" } },
    // Lost City
    { MapType::LOST_CITY,   { "Lost City" } },
    // Deep Dark
    { MapType::DEEP_DARK_1, { "Embrace of Darkness" } },
    { MapType::DEEP_DARK_2, { "Underdark" } },
    { MapType::DEEP_DARK_3, { "Deep Dark" } },
    { MapType::DEEP_DARK_4, { "Starless Night" } },
    { MapType::DEEP_DARK_5, { "Sinister" } },
    // Stygian Abyss
    { MapType::ABYSS_1,     { "Plains of Desolation" } },
    { MapType::ABYSS_2,     { "Grey Wastes" } },
    { MapType::ABYSS_3,     { "Soul Pastures" } },
    { MapType::ABYSS_4,     { "Stygian Abyss" } },
    { MapType::ABYSS_5,     { "Hell's Maw" } },
    // Nether
    { MapType::NETHER_1,    { "Blazing Gates" } },
    { MapType::NETHER_2,    { "River of Fire" } },
    { MapType::NETHER_3,    { "Red Wastes" } },
    { MapType::NETHER_4,    { "Citadel" } },
    { MapType::NETHER_5,    { "The Hearth" } },
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
    // Might, Spirit, Knowledge, Holy, Healing, Potential, Tranquility, Perception
    {
      "#####",
      "#...#",
      "+./.#",
      "#...#",
      "#####"
    },
    // 1
    // Holy, Healing, Potential, Tranquility, Perception
    {
      "#g g#",
      "gg gg",
      "  /  ",
      "gg gg",
      "#g g#"
    },
    // 2
    // Holy, Healing, Potential, Tranquility, Perception
    {
      " www ",
      "ww ww",
      "w / w",
      "ww ww",
      " w w "
    },
    // 3
    // Forgotten, Hidden, Ruined, Desecrated, Disturbing
    {
      "#.#..",
      ".  .#",
      "+./.#",
      "#.  .",
      "..###"
    },
    // 4
    // Abyssal
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
    }
  };
}

#endif
