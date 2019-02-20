#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifdef USE_SDL

  #include <SDL2/SDL_keycode.h>

  #define NUMPAD_7  SDLK_KP_7
  #define NUMPAD_8  SDLK_KP_8
  #define NUMPAD_9  SDLK_KP_9
  #define NUMPAD_4  SDLK_KP_4
  #define NUMPAD_5  SDLK_KP_5
  #define NUMPAD_6  SDLK_KP_6
  #define NUMPAD_1  SDLK_KP_1
  #define NUMPAD_2  SDLK_KP_2
  #define NUMPAD_3  SDLK_KP_3

  // ********************

  #define VK_BACKSPACE SDLK_BACKSPACE
  #define VK_ENTER     SDLK_RETURN

  #define KEY_DOWN     SDLK_DOWN
  #define KEY_UP       SDLK_UP

#else

#include <ncurses.h>

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
#endif

#include <vector>
#include <string>
#include <map>
#include <functional>

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

/// Helper struct to reduce the amount of writing when creating objects
struct Tile
{
  void Set(bool isBlocking,
           bool blocksSight,
           int image,
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
  int Image;

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

  /// Returns value including modifier
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

enum class NameCP437
{
  FIRST = 0,
  FACE_1,
  FACE_2,
  HEART,
  DIAMOND,
  CLUB,
  SPADE,
  DOT_1,
  DOT_2,
  SQUARE_1,
  SQUARE_2,
  SIGN_M,
  SIGN_F,
  NOTE_1,
  NOTE_2,
  LAMP,
  RARROW_2,
  LARROW_2,
  UDARROW_1,
  EXCLAIM_DBL,
  PI,
  DOLLAR_1,
  DASH_1,
  UDARROW_2,
  UARROW_1,
  DARROW_1,
  RARROW_1,
  LARROW_1,
  CORNER_SMALL,
  LRARROW,
  UARROW_2,
  DARROW_2,
  ULCORNER_1 = 218,
  DLCORNER_1 = 192,
  URCORNER_1 = 191,
  DRCORNER_1 = 217,
  HBAR_1 = 196,
  VBAR_1 = 179,
  BLOCK = 219,
  WAVES = 247,
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
  static std::map<NameCP437, int> CP437IndexByType =
  {
    { NameCP437::FACE_1,       (int)NameCP437::FACE_1       },
    { NameCP437::FACE_2,       (int)NameCP437::FACE_2       },
    { NameCP437::HEART,        (int)NameCP437::HEART        },
    { NameCP437::DIAMOND,      (int)NameCP437::DIAMOND      },
    { NameCP437::CLUB,         (int)NameCP437::CLUB         },
    { NameCP437::SPADE,        (int)NameCP437::SPADE        },
    { NameCP437::DOT_1,        (int)NameCP437::DOT_1        },
    { NameCP437::DOT_2,        (int)NameCP437::DOT_2        },
    { NameCP437::SQUARE_1,     (int)NameCP437::SQUARE_1     },
    { NameCP437::SQUARE_2,     (int)NameCP437::SQUARE_2     },
    { NameCP437::SIGN_M,       (int)NameCP437::SIGN_M       },
    { NameCP437::SIGN_F,       (int)NameCP437::SIGN_F       },
    { NameCP437::NOTE_1,       (int)NameCP437::NOTE_1       },
    { NameCP437::NOTE_2,       (int)NameCP437::NOTE_2       },
    { NameCP437::LAMP,         (int)NameCP437::LAMP         },
    { NameCP437::RARROW_2,     (int)NameCP437::RARROW_2     },
    { NameCP437::LARROW_2,     (int)NameCP437::LARROW_2     },
    { NameCP437::UDARROW_1,    (int)NameCP437::UDARROW_1    },
    { NameCP437::EXCLAIM_DBL,  (int)NameCP437::EXCLAIM_DBL  },
    { NameCP437::PI,           (int)NameCP437::PI           },
    { NameCP437::DOLLAR_1,     (int)NameCP437::DOLLAR_1     },
    { NameCP437::DASH_1,       (int)NameCP437::DASH_1       },
    { NameCP437::UDARROW_2,    (int)NameCP437::UDARROW_2    },
    { NameCP437::UARROW_1,     (int)NameCP437::UARROW_1     },
    { NameCP437::DARROW_1,     (int)NameCP437::DARROW_1     },
    { NameCP437::RARROW_1,     (int)NameCP437::RARROW_1     },
    { NameCP437::LARROW_1,     (int)NameCP437::LARROW_1     },
    { NameCP437::CORNER_SMALL, (int)NameCP437::CORNER_SMALL },
    { NameCP437::LRARROW,      (int)NameCP437::LRARROW      },
    { NameCP437::UARROW_2,     (int)NameCP437::UARROW_2     },
    { NameCP437::DARROW_2,     (int)NameCP437::DARROW_2     },
    { NameCP437::BLOCK,        (int)NameCP437::BLOCK        },
    { NameCP437::WAVES,        (int)NameCP437::WAVES        },
    { NameCP437::URCORNER_1,   (int)NameCP437::URCORNER_1   },
    { NameCP437::ULCORNER_1,   (int)NameCP437::ULCORNER_1   },
    { NameCP437::DRCORNER_1,   (int)NameCP437::DRCORNER_1   },
    { NameCP437::DLCORNER_1,   (int)NameCP437::DLCORNER_1   },
    { NameCP437::HBAR_1,       (int)NameCP437::HBAR_1       },
    { NameCP437::VBAR_1,       (int)NameCP437::VBAR_1       }
  };

  static const std::string Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  static const std::string Tileset8x16Base64 = "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";
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
  static const int TurnReadyValue = 1000;
  static const int TurnTickValue = 100;

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
