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
  #define VK_DELETE    SDLK_DELETE

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
  #define VK_DELETE    KEY_DC

#endif

#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <unordered_set>
#include <functional>
#include <cstdint>

#include "enumerations.h"

using UOSetS  = std::unordered_set<std::string>;
using SetS    = std::set<std::string>;
using StringV = std::vector<std::string>;
using CharV2  = std::vector<std::vector<char>>;
using IR      = std::pair<InteractionResult, GameStates>;

namespace Colors
{
  namespace ShadesOfGrey
  {
    extern const uint32_t One;
    extern const uint32_t Two;
    extern const uint32_t Three;
    extern const uint32_t Four;
    extern const uint32_t Five;
    extern const uint32_t Six;
    extern const uint32_t Seven;
    extern const uint32_t Eight;
    extern const uint32_t Nine;
    extern const uint32_t Ten;
    extern const uint32_t Eleven;
    extern const uint32_t Twelve;
    extern const uint32_t Thirteen;
    extern const uint32_t Fourteen;
  }

  extern const uint32_t None;
  extern const uint32_t BlackColor;
  extern const uint32_t WhiteColor;
  extern const uint32_t RedColor;
  extern const uint32_t GreenColor;
  extern const uint32_t BlueColor;
  extern const uint32_t CyanColor;
  extern const uint32_t MagentaColor;
  extern const uint32_t YellowColor;
  extern const uint32_t DandelionYellowColor;
  extern const uint32_t RedPoppyColor;
  extern const uint32_t MarbleColor;
  extern const uint32_t CaveWallColor;
  extern const uint32_t ObsidianColorHigh;
  extern const uint32_t ObsidianColorLow;
  extern const uint32_t GrassColor;
  extern const uint32_t GrassDotColor;
  extern const uint32_t DirtColor;
  extern const uint32_t DirtDotColor;
  extern const uint32_t PlayerColor;
  extern const uint32_t MonsterColor;
  extern const uint32_t MonsterUniqueColor;
  extern const uint32_t ShallowWaterColor;
  extern const uint32_t DeepWaterColor;
  extern const uint32_t IronColor;
  extern const uint32_t RoomFloorColor;
  extern const uint32_t WoodColor;
  extern const uint32_t ChestColor;
  extern const uint32_t BrickColor;
  extern const uint32_t LavaColor;
  extern const uint32_t LavaWavesColor;
  extern const uint32_t FogOfWarColor;
  extern const uint32_t CoinsColor;
  extern const uint32_t DoorHighlightColor;
  extern const uint32_t MessageBoxRedBorderColor;
  extern const uint32_t MessageBoxBlueBorderColor;
  extern const uint32_t MessageBoxHeaderBgColor;
  extern const uint32_t ItemMagicColor;
  extern const uint32_t ItemRareColor;
  extern const uint32_t ItemUniqueColor;
  extern const uint32_t ItemMixedColor;
  extern const uint32_t ItemCursedColor;

  extern const std::unordered_map<ShrineType,    std::pair<uint32_t, uint32_t>> ShrineColorsByType;
  extern const std::unordered_map<std::string,   std::vector<uint32_t>>         PotionColorsByName;
  extern const std::unordered_map<GemType,       std::string>                   GemColorNameByType;
  extern const std::unordered_map<WandMaterials, std::pair<uint32_t, uint32_t>> WandColorsByMaterial;
  extern const std::unordered_map<GemType,       std::pair<uint32_t, uint32_t>> GemColorByType;
}

