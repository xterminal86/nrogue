#ifndef CONSTANTS_H
#define CONSTANTS_H

#define VK_CANCEL      'q'

#define ALT_K7         '0'
#define ALT_K8         '-'
#define ALT_K9         '='
#define ALT_K4         'p'
#define ALT_K5         '\''
#define ALT_K6         ']'
#define ALT_K1         ';'
#define ALT_K2         '['
#define ALT_K3         '\\'

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
  #define KEY_RIGHT    SDLK_RIGHT
  #define KEY_LEFT     SDLK_LEFT

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
using CharArray2D    = std::vector<std::vector<char>>;

enum class GameStates
{
  EXIT_GAME = -1,
  ATTACK_STATE,
  MENU_STATE,
  MAIN_STATE,
  SELECT_CLASS_STATE,
  CUSTOM_CLASS_STATE,
  ENTER_NAME_STATE,
  INTRO_STATE,
  HELP_STATE,
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
  SERVICE_STATE,
  TARGET_STATE,
  EXITING_STATE,
  MESSAGE_BOX_STATE,
  DEV_CONSOLE,
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
  TOWN = 1,
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
  REMAINS,
  PLAYER,
  GROUND,
  SHALLOW_WATER,
  DEEP_WATER,
  LAVA,
  CHASM,
  PICKAXEABLE,
  TRAP,
  RAT,
  BAT,
  VAMPIRE_BAT,
  SPIDER,
  TROLL,
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

enum class ServiceType
{
  NONE = 0,
  IDENTIFY,
  REPAIR,
  RECHARGE,
  BLESS
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
  AWARENESS     // Autodetect traps (starting for Thief)
};

enum class PlayerStats
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

enum class EquipmentCategory
{
  NOT_EQUIPPABLE = 0,
  HEAD,
  NECK,
  TORSO,
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
  NP_POTION,
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
  INVISIBILITY,   // monsters can't see you (unless they can)
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
  THORNS,         // damages enemy if player was hit
  PARALYZE,
  TELEPATHY,      // See monsters
  LEVITATION,
  BLINDNESS,      // anti-visibility (-100500 to LightRadius)
  FROZEN,         // SPD penalty
  BURNING,        // inflicts fire damage over several turns + gives illuminated
  ILLUMINATED,    // + to light radius
  POISONED        // anti-regen
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
  NONE = -1,         // Wand of Nothing can be imbued with spell
  STRIKE = 0,        // 1 target, DEF
  MAGIC_MISSILE,     // 1 target, RES
  FROST,             // 1 target, RES, slow
  FIREBALL,          // area damage, RES
  LIGHTNING,         // chains (self-damage possible), RES
  LASER,             // pierces through, DEF
  LIGHT,             // increase visibility radius, temporary
  IDENTIFY,
  MAGIC_MAPPING,
  TELEPORT,          // TODO: teleport control?
  TOWN_PORTAL,
  DETECT_MONSTERS,   // shows monsters (not all?), temporary
  REMOVE_CURSE,
  REPAIR,
  HEAL,
  NEUTRALIZE_POISON,
  MANA_SHIELD        // nuff said
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

enum class DoorMaterials
{
  WOOD = 0,
  STONE,
  IRON
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

// In certain algorithms it is more suitable
// to call the same type by another name.
using Direction = RoomEdgeEnum;

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

#ifdef USE_SDL
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
  EXCL_MARK_INV = 173,
  DBL_QUOTES_OPEN = 174,
  DBL_QUOTES_CLOSED = 175,
  SHADING_1 = 176,
  SHADING_2 = 177,
  SHADING_3 = 178,
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
  LAST_ELEMENT
};

enum class GraphicTiles
{
  BARREL = 256,
  BED,
  BRICKS_RED1,
  BRICKS_RED2,
  CHAMOMILE,
  DANDELION,
  FACHWERK1,
  FACHWERK2,
  GRASS,
  GRAVEL1,
  GRAVEL2,
  GATE_IRON_CLOSED,
  MINE_WALL1,
  MINE_WALL2,
  PICKAXE,
  POPPY,
  RING,
  STAIRS_DOWN,
  STAIRS_UP,
  CHEST,
  BRICKS_WHITE1,
  BRICKS_WHITE2,
  TILES_STONE,
  SUPPORT_BEAM,
  TILES_STONE_OLD,
  WATER_DEEP,
  WATER_SHALLOW,
  WOODEN_PLANKS,
  DOOR_WS_CLOSED,
  DOOR_WS_OPEN,
  DOOR_W_CLOSED,
  DOOR_W_OPEN,
  GATE_IRON_OPEN,
  COBBLESTONE,
  WINDOW,
  DIRT,
  GEM_BLACK,
  GEM_BLUE,
  GEM_GREEN,
  GEM_ORANGE,
  GEM_PURPLE,
  GEM_RED,
  GEM_YELLOW,
  GEM_WHITE,
  AMULET_MOON,
  FACHWERK3,
  FACHWERK4,
  CHEST_GOLD,
  STASH,
  EARTH,
  CRATE1,
  POTION_YELLOW,
  POTION_WHITE,
  POTION_RED,
  POTION_RADIANT,
  POTION_MORBID,
  POTION_MAGENTA,
  POTION_GREEN,
  POTION_CYAN,
  POTION_BLUE,
  POTION_BLACK,
  CRATE2,
  PICKAXE_UNIQUE,
  LAST_ELEMENT
};
#endif

namespace Colors
{
  namespace ShadesOfGrey
  {
    extern const std::string One;
    extern const std::string Two;
    extern const std::string Three;
    extern const std::string Four;
    extern const std::string Five;
    extern const std::string Six;
    extern const std::string Seven;
    extern const std::string Eight;
    extern const std::string Nine;
    extern const std::string Ten;
    extern const std::string Eleven;
    extern const std::string Twelve;
    extern const std::string Thirteen;
    extern const std::string Fourteen;
  }

