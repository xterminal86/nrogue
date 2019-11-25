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
  #define VK_TAB       SDLK_TAB

  #define KEY_DOWN     SDLK_DOWN
  #define KEY_UP       SDLK_UP  

#else

#include <ncurses.h>

  // 262, 259, 339, 260, 350, 261, 360, 258, 338

  #if defined(__unix__) || defined(__linux__)

  // NOTE: probably will work in xterm only

    #if NCURSES_VERSION_MAJOR > 5
      #define NUMPAD_7  262
      #define NUMPAD_8  259
      #define NUMPAD_9  339
      #define NUMPAD_4  260
      #define NUMPAD_5  350
      #define NUMPAD_6  261
      #define NUMPAD_1  360
      #define NUMPAD_2  258
      #define NUMPAD_3  338

      #define VK_BACKSPACE 127
    #else
      #define NUMPAD_7  KEY_HOME
      #define NUMPAD_8  KEY_UP
      #define NUMPAD_9  KEY_PPAGE
      #define NUMPAD_4  KEY_LEFT
      #define NUMPAD_5  KEY_B2
      #define NUMPAD_6  KEY_RIGHT
      #define NUMPAD_1  KEY_END
      #define NUMPAD_2  KEY_DOWN
      #define NUMPAD_3  KEY_NPAGE

      #define VK_BACKSPACE KEY_BACKSPACE
    #endif

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

  #endif

  #define VK_ENTER     10
  #define VK_TAB       '\t'

#endif

#include <vector>
#include <string>
#include <map>
#include <functional>

using StringsArray2D = std::vector<std::string>;
using CharArray2D = std::vector<std::vector<char>>;

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
  TARGET_STATE,
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

enum class GameObjectType
{
  HARMLESS = 0,
  PLAYER,
  GROUND,
  SHALLOW_WATER,
  DEEP_WATER,
  LAVA,
  PICKAXEABLE,
  TRAP,
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
  RECHARGE = 0, // Use mana to recharge a wand (starting for Arcanist)
  REPAIR,       // Use repair kit to repair a weapon (starting for Soldier)
  SPELLCASTING, // Allows character to cast spells (starting for Arcanist)
  AWARENESS     // Autodetect traps
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

// TODO: loot from kills uses GameObjectsFactory::CreateGameObject()
// which in turn does switch case on this enum.
// Don't forget to add other cases when finished
// with deciding loot tables for monsters.
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
  ACCESSORY,
  WAND,
  SPELLBOOK,
  REPAIR_KIT,
  RANGED_WEAPON,  
  ARROWS
};

enum class ItemQuality
{
  RANDOM = -3,
  DAMAGED,
  FLAWED,
  NORMAL,
  FINE,
  EXCEPTIONAL
};

enum class WeaponType
{
  NONE = 0,
  DAGGER,
  SHORT_SWORD,
  ARMING_SWORD,
  LONG_SWORD,
  GREAT_SWORD,
  STAFF,
  PICKAXE
};

enum class RangedWeaponType
{
  NONE = 0,
  SHORT_BOW,
  LONGBOW,
  WAR_BOW,
  LIGHT_XBOW,
  XBOW,
  HEAVY_XBOW
};

enum class ArmorType
{
  NONE = 0,
  PADDING,
  LEATHER,
  MAIL,
  SCALE,
  PLATE
};

enum class ItemBonusType
{
  NONE = 0,
  STR,            // adds to corresponding stat
  DEF,            //
  MAG,            //
  RES,            //
  SKL,            //
  SPD,            //
  HP,             // adds to max HP
  MP,             // adds to max MP
  INDESTRUCTIBLE,
  SELF_REPAIR,
  VISIBILITY,     // increases visibility radius
  DAMAGE,         // adds to total damage
  HUNGER,         // stops hunger counter
  IGNORE_DEFENCE, // ignores DEF for damage calculation
  KNOCKBACK,
  MANA_SHIELD,
  REGEN,
  REFLECT,
  LEECH,          // get hp on hit
  DMG_ABSORB,
  MAG_ABSORB,
  THORNS          // damages enemy if player was hit
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
  BLACK_OBSIDIAN,
  GREEN_JADE,
  PURPLE_FLUORITE,
  PURPLE_AMETHYST,
  RED_GARNET,
  WHITE_OPAL,
  BLACK_JETSTONE,
  ORANGE_AMBER,
  YELLOW_CITRINE,
  BLUE_AQUAMARINE,
  GREEN_EMERALD,
  BLUE_SAPPHIRE,
  ORANGE_JACINTH,
  RED_RUBY,
  WHITE_DIAMOND
};

