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

#include "enumerations.h"

using StringsArray2D = std::vector<std::string>;
using CharArray2D    = std::vector<std::vector<char>>;
using IR             = std::pair<InteractionResult, GameStates>;

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
  extern const std::string ObsidianColorHigh;
  extern const std::string ObsidianColorLow;
  extern const std::string GrassColor;
  extern const std::string GrassDotColor;
  extern const std::string DirtColor;
  extern const std::string DirtDotColor;
  extern const std::string PlayerColor;
  extern const std::string MonsterColor;
  extern const std::string MonsterUniqueColor;
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
  extern const std::string HexChars;
  extern const std::string Base64Chars;

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

  extern const std::string BlackboardKeyPlayerPos;
  extern const std::string BlackboardKeyLastMinedPos;
  extern const std::string BlackboardKeyObjectId;

  extern const char InventoryEmptySlotChar;

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

  extern const std::string FmtPickedUpIS;
  extern const std::string FmtPickedUpS;
  extern const std::string FmtDroppedIS;
  extern const std::string FmtDroppedS;
  extern const std::string FmtSMissed;

  extern const std::vector<std::string> MsgNotInTown;

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
    extern const std::string RoyalGateText;
    extern const std::string TreeText;
    extern const std::string WitheredTreeText;
    extern const std::string GrassText;
    extern const std::string BloodText;
    extern const std::string HellrockText;
    extern const std::string HellstoneText;
  }
}

namespace GlobalConstants
{
  extern std::map<RoomEdgeEnum, std::string> DirNamesByDir;

#ifdef USE_SDL
  extern std::map<NameCP437, int> CP437IndexByType;
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
  extern const int MonstersRespawnTimeout;

  extern const int EffectDurationSkipsForTurn;
  extern const int EffectDefaultDuration;

  extern const std::map<int, std::pair<StatsEnum, std::string>> AllStatNames;
  extern const std::map<ItemBonusType, std::string> BonusNameByType;

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

  extern const std::map<ItemType,      int> PotionsWeightTable;
  extern const std::map<ArmorType,     int> ArmorDurabilityByType;
  extern const std::map<GemType,       int> GemCostByType;
  extern const std::map<WandMaterials, int> WandCapacityByMaterial;
  extern const std::map<WandMaterials, int> WandRangeByMaterial;
  extern const std::map<SpellType,     int> WandSpellCapacityCostByType;
  extern const std::map<ItemBonusType, int> MoneyCostIncreaseByBonusType;

  extern const std::map<std::string, ItemType> PotionTypeByStatName;

  extern const std::map<FoodType, std::pair<std::string, int>> FoodHungerPercentageByName;

  extern const std::map<WandMaterials, std::string>                 WandMaterialNamesByMaterial;
  extern const std::map<ShrineType,    std::string>                 ShrineSaintByType;
  extern const std::map<ShrineType,    std::string>                 ShrineNameByType;
  extern const std::map<MapType,       std::vector<std::string>>    MapLevelNames;
  extern const std::map<ShrineType,    std::vector<StringsArray2D>> ShrineLayoutsByType;

  extern const std::map<SpellType, std::string> SpellShortNameByType;

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