  extern const std::string BlackColor;
  extern const std::string WhiteColor;
  extern const std::string RedColor;
  extern const std::string GreenColor;
  extern const std::string BlueColor;
  extern const std::string CyanColor;
  extern const std::string MagentaColor;
  extern const std::string YellowColor;
  extern const std::string DandelionYellowColor;
  extern const std::string RedPoppyColor;
  extern const std::string MarbleColor;
  extern const std::string CaveWallColor;
  extern const std::string GrassColor;
  extern const std::string GrassDotColor;
  extern const std::string DirtColor;
  extern const std::string DirtDotColor;
  extern const std::string PlayerColor;
  extern const std::string MonsterColor;
  extern const std::string ShallowWaterColor;
  extern const std::string DeepWaterColor;
  extern const std::string IronColor;
  extern const std::string RoomFloorColor;
  extern const std::string WoodColor;
  extern const std::string ChestColor;
  extern const std::string BrickColor;
  extern const std::string LavaColor;
  extern const std::string LavaWavesColor;
  extern const std::string FogOfWarColor;
  extern const std::string CoinsColor;
  extern const std::string DoorHighlightColor;
  extern const std::string MessageBoxRedBorderColor;
  extern const std::string MessageBoxBlueBorderColor;
  extern const std::string MessageBoxHeaderBgColor;
  extern const std::string ItemMagicColor;
  extern const std::string ItemRareColor;
  extern const std::string ItemUniqueColor;
  extern const std::string ItemMixedColor;
  extern const std::string ItemCursedColor;

  extern const std::map<ShrineType,    std::pair<std::string, std::string>> ShrineColorsByType;
  extern const std::map<std::string,   std::vector<std::string>>            PotionColorsByName;
  extern const std::map<GemType,       std::string>                         GemColorNameByType;
  extern const std::map<WandMaterials, std::pair<std::string, std::string>> WandColorsByMaterial;
  extern const std::map<GemType,       std::pair<std::string, std::string>> GemColorByType;
}

namespace Strings
{
  extern const std::string Base64Chars;
  extern const std::string Tileset8x16Base64;
  extern const std::string IconBase64;
  extern const std::string HexChars;

  extern const std::string AlphabetUppercase;
  extern const std::string AlphabetLowercase;
  extern const std::string Vowels;
  extern const std::string Consonants;
  extern const std::string Numbers;
  extern const std::string MoneyName;
  extern const std::string NoActionText;
  extern const std::string MessageBoxInformationHeaderText;
  extern const std::string MessageBoxEpicFailHeaderText;
  extern const std::string UnidentifiedEffectText;

  extern const std::string BlackboardKeyPlayerPosX;
  extern const std::string BlackboardKeyPlayerPosY;

  extern const char InventoryEmptySlotChar;

  extern const std::string MsgWait;
  extern const std::string MsgNotInWater;
  extern const std::string MsgStairsDown;
  extern const std::string MsgStairsUp;
  extern const std::string MsgInventoryFull;
  extern const std::string MsgNothingHere;
  extern const std::string MsgSomethingLying;
  extern const std::string MsgNoStairsDown;
  extern const std::string MsgNoStairsUp;
  extern const std::string MsgNotRangedWeapon;
  extern const std::string MsgEquipWeapon;
  extern const std::string MsgWhatToShoot;
  extern const std::string MsgWrongAmmo;
  extern const std::string MsgNoAmmo;
  extern const std::string MsgNoCharges;
  extern const std::string MsgCancelled;
  extern const std::string MsgUnequipFirst;
  extern const std::string MsgLooksImportant;
  extern const std::string MsgNoRoomInInventory;
  extern const std::string MsgNotEnoughMoney;
  extern const std::string MsgInteractDir;
  extern const std::string MsgItemUndamaged;
  extern const std::string MsgCantBeUsed;
  extern const std::string MsgNoMoney;