namespace Strings
{
  extern const std::string HexChars;
  extern const std::string Base64Chars;
  // ---------------------------------------------------------------------------
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
  extern const std::string TripleQuestionMarks;
  // ---------------------------------------------------------------------------
  extern const std::string BlackboardKeyPlayerPos;
  extern const std::string BlackboardKeyLastMinedPos;
  extern const std::string BlackboardKeyObjectId;
  // ---------------------------------------------------------------------------
  extern const char InventoryEmptySlotChar;
  // ---------------------------------------------------------------------------
  extern const std::string ItemDefaultDescAccessory;
  extern const std::string ItemDefaultDescGem;
  extern const std::string ItemDefaultDescWeaponDmg;
  extern const std::string ItemDefaultDescWeaponEnd;
  // ---------------------------------------------------------------------------
  extern const std::string MsgWait;
  extern const std::string MsgNotInWater;
  extern const std::string MsgStairsDown;
  extern const std::string MsgStairsUp;
  extern const std::string MsgInventoryFull;
  extern const std::string MsgNothingHere;
  extern const std::string MsgItemsLyingHere;
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
  // ---------------------------------------------------------------------------
  extern const std::string FmtPickedUpIS;
  extern const std::string FmtPickedUpS;
  extern const std::string FmtDroppedIS;
  extern const std::string FmtDroppedS;
  extern const std::string FmtSMissed;
  extern const std::string FmtSNotEvenScratched;
  // ---------------------------------------------------------------------------
  extern const std::vector<std::string> MsgNotInTown;
  // ---------------------------------------------------------------------------
  extern const std::string SaveFileName;
  // ---------------------------------------------------------------------------
  namespace TileNames
  {
    extern const std::string MineWallText;
    extern const std::string CaveWallText;
    extern const std::string ObsidianWallText;
    extern const std::string GroundText;
    extern const std::string WallsText;
    extern const std::string FlagstoneText;
    extern const std::string StoneText;
    extern const std::string StoneFloorText;
    extern const std::string StoneWallText;
    extern const std::string StoneTilesText;
    extern const std::string StonePillarText;
    extern const std::string StainedGlassText;
    extern const std::string WoodenFloorText;
    extern const std::string BrickWallText;
    extern const std::string WorkbenchText;
    extern const std::string ForgeText;
    extern const std::string BedText;
    extern const std::string FountainText;
    extern const std::string DirtText;
    extern const std::string RocksText;
    extern const std::string AbyssalRocksText;
    extern const std::string AbyssalFloorText;
    extern const std::string MarbleFenceText;
    extern const std::string MarbleColumnText;
    extern const std::string DeepWaterText;
    extern const std::string ShallowWaterText;
    extern const std::string LavaText;
    extern const std::string ChasmText;
    extern const std::string GatesText;
    extern const std::string StashText;
    extern const std::string WindowText;
    extern const std::string WoodenBenchText;
    extern const std::string RoyalGatesText;
    extern const std::string TreeText;
    extern const std::string WitheredTreeText;
    extern const std::string GrassText;
    extern const std::string BloodText;
    extern const std::string HellrockText;
    extern const std::string HellstoneText;
    extern const std::string TiledFloorText;
  }
  // ---------------------------------------------------------------------------
  namespace SerializationKeys
  {
    extern const std::string Root;
    extern const std::string Gid;
    extern const std::string Seed;
    extern const std::string Name;
    extern const std::string Value;
    extern const std::string Size;
    extern const std::string Type;
    extern const std::string Zone;
    extern const std::string Visibility;
    extern const std::string Respawn;
    extern const std::string MapObjects;
    extern const std::string Image;
    extern const std::string Color;
    extern const std::string FowName;
    extern const std::string Player;
    extern const std::string Border;
    extern const std::string Class;
    extern const std::string Pos;
    extern const std::string Owner;
    extern const std::string Ground;
    extern const std::string Id;
    extern const std::string Mask;
    extern const std::string Layout;
    extern const std::string Objects;
  }
}

namespace GlobalConstants
{
  extern const std::unordered_map<RoomEdgeEnum, std::string> DirNamesByDir;

#ifdef USE_SDL
  extern std::unordered_map<NameCP437, int> CP437IndexByType;
#endif

  extern int TerminalWidth;
  extern int TerminalHeight;

  extern const size_t OpenedByAnyone;
  extern const size_t OpenedByNobody;

  extern const size_t InventoryMaxNameLength;

  extern const int HPMPBarLength;
  extern const int AttributeMinimumRaiseChance;
  extern const int AttributeIncreasedRaiseStep;
  extern const int AwardedExperienceStep;
  extern const int AwardedExpMiniboss;
  extern const int AwardedExpBoss;
  extern const int AwardedExpMax;
  extern const int MinHitChance;
  extern const int MaxHitChance;
  extern const int DisplayAttackDelayMs;
  extern const int InventoryMaxSize;
  extern const int TurnReadyValue;
  extern const int TurnTickValue;
  extern const int MaxVisibilityRadius;
  extern const int StarvationDamageTimeout;
  extern const int MonstersRespawnTimeout;
  extern const int MaxNameLength;
  extern const int MaxSeedStringLength;
  extern const int EffectDurationSkipsForTurn;
  extern const int EffectDefaultDuration;