enum class SpellType
{
  NONE = -1,  
  STRIKE = 0,      // 1 target, DEF
  MAGIC_MISSILE,   // 1 target, RES
  FROST,           // 1 target, RES, slow
  FIREBALL,        // area damage, RES
  LIGHTNING,       // chains (self-damage possible), RES
  LASER,           // pierces through, DEF
  LIGHT,           // increase visibility radius, temporary
  IDENTIFY,
  MAGIC_MAPPING,  
  TELEPORT,        // TODO: teleport control?
  TOWN_PORTAL,
  DETECT_MONSTERS, // shows monsters (not all?), temporary
  REMOVE_CURSE,
  REPAIR,
  HEAL,
  NEUTRALIZE_POISON,  
  MANA_SHIELD      // nuff said
};

// Number means tier
enum class WandMaterials
{
  YEW_1 = 0,
  IVORY_2,
  EBONY_3,
  ONYX_4,
  GLASS_5,
  COPPER_6,
  GOLDEN_7
};

enum class ArrowType
{
  NONE = 0,
  ARROWS,
  BOLTS
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
  MAGIC,        // 1 or 2 bonuses (50% chance)
  RARE,         // 3 bonuses
  UNIQUE
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
  ULCORNER_2 = 201,
  DLCORNER_2 = 200,
  URCORNER_2 = 187,
  DRCORNER_2 = 188,
  HBAR_2 = 205,
  VBAR_2 = 186,
  ULCORNER_3 = 222,
  DLCORNER_3 = 222,
  URCORNER_3 = 221,
  DRCORNER_3 = 221,
  HBAR_3U = 223,
  HBAR_3D = 220,
  VBAR_3L = 222,
  VBAR_3R = 221,
  BLOCK = 219,
  WAVES = 247,
};