  extern const std::string FmtPickedUpIS;
  extern const std::string FmtPickedUpS;
  extern const std::string FmtDroppedIS;
  extern const std::string FmtDroppedS;

  extern const std::vector<std::string> MsgNotInTown;
}

namespace GlobalConstants
{
  extern std::map<RoomEdgeEnum, std::string> DirNamesByDir;

#ifdef USE_SDL
  extern std::map<GraphicTiles, int> GraphicTilesIndexByType;
  extern std::map<NameCP437, int>    CP437IndexByType;
#endif

  // ===============================================

  extern int TerminalWidth;
  extern int TerminalHeight;

  extern const size_t OpenedByAnyone;
  extern const size_t OpenedByNobody;

  extern const size_t InventoryMaxNameLength;

  extern const int HPMPBarLength;
  extern const int AttributeMinimumRaiseChance;
  extern const int AttributeIncreasedRaiseStep;
  extern const int AwardedExperienceStep;
  extern const int AwardedExpDefault;
  extern const int AwardedExpMax;
  extern const int MinHitChance;
  extern const int MaxHitChance;
  extern const int DisplayAttackDelayMs;
  extern const int InventoryMaxSize;
  extern const int TurnReadyValue;
  extern const int TurnTickValue;
  extern const int MaxVisibilityRadius;
  extern const int StarvationDamageTimeout;
  extern const int LavaDamage;

  extern const int EffectDurationSkipsForTurn;
  extern const int EffectDefaultDuration;

  extern const std::map<ItemQuality,      std::string> QualityNameByQuality;
  extern const std::map<PlayerSkills,     std::string> SkillNameByType;
  extern const std::map<TraderRole,       std::string> ShopNameByType;
  extern const std::map<PlayerStats,      std::string> StatNameByType;
  extern const std::map<ItemType,         std::string> PotionNameByType;
  extern const std::map<ItemType,         std::string> StatNameByPotionType;
  extern const std::map<WeaponType,       std::string> WeaponNameByType;
  extern const std::map<RangedWeaponType, std::string> RangedWeaponNameByType;
  extern const std::map<ArrowType,        std::string> ArrowNameByType;
  extern const std::map<ArmorType,        std::string> ArmorNameByType;
  extern const std::map<GemType,          std::string> GemNameByType;
  extern const std::map<ItemBonusType,    std::string> ItemBonusPrefixes;
  extern const std::map<ItemBonusType,    std::string> ItemBonusSuffixes;
  extern const std::map<ItemBonusType,    std::string> BonusDisplayNameByType;

  extern const std::map<GameObjectType, std::map<ItemType, int>> LootTable;
  extern const std::map<GameObjectType, std::map<FoodType, int>> FoodLootTable;

  extern const std::map<ItemType,      int> PotionsWeightTable;
  extern const std::map<ArmorType,     int> ArmorDurabilityByType;
  extern const std::map<GemType,       int> GemCostByType;
  extern const std::map<WandMaterials, int> WandCapacityByMaterial;
  extern const std::map<WandMaterials, int> WandRangeByMaterial;
  extern const std::map<SpellType,     int> WandSpellCapacityCostByType;
  extern const std::map<ItemBonusType, int> MoneyCostIncreaseByBonusType;

  extern const std::map<std::string, ItemType> PotionTypeByStatName;

  extern const std::map<FoodType, std::pair<std::string, int>>         FoodHungerPercentageByName;

  extern const std::map<WandMaterials, std::string>                         WandMaterialNamesByMaterial;
  extern const std::map<ShrineType,    std::string>                         ShrineSaintByType;
  extern const std::map<ShrineType,    std::string>                         ShrineNameByType;
  extern const std::map<MapType,       std::vector<std::string>>            MapLevelNames;
  extern const std::map<ShrineType,    std::vector<StringsArray2D>>         ShrineLayoutsByType;

  extern const std::vector<SpellType> ScrollValidSpellTypes;

  extern const std::vector<std::string> ScrollUnidentifiedNames;
  extern const std::vector<std::string> RandomNames;

  extern const std::vector<std::string> TownNameEndings;
  extern const std::vector<std::string> NameEndings;

  extern const std::vector<StringsArray2D> DungeonRooms;
  extern const std::vector<StringsArray2D> ShrineLayouts;
  extern const std::vector<StringsArray2D> PondLayouts;
  extern const std::vector<StringsArray2D> FountainLayouts;
  extern const std::vector<StringsArray2D> GardenLayouts;
  extern const std::vector<StringsArray2D> PillarsLayouts;
  extern const std::vector<StringsArray2D> SpecialRooms;
}

#endif