  extern const std::map<int, std::pair<StatsEnum, std::string>> AllStatNames;
  extern const std::unordered_map<ItemBonusType, std::string> BonusNameByType;

  extern const std::unordered_map<ItemQuality,      std::string> QualityNameByQuality;
  extern const std::unordered_map<PlayerSkills,     std::string> SkillNameByType;
  extern const std::unordered_map<TraderRole,       std::string> ShopNameByType;
  extern const std::unordered_map<PlayerStats,      std::string> StatNameByType;
  extern const std::unordered_map<PotionType,       std::string> PotionNameByType;
  extern const std::unordered_map<PotionType,       std::string> StatNameByPotionType;
  extern const std::unordered_map<WeaponType,       std::string> WeaponNameByType;
  extern const std::unordered_map<RangedWeaponType, std::string> RangedWeaponNameByType;
  extern const std::unordered_map<ArrowType,        std::string> ArrowNameByType;
  extern const std::unordered_map<ArmorType,        std::string> ArmorNameByType;
  extern const std::unordered_map<GemType,          std::string> GemNameByType;
  extern const std::unordered_map<GemType,              StringV> GemDescriptionByType;
  extern const std::unordered_map<ItemQuality,      std::string> GemRatingByQuality;
  extern const std::unordered_map<ItemBonusType,    std::string> ItemBonusPrefixes;
  extern const std::unordered_map<ItemBonusType,    std::string> ItemBonusSuffixes;
  extern const std::unordered_map<ItemBonusType,    std::string> BonusDisplayNameByType;

  extern const std::unordered_map<std::string, ScriptTaskNames>  BTSTaskNamesByName;
  extern const std::unordered_map<std::string, ScriptParamNames> BTSParamNamesByName;

  extern const std::unordered_map<PotionType,    int> PotionsWeightTable;
  extern const std::unordered_map<ArmorType,     int> ArmorDurabilityByType;
  extern const std::unordered_map<GemType,       int> GemCostByType;
  extern const std::unordered_map<WandMaterials, int> WandCapacityByMaterial;
  extern const std::unordered_map<WandMaterials, int> WandRangeByMaterial;
  extern const std::unordered_map<SpellType,     int> WandSpellCapacityCostByType;
  extern const std::unordered_map<ItemBonusType, int> MoneyCostIncreaseByBonusType;

  extern const std::unordered_map<FoodType, std::pair<std::string, int>> FoodHungerPercentageByName;

  extern const std::unordered_map<WandMaterials, std::string>          WandMaterialNamesByMaterial;
  extern const std::unordered_map<ShrineType,    std::string>          ShrineSaintByType;
  extern const std::unordered_map<ShrineType,    std::string>          ShrineNameByType;
  extern const std::unordered_map<MapType,       std::string>          MapLevelNames;
  extern const std::unordered_map<ShrineType,    std::vector<StringV>> ShrineLayoutsByType;

  extern const std::unordered_map<SpellType, std::string> SpellShortNameByType;

  extern const std::unordered_map<PlayerClass, std::string> PlayerClassNameByType;

  extern const std::unordered_map<GameObjectType, bool> CanSwimMap;

  extern const std::vector<SpellType> ScrollValidSpellTypes;

  extern const std::vector<std::string> ScrollUnidentifiedNames;
  extern const std::vector<std::string> RandomNames;

  extern const std::vector<std::string> TownNameEndings;
  extern const std::vector<std::string> NameEndings;

  extern const std::vector<StringV> ShrineLayouts;
  extern const std::vector<StringV> PondLayouts;
  extern const std::vector<StringV> FountainLayouts;
  extern const std::vector<StringV> GardenLayouts;
  extern const std::vector<StringV> PillarsLayouts;
  extern const std::vector<StringV> SpecialRooms;
}

#endif