namespace GlobalConstants
{
  static std::map<RoomEdgeEnum, std::string> DirNamesByDir =
  {
    { RoomEdgeEnum::NORTH, "NORTH" },
    { RoomEdgeEnum::EAST,  "EAST"  },
    { RoomEdgeEnum::SOUTH, "SOUTH" },
    { RoomEdgeEnum::WEST,  "WEST"  }
  };

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
    { NameCP437::URCORNER_1,   (int)NameCP437::URCORNER_1   },
    { NameCP437::ULCORNER_1,   (int)NameCP437::ULCORNER_1   },
    { NameCP437::DRCORNER_1,   (int)NameCP437::DRCORNER_1   },
    { NameCP437::DLCORNER_1,   (int)NameCP437::DLCORNER_1   },
    { NameCP437::HBAR_1,       (int)NameCP437::HBAR_1       },
    { NameCP437::VBAR_1,       (int)NameCP437::VBAR_1       },
    { NameCP437::URCORNER_2,   (int)NameCP437::URCORNER_2   },
    { NameCP437::ULCORNER_2,   (int)NameCP437::ULCORNER_2   },
    { NameCP437::DRCORNER_2,   (int)NameCP437::DRCORNER_2   },
    { NameCP437::DLCORNER_2,   (int)NameCP437::DLCORNER_2   },
    { NameCP437::HBAR_2,       (int)NameCP437::HBAR_2       },
    { NameCP437::VBAR_2,       (int)NameCP437::VBAR_2       },
    { NameCP437::URCORNER_3,   (int)NameCP437::URCORNER_3   },
    { NameCP437::ULCORNER_3,   (int)NameCP437::ULCORNER_3   },
    { NameCP437::DRCORNER_3,   (int)NameCP437::DRCORNER_3   },
    { NameCP437::DLCORNER_3,   (int)NameCP437::DLCORNER_3   },
    { NameCP437::HBAR_3U,      (int)NameCP437::HBAR_3U      },
    { NameCP437::HBAR_3D,      (int)NameCP437::HBAR_3D      },
    { NameCP437::VBAR_3L,      (int)NameCP437::VBAR_3L      },
    { NameCP437::VBAR_3R,      (int)NameCP437::VBAR_3R      },
    { NameCP437::BLOCK,        (int)NameCP437::BLOCK        },
    { NameCP437::WAVES,        (int)NameCP437::WAVES        }
  };

  static const std::string Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  static const std::string Tileset8x16Base64 = "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";
  //static const std::string IconBase64 = "AAABAAEAICAAAAEAIACoEAAAFgAAACgAAAAgAAAAQAAAAAEAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA//////////////////////////////////////8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/////////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/////////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/////////////////wAAAP8AAAD//////////////////////wAAAP8AAAD/AAAA//////////////////////8AAAD/AAAA/wAAAP//////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/////////////////wAAAP8AAAD///////////8AAAD///////////8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP////////////////8AAAD/AAAA/wAAAP8AAAD/AAAA/////////////////wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/////////////////wAAAP8AAAD/AAAA/wAAAP8AAAD/////////////////AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/////////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP////////////////8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA//////////////////////8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/////////////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/////////////////wAAAP8AAAD/AAAA/wAAAP8AAAD/////////////////AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/wAAAP8AAAD//////////////////////wAAAP8AAAD/AAAA/wAAAP////////////////8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/wAAAP8AAAD/AAAA/////////////////wAAAP8AAAD//////wAAAP///////////wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////////////////////AAAA/wAAAP///////////wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD///////////8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA////////////AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP////////////////8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP///////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD//////////////////////////////////////wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAA/wAAAP8AAAD/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=";
  static const std::string IconBase64 = "iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAABhGlDQ1BJQ0MgcHJvZmlsZQAAKJF9kT1Iw0AcxV/TSkUrDnYQcchQdbEiKuKoVShChVArtOpgcukXNGlIUlwcBdeCgx+LVQcXZ10dXAVB8APExdVJ0UVK/F9SaBHrwXE/3t173L0DhFqJaVZgHNB020zGY2I6syoGX9GNIAIYw4jMLGNOkhJoO77u4ePrXZRntT/35+hRsxYDfCLxLDNMm3iDeHrTNjjvE4dZQVaJz4lHTbog8SPXFY/fOOddFnhm2Ewl54nDxGK+hZUWZgVTI54ijqiaTvlC2mOV8xZnrVRhjXvyF4ay+soy12kOIo5FLEGCCAUVFFGCjSitOikWkrQfa+MfcP0SuRRyFcHIsYAyNMiuH/wPfndr5SYnvKRQDOh4cZyPISC4C9SrjvN97Dj1E8D/DFzpTX+5Bsx8kl5tapEjoHcbuLhuasoecLkD9D8Zsim7kp+mkMsB72f0TRmg7xboWvN6a+zj9AFIUVeJG+DgEBjOU/Z6m3d3tvb275lGfz9yxXKnGLxfigAAAAZiS0dEAP8A/wD/oL2nkwAAAAlwSFlzAAAuIwAALiMBeKU/dgAAAAd0SU1FB+MLBwUKNGuTFPAAAAAZdEVYdENvbW1lbnQAQ3JlYXRlZCB3aXRoIEdJTVBXgQ4XAAAA1klEQVRYw+2XwQ3DMAhFIeoSeA3vv4G9hj0GPVStVDVxwIHgQ7+UU5SfJ34CGOElBrnw5L7Ka/s8xXx4tdaglCJ21XhtsJB4T601LqWwsrTTXhYvV3mFR7A2ABEBALy/Wg6pQM4ZUkpuEKIIiMgNYgjQe3eHOK1ArdUVQhQBIrpBiH9DLwhNH0APCG0jMoeY6YSmELOt2Aziyiwwgbg6jEYQIj0MegkiIjPz3gC7BeAQ4u594CeOiIXkCyJqI1JBeK1kYoj/UrrMwYRHUkKovMIjwOjj+RO9j2OYLO5RtQAAAABJRU5ErkJggg==";
  static const std::string AlphabetUppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static const std::string AlphabetLowercase = "abcdefghijklmnopqrstuvwxyz";
  static const std::string Numbers = "0123456789";
  static const std::string MoneyName = "Crowns";

  static const int MaxNameLength = 24;
  static const int HPMPBarLength = 20;
  static const int AttributeMinimumRaiseChance = 5;
  static const int AttributeIncreasedRaiseStep = 25;
  static const int AwardedExperienceStep = 5;
  static const int AwardedExpDefault = 20;
  static const int AwardedExpMax = 40;
  static const int MinHitChance = 5;
  static const int MaxHitChance = 95;
  static const int DisplayAttackDelayMs = 50;
  static const int InventoryMaxNameLength = 20;
  static const int InventoryMaxSize = 20;
  static const int TurnReadyValue = 100;
  static const int TurnTickValue = 100;

  // 11 is because if ( < StarvationDamageTimeout)
  static const int StarvationDamageTimeout = 11;

  static const std::string BlackColor = "#000000";
  static const std::string WhiteColor = "#FFFFFF";
  static const std::string DandelionYellowColor = "#F0E130";
  static const std::string RedPoppyColor = "#E42F0C";
  static const std::string MarbleColor = "#FFE0E0";
  static const std::string WallColor = "#888888";
  static const std::string CaveWallColor = "#964B00";
  static const std::string GrassColor = "#006600";
  static const std::string GrassDotColor = "#009900";
  static const std::string DirtColor = "#9B7653";
  static const std::string DirtDotColor = "#7B5633";
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
  static const std::string StoneColor = "#AAAAAA";
  static const std::string BrickColor = "#DC5539";
  static const std::string LavaColor = "#E36517";
  static const std::string LavaWavesColor = "#E3A917";
  static const std::string FogOfWarColor = "#202020";
  static const std::string CoinsColor = "#FFD700";
  static const std::string DoorHighlightColor = "#666600";
  static const std::string MessageBoxDefaultBgColor = "#222222";
  static const std::string MessageBoxDefaultBorderColor = "#666666";
  static const std::string MessageBoxRedBorderColor = "#880000";
  static const std::string MessageBoxBlueBorderColor = "#8888FF";
  static const std::string MessageBoxHeaderBgColor = "#4444AA";
  static const std::string ItemMagicColor = "#4169E1";
  static const std::string ItemRareColor = "#EEEE52";
  static const std::string ItemUniqueColor = "#A59263";
  static const std::string ItemCursedColor = "#AA0000";

  static std::map<ItemQuality, std::string> QualityNameByQuality =
  {
    { ItemQuality::DAMAGED,     "Damaged"     },
    { ItemQuality::FLAWED,      "Flawed"      },
    { ItemQuality::NORMAL,      ""            },
    { ItemQuality::FINE,        "Fine"        },
    { ItemQuality::EXCEPTIONAL, "Exceptional" }
  };

  static const std::map<PlayerSkills, std::string> SkillNameByType =
  {
    { PlayerSkills::REPAIR,        "Repair"       },
    { PlayerSkills::RECHARGE,      "Recharge"     },
    { PlayerSkills::SPELLCASTING,  "Spellcasting" },
    { PlayerSkills::AWARENESS,     "Awareness"    }
  };

  static const std::map<TraderRole, std::string> ShopNameByType =
  {
    { TraderRole::BLACKSMITH, "Armory"    },
    { TraderRole::CLERIC,     "Sanctuary" },
    { TraderRole::COOK,       "Grocery"   },
    { TraderRole::JUNKER,     "Junkyard"  }
  };

  // Weighted random
  static const std::map<GameObjectType, std::map<ItemType, int>> LootTable =
  {
    {
      GameObjectType::RAT,
      {
        { ItemType::COINS,    1 },
        { ItemType::FOOD,     4 },
        { ItemType::NOTHING, 20 }
      }
    },
    {
      GameObjectType::BAT,
      {
        { ItemType::COINS,    1 },
        { ItemType::FOOD,     4 },
        { ItemType::NOTHING, 20 }
      }
    },
    {
      GameObjectType::SPIDER,
      {
        { ItemType::COINS,    2 },
        { ItemType::NOTHING, 20 }
      }
    }
  };

  static const std::map<GameObjectType, std::map<FoodType, int>> FoodLootTable =
  {
    {
      GameObjectType::RAT,
      {
        { FoodType::CHEESE, 4 },
        { FoodType::MEAT,   1 }
      }
    },
    {
      GameObjectType::BAT,
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
    { WeaponType::DAGGER,       "Dagger"         },
    { WeaponType::SHORT_SWORD,  "Short Sword"    },
    { WeaponType::ARMING_SWORD, "Arming Sword"   },
    { WeaponType::LONG_SWORD,   "Longsword"      },
    { WeaponType::GREAT_SWORD,  "Great Sword"    },
    { WeaponType::STAFF,        "Battle Staff"   },
    { WeaponType::PICKAXE,      "Pickaxe"        }
  };

  static const std::map<RangedWeaponType, std::string> RangedWeaponNameByType =
  {
    { RangedWeaponType::SHORT_BOW,    "Short Bow"      },
    { RangedWeaponType::LONGBOW,      "Longbow"        },
    { RangedWeaponType::WAR_BOW,      "War Bow"        },
    { RangedWeaponType::LIGHT_XBOW,   "L. Crossbow"    },
    { RangedWeaponType::XBOW,         "Crossbow"       },
    { RangedWeaponType::HEAVY_XBOW,   "H. Crossbow"    }
  };

  static const std::map<ArrowType, std::string> ArrowNameByType =
  {
    { ArrowType::ARROWS, "Arrows" },
    { ArrowType::BOLTS,  "Bolts"  }
  };

  static const std::map<ArmorType, std::string> ArmorNameByType =
  {    
    { ArmorType::PADDING, "Padded Surcoat"   },
    { ArmorType::LEATHER, "Leather Jacket"   },
    { ArmorType::MAIL,    "Mail Hauberk"     },
    { ArmorType::SCALE,   "Scale Armor"      },
    { ArmorType::PLATE,   "Coat of Plates"   }
  };

  static const std::map<ArmorType, int> ArmorDurabilityByType =
  {
    { ArmorType::PADDING, 25 },
    { ArmorType::LEATHER, 50 },
    { ArmorType::MAIL,    80 },
    { ArmorType::SCALE,  120 },
    { ArmorType::PLATE,  180 }
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
    { GemType::WORTHLESS_GLASS, 0    },   // 0
    { GemType::BLACK_OBSIDIAN,  200  },  // 200
    { GemType::GREEN_JADE,      300  },  // 300
    { GemType::PURPLE_FLUORITE, 400  },  // 400
    { GemType::PURPLE_AMETHYST, 600  },  // 600
    { GemType::RED_GARNET,      700  },  // 700
    { GemType::WHITE_OPAL,      800  },  // 800
    { GemType::BLACK_JETSTONE,  850  },  // 850
    { GemType::ORANGE_AMBER,    1000 },  // 1000
    { GemType::YELLOW_CITRINE,  1500 },  // 1500
    { GemType::BLUE_AQUAMARINE, 1500 },  // 1500
    { GemType::GREEN_EMERALD,   2500 },  // 2500
    { GemType::BLUE_SAPPHIRE,   3000 },  // 3000
    { GemType::ORANGE_JACINTH,  3250 },  // 3250
    { GemType::RED_RUBY,        3500 },  // 3500
    { GemType::WHITE_DIAMOND,   4000 }   // 4000
  };

  static const std::map<WandMaterials, int> WandCapacityByMaterial =
  {
    { WandMaterials::YEW_1,    100 },
    { WandMaterials::IVORY_2,  150 },
    { WandMaterials::EBONY_3,  200 },
    { WandMaterials::ONYX_4,   300 },
    { WandMaterials::GLASS_5,  450 },
    { WandMaterials::COPPER_6, 600 },
    { WandMaterials::GOLDEN_7, 800 }
  };

  // Divide wand capacity by this value to get amount of charges
  static const std::map<SpellType, int> WandSpellCapacityCostByType =
  {
    { SpellType::LIGHT,         10  },
    { SpellType::STRIKE,        25  },
    { SpellType::FROST,         50  },
    { SpellType::TELEPORT,      75  },
    { SpellType::FIREBALL,      100 },
    { SpellType::LASER,         150 },
    { SpellType::LIGHTNING,     100 },
    { SpellType::MAGIC_MISSILE, 25  }
  };

  static const std::vector<SpellType> ScrollValidSpellTypes =
  {
    SpellType::LIGHT,
    SpellType::IDENTIFY,
    SpellType::MAGIC_MAPPING,
    SpellType::TELEPORT,
    SpellType::DETECT_MONSTERS,
    SpellType::REMOVE_CURSE,
    SpellType::REPAIR,
    SpellType::TOWN_PORTAL,
    SpellType::HEAL,
    SpellType::NEUTRALIZE_POISON,
    SpellType::MANA_SHIELD,
  };

  // Not including combat spells
  static const std::vector<std::string> ScrollUnidentifiedNames =
  {
    "TAMAM SHUD",
    "ELAM EBOW",
    "ELBIB YLOH",
    "ZELGO MER",
    "LOREM IPSUM",
    "GHOTI",
    "LALIHO",
    "SHAN DONG",
    "SUIL A RUIN",
    "DULAMAN GAELACH",
    "KOBAYASHI MARU"
  };

  static const std::map<WandMaterials, std::pair<std::string, std::string>> WandColorsByMaterial =
  {
    { WandMaterials::YEW_1,    { "#C19A6B", "#8A8B5C" } },
    { WandMaterials::IVORY_2,  { "#FFFFFF", "#9A9A9A" } },
    { WandMaterials::EBONY_3,  { "#888888", "#555D50" } },
    { WandMaterials::ONYX_4,   { "#666666", "#2F2F2F" } },
    { WandMaterials::GLASS_5,  { "#000000", "#FFFFFF" } },
    { WandMaterials::COPPER_6, { "#FF8C00", "#B87333" } },
    { WandMaterials::GOLDEN_7, { "#FFFF00", "#DDD700" } }
  };

  static const std::map<WandMaterials, std::string> WandMaterialNamesByMaterial =
  {
    { WandMaterials::YEW_1,    "Yew"    },
    { WandMaterials::IVORY_2,  "Ivory"  },
    { WandMaterials::EBONY_3,  "Ebony"  },
    { WandMaterials::ONYX_4,   "Onyx"   },
    { WandMaterials::GLASS_5,  "Glass"  },
    { WandMaterials::COPPER_6, "Copper" },
    { WandMaterials::GOLDEN_7, "Golden" }
  };

  static const std::map<ShrineType, std::string> ShrineSaintByType =
  {
    { ShrineType::MIGHT,       "St. George the Soldier"     },
    { ShrineType::SPIRIT,      "St. Mary the Mother"        },
    { ShrineType::KNOWLEDGE,   "St. Nestor the Scribe"      },
    { ShrineType::PERCEPTION,  "St. Justin the Philosopher" },
    { ShrineType::HEALING,     "St. Luke the Healer"        }
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
    { MapType::TOWN,
      {
        "Village of Darwin",
        "Town of Tristram",
        "Outpost of Fargoal",
        "Resort of Protvino",
        "Commune of Minetown",
        "Settlement of Punchtree"
      }
    },
    // Abandoned Mines
    { MapType::MINES_1,     { "Mine Entrance"        } },
    { MapType::MINES_2,     { "Forsaken Prospects"   } },
    { MapType::MINES_3,     { "Abandoned Mines"      } },
    { MapType::MINES_4,     { "Deep Mines"           } },
    { MapType::MINES_5,     { "Sealed Chamber"       } },
    // Caves of Circe
    { MapType::CAVES_1,     { "Catacombs"            } },
    { MapType::CAVES_2,     { "Caves of Circe"       } },
    { MapType::CAVES_3,     { "Corridors of Time"    } },
    { MapType::CAVES_4,     { "Windy Tunnels"        } },
    { MapType::CAVES_5,     { "Vertigo"              } },
    // Lost City
    { MapType::LOST_CITY,   { "Lost City"            } },
    // Deep Dark
    { MapType::DEEP_DARK_1, { "Embrace of Darkness"  } },
    { MapType::DEEP_DARK_2, { "Underdark"            } },
    { MapType::DEEP_DARK_3, { "Deep Dark"            } },
    { MapType::DEEP_DARK_4, { "Starless Night"       } },
    { MapType::DEEP_DARK_5, { "Sinister"             } },
    // Stygian Abyss
    { MapType::ABYSS_1,     { "Plains of Desolation" } },
    { MapType::ABYSS_2,     { "Grey Wastes"          } },
    { MapType::ABYSS_3,     { "Soul Pastures"        } },
    { MapType::ABYSS_4,     { "Stygian Abyss"        } },
    { MapType::ABYSS_5,     { "Hell's Maw"           } },
    // Nether
    { MapType::NETHER_1,    { "Blazing Gates"        } },
    { MapType::NETHER_2,    { "River of Fire"        } },
    { MapType::NETHER_3,    { "Red Wastes"           } },
    { MapType::NETHER_4,    { "Citadel"              } },
    { MapType::NETHER_5,    { "The Hearth"           } },
    // The End
    { MapType::THE_END,     { "???" } }
  };

  static const std::map<ItemBonusType, std::string> ItemBonusPrefixes =
  {
    { ItemBonusType::STR,             "Heavy"       },
    { ItemBonusType::DEF,             "Rampart"     },
    { ItemBonusType::MAG,             "Magical"     },
    { ItemBonusType::RES,             "Electrum"    },
    { ItemBonusType::SKL,             "Expert"      },
    { ItemBonusType::SPD,             "Swift"       },
    { ItemBonusType::HP,              "Heart's"     },
    { ItemBonusType::MP,              "Soulful"     },
    { ItemBonusType::INDESTRUCTIBLE,  "Everlasting" },
    { ItemBonusType::SELF_REPAIR,     "Reliable"    },
    { ItemBonusType::VISIBILITY,      "Shining"     },
    { ItemBonusType::DAMAGE,          "Fighter's"   },
    { ItemBonusType::HUNGER,          "Sustaining"  },
    { ItemBonusType::IGNORE_DEFENCE,  "Piercing"    },
    { ItemBonusType::KNOCKBACK,       "Mighty"      },
    { ItemBonusType::MANA_SHIELD,     "Spiritual"   },
    { ItemBonusType::REGEN,           "Restorative" },
    { ItemBonusType::REFLECT,         "Silver"      },
    { ItemBonusType::LEECH,           "Vampire"     },
    { ItemBonusType::DMG_ABSORB,      "Protective"  },
    { ItemBonusType::MAG_ABSORB,      "Shielding"   },
    { ItemBonusType::THORNS,          "Ivy"         }
  };

  static const std::map<ItemBonusType, std::string> ItemBonusSuffixes =
  {
    { ItemBonusType::STR,             "of Strength"    },
    { ItemBonusType::DEF,             "of Defence"     },
    { ItemBonusType::MAG,             "of Magic"       },
    { ItemBonusType::RES,             "of Resistance"  },
    { ItemBonusType::SKL,             "of Skill"       },
    { ItemBonusType::SPD,             "of Speed"       },
    { ItemBonusType::HP,              "of Life"        },
    { ItemBonusType::MP,              "of Mana"        },
    { ItemBonusType::INDESTRUCTIBLE,  "of Ages"        },
    { ItemBonusType::SELF_REPAIR,     "of Reliability" },
    { ItemBonusType::VISIBILITY,      "of the Sun"     },
    { ItemBonusType::DAMAGE,          "of Destruction" },
    { ItemBonusType::HUNGER,          "of Satiation"   },
    { ItemBonusType::IGNORE_DEFENCE,  "of the Master"  },
    { ItemBonusType::KNOCKBACK,       "of the Bear"    },
    { ItemBonusType::MANA_SHIELD,     "of the Force"   },
    { ItemBonusType::REGEN,           "of the Undying" },
    { ItemBonusType::REFLECT,         "of Reflection"  },
    { ItemBonusType::LEECH,           "of the Blood"   },
    { ItemBonusType::DMG_ABSORB,      "of Protection"  },
    { ItemBonusType::MAG_ABSORB,      "of Shielding"   },
    { ItemBonusType::THORNS,          "of the Ivy"     }
  };

  static const std::map<ItemBonusType, int> MoneyCostIncreaseByBonusType =
  {
    { ItemBonusType::STR,             250  },
    { ItemBonusType::DEF,             250  },
    { ItemBonusType::MAG,             250  },
    { ItemBonusType::RES,             250  },
    { ItemBonusType::SKL,             250  },
    { ItemBonusType::SPD,             250  },
    { ItemBonusType::HP,              100  },
    { ItemBonusType::MP,              100  },
    { ItemBonusType::INDESTRUCTIBLE,  5000 },
    { ItemBonusType::SELF_REPAIR,     125  },
    { ItemBonusType::VISIBILITY,       50  },
    { ItemBonusType::DAMAGE,          200  },
    { ItemBonusType::HUNGER,          300  },
    { ItemBonusType::IGNORE_DEFENCE,  350  },
    { ItemBonusType::KNOCKBACK,       100  },
    { ItemBonusType::MANA_SHIELD,     500  },
    { ItemBonusType::REGEN,           400  },
    { ItemBonusType::REFLECT,         500  },
    { ItemBonusType::LEECH,            20  },
    { ItemBonusType::DMG_ABSORB,      300  },
    { ItemBonusType::MAG_ABSORB,      300  },
    { ItemBonusType::THORNS,           40  }
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

  static const std::vector<StringsArray2D> ShrineLayouts =
  {
    // 0
    // Might, Spirit, Knowledge, Holy, Healing, Potential, Tranquility, Perception
    {
      "##.##",
      "#...#",
      "../..",
      "#...#",
      "##.##"
    },
    // 1
    // Holy, Healing, Potential, Tranquility, Perception
    {
      "wg gw",
      "gg gg",
      "  /  ",
      "gg gg",
      "wg gw"
    },
    // 2
    // Holy, Healing, Potential, Tranquility, Perception
    {
      "ww ww",
      "w   w",
      "  /  ",
      "w   w",
      "ww ww"
    },
    // 3
    // Forgotten, Hidden, Ruined, Desecrated, Disturbing
    {
      ".#+.#",
      ".   #",
      "../..",
      "#.  .",
      "##..#"
    },
    // 4
    // Abyssal
    {
      "ll ll",
      "l   l",
      "  /  ",
      "l   l",
      "ll ll"
    }
  };

  static const std::vector<StringsArray2D> PondLayouts =
  {
    {
      "         ",
      " www www ",
      " wgg ggw ",
      " wgg ggw ",
      "    W    ",
      " wgg ggw ",
      " wgg ggw ",
      " www www ",
      "         "
    },
    {
      "ggggggggg",
      "ggggggggg",
      "gg     gg",
      "gg WWW gg",
      "gg WWW gg",
      "gg WWW gg",
      "gg     gg",
      "ggggggggg",
      "ggggggggg"
    },
    {
      "         ",
      " ### ### ",
      " #ggggg# ",
      " #gwwwg# ",
      "  gwWwg  ",
      " #gwwwg# ",
      " #ggggg# ",
      " ### ### ",
      "         "
    }
  };

  static const std::vector<StringsArray2D> FountainLayouts =
  {
    {
      ".... ....",
      ".##. .##.",
      ".#.   .#.",
      ".. www ..",
      "   wFw   ",
      ".. www ..",
      ".#.   .#.",
      ".##. .##.",
      ".... ...."
    },
    {
      "ggggggggg",
      "gwwwgwwwg",
      "gwFwgwFwg",
      "gwwwgwwwg",
      "ggggggggg",
      "gwwwgwwwg",
      "gwFwgwFwg",
      "gwwwgwwwg",
      "ggggggggg"
    }
  };

  static const std::vector<StringsArray2D> GardenLayouts =
  {
    {
      "ggggggggg",
      "ggggTgggg",
      "ggTgggTgg",
      "ggggggggg",
      "gTggTggTg",
      "ggggggggg",
      "ggTgggTgg",
      "ggggTgggg",
      "ggggggggg"
    },
    {
      "gggg gggg",
      "gwww wwwg",
      "gwTg gTwg",
      "gwgg ggwg",
      "         ",
      "gwgg ggwg",
      "gwTg gTwg",
      "gwww wwwg",
      "gggg gggg"
    }
  };

  static const std::vector<StringsArray2D> PillarsLayouts =
  {
    {
      ".....",
      ".#.#.",
      ".....",
      ".#.#.",
      "....."
    },
    {
      ".......",
      ".##.##.",
      ".##.##.",
      ".......",
      ".##.##.",
      ".##.##.",
      "......."
    },
    {
      ".........",
      "..#...#..",
      ".###.###.",
      "..#...#..",
      ".........",
      "..#...#..",
      ".###.###.",
      "..#...#..",
      "........."
    }
  };

  static const std::map<ShrineType, std::vector<StringsArray2D>> ShrineLayoutsByType =
  {
    {
      ShrineType::MIGHT,       { ShrineLayouts[0] }
    },
    {
      ShrineType::SPIRIT,      { ShrineLayouts[0] }
    },
    {
      ShrineType::KNOWLEDGE,   { ShrineLayouts[0]}
    },
    {
      ShrineType::HOLY,        { ShrineLayouts[0],
                                 ShrineLayouts[1],
                                 ShrineLayouts[2] }
    },
    {
      ShrineType::HEALING,     { ShrineLayouts[0],
                                 ShrineLayouts[1],
                                 ShrineLayouts[2] }
    },
    {
      ShrineType::POTENTIAL,   { ShrineLayouts[0],
                                 ShrineLayouts[1],
                                 ShrineLayouts[2] }
    },
    {
      ShrineType::TRANQUILITY, { ShrineLayouts[0],
                                 ShrineLayouts[1],
                                 ShrineLayouts[2] }
    },
    {
      ShrineType::PERCEPTION,  { ShrineLayouts[0],
                                 ShrineLayouts[1],
                                 ShrineLayouts[2] }
    },
    {
      ShrineType::FORGOTTEN,   { ShrineLayouts[3] }
    },
    {
      ShrineType::HIDDEN,      { ShrineLayouts[3] }
    },
    {
      ShrineType::DESECRATED,  { ShrineLayouts[3] }
    },
    {
      ShrineType::RUINED,      { ShrineLayouts[3] }
    },
    {
      ShrineType::DISTURBING,  { ShrineLayouts[3] }
    },
    {
      ShrineType::ABYSSAL,     { ShrineLayouts[4] }
    }
  };

  static const std::vector<std::vector<std::string>> SpecialRooms =
  {    
    // Castle
    {
      "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
      "W###-###WWWWWWWWWWWWWWW###-###W",
      "W#     #WWWWWWWWWWWWWWW#     #W",
      "W#     #################     #W",
      "W|                           |W",
      "W#     #################     #W",
      "W#     #.#######.......#     #W",
      "W### #+#.#hh   S.......#+# ###W",
      "WWW# #...+   / S.........# #WWW",
      "WWW# #...#hh   S..######## #WWW",
      "WWW# #...#######..#     ## #WWW",
      "WWW# #............#     ## #WWW",
      ".#######..........+     ## #WWW",
      ".+.....+...ggg....+     ## #WWW",
      ".+.....+...gFg....#     ## #WWW",
      ".#######...ggg....#     ## #WWW",
      "WWW# #............######## #WWW",
      "WWW# #...................# #WWW",
      "WWW# #..##+###+###+##....# #WWW",
      "WWW# #..#   #   #   #....# #WWW",
      "W### #+##   #   #   #..#+# ###W",
      "W#     ##############..#     #W",
      "W#     #################     #W",
      "W|                           |W",
      "W#     #################     #W",
      "W#     #WWWWWWWWWWWWWWW#     #W",
      "W###-###WWWWWWWWWWWWWWW###-###W",
      "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
    }
  };
}

#endif
