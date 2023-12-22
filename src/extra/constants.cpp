#include "constants.h"
#include "base64-strings.h"
#include "string-obfuscator.h"

namespace Colors
{
  namespace ShadesOfGrey
  {
    const uint32_t One      = 0x111111;
    const uint32_t Two      = 0x222222;
    const uint32_t Three    = 0x333333;
    const uint32_t Four     = 0x444444;
    const uint32_t Five     = 0x555555;
    const uint32_t Six      = 0x666666;
    const uint32_t Seven    = 0x777777;
    const uint32_t Eight    = 0x888888;
    const uint32_t Nine     = 0x999999;
    const uint32_t Ten      = 0xAAAAAA;
    const uint32_t Eleven   = 0xBBBBBB;
    const uint32_t Twelve   = 0xCCCCCC;
    const uint32_t Thirteen = 0xDDDDDD;
    const uint32_t Fourteen = 0xEEEEEE;
  }

  //
  // Special value to determine whether we should draw an object or not.
  //
  const uint32_t None = 0xFFFFFFFF;

  //
  // Mouse over commented string to get corresponding
  // color sample in QT Creator.
  //
  const uint32_t BlackColor                = 0x000000; // "#000000";
  const uint32_t WhiteColor                = 0xFFFFFF; // "#FFFFFF";
  const uint32_t RedColor                  = 0xFF0000; // "#FF0000";
  const uint32_t GreenColor                = 0x00FF00; // "#00FF00";
  const uint32_t BlueColor                 = 0x0000FF; // "#0000FF";
  const uint32_t CyanColor                 = 0x00FFFF; // "#00FFFF";
  const uint32_t MagentaColor              = 0xFF00FF; // "#FF00FF";
  const uint32_t YellowColor               = 0xFFFF00; // "#FFFF00";
  const uint32_t DandelionYellowColor      = 0xF0E130; // "#F0E130";
  const uint32_t RedPoppyColor             = 0xE42F0C; // "#E42F0C";
  const uint32_t MarbleColor               = 0xFFE0E0; // "#FFE0E0";
  const uint32_t CaveWallColor             = 0x964B00; // "#964B00";
  const uint32_t ObsidianColorHigh         = 0x5B4965; // "#5B4965";
  const uint32_t ObsidianColorLow          = 0x3D354B; // "#3D354B";
  const uint32_t GrassColor                = 0x006600; // "#006600";
  const uint32_t GrassDotColor             = 0x009900; // "#009900";
  const uint32_t DirtColor                 = 0x9B7653; // "#9B7653";
  const uint32_t DirtDotColor              = 0x7B5633; // "#7B5633";
  const uint32_t PlayerColor               = 0x00FFFF; // "#00FFFF";
  const uint32_t MonsterColor              = 0xFF0000; // "#FF0000";
  const uint32_t MonsterUniqueColor        = 0xFF8888; // "#FF8888";
  const uint32_t ShallowWaterColor         = 0x9999FF; // "#9999FF";
  const uint32_t DeepWaterColor            = 0x3333FF; // "#3333FF";
  const uint32_t IronColor                 = 0xCBCDCD; // "#CBCDCD";
  const uint32_t RoomFloorColor            = 0x692E11; // "#692E11";
  const uint32_t WoodColor                 = 0xDCB579; // "#DCB579";
  const uint32_t ChestColor                = 0xA0793D; // "#A0793D";
  const uint32_t BrickColor                = 0xDC5539; // "#DC5539";
  const uint32_t LavaColor                 = 0xE36517; // "#E36517";
  const uint32_t LavaWavesColor            = 0xE3A917; // "#E3A917";
  const uint32_t FogOfWarColor             = 0x202020; // "#202020";
  const uint32_t CoinsColor                = 0xFFD700; // "#FFD700";
  const uint32_t DoorHighlightColor        = 0x666600; // "#666600";
  const uint32_t MessageBoxRedBorderColor  = 0x880000; // "#880000";
  const uint32_t MessageBoxBlueBorderColor = 0x8888FF; // "#8888FF";
  const uint32_t MessageBoxHeaderBgColor   = 0x4444AA; // "#4444AA";
  const uint32_t ItemMagicColor            = 0x4169E1; // "#4169E1";
  const uint32_t ItemRareColor             = 0xEEEE52; // "#EEEE52";
  const uint32_t ItemUniqueColor           = 0xA59263; // "#A59263";
  const uint32_t ItemMixedColor            = 0xAA7700; // "#AA7700";
  const uint32_t ItemCursedColor           = 0xAA0000; // "#AA0000";

  const std::unordered_map<GemType, std::pair<uint32_t, uint32_t>> GemColorByType =
  {
    { GemType::BLACK_JETSTONE,  { 0xFFFFFF, 0x000000 } },
    { GemType::BLACK_OBSIDIAN,  { 0xFFFFFF, 0x000000 } },
    { GemType::BLUE_AQUAMARINE, { 0xFFFFFF, 0x0000FF } }, // "#88FFFF"
    { GemType::BLUE_SAPPHIRE,   { 0xFFFFFF, 0x0000FF } },
    { GemType::GREEN_EMERALD,   { 0xFFFFFF, 0x88CC88 } }, // "#88CC88"
    { GemType::GREEN_JADE,      { 0xFFFFFF, 0x88CC88 } },
    { GemType::ORANGE_AMBER,    { 0x000000, 0xFF9900 } }, // "#FF9900"
    { GemType::ORANGE_JACINTH,  { 0x000000, 0xFF9900 } },
    { GemType::PURPLE_AMETHYST, { 0xFFFFFF, 0xA000A0 } }, // "#A000A0"
    { GemType::PURPLE_FLUORITE, { 0xFFFFFF, 0xA000A0 } },
    { GemType::RED_GARNET,      { 0xFFFFFF, 0xAA0000 } }, // "#AA0000"
    { GemType::RED_RUBY,        { 0xFFFFFF, 0xAA0000 } },
    { GemType::WHITE_DIAMOND,   { 0x000000, 0xFFFFFF } },
    { GemType::WHITE_OPAL,      { 0x000000, 0xFFFFFF } },
    { GemType::YELLOW_CITRINE,  { 0x000000, 0xFFFF00 } }
  };

  const std::unordered_map<ShrineType, std::pair<uint32_t, uint32_t>> ShrineColorsByType =
  {
    { ShrineType::MIGHT,       { 0xFF0000, 0x888888 } },
    { ShrineType::SPIRIT,      { 0x0088FF, 0x888888 } },
    { ShrineType::TRANQUILITY, { 0x0088FF, 0x888888 } },
    { ShrineType::KNOWLEDGE,   { 0x44FF44, 0x888888 } },
    { ShrineType::PERCEPTION,  { 0xFFFFFF, 0x888888 } },
    { ShrineType::HEALING,     { 0xFF0000, 0x888888 } },
    { ShrineType::FORGOTTEN,   { 0xFFFFFF, 0x888888 } },
    { ShrineType::ABYSSAL,     { 0xFF8000, 0x880000 } },
    { ShrineType::DESECRATED,  { 0x888800, 0x440000 } },
    { ShrineType::DISTURBING,  { 0x660000, 0x888888 } },
    { ShrineType::RUINED,      { 0x666666, 0x000000 } },
    { ShrineType::POTENTIAL,   { 0xFF0000, 0x888888 } },
    { ShrineType::HIDDEN,      { 0x888888, 0x333333 } },
    { ShrineType::HOLY,        { 0xFFFF00, 0x888888 } },
  };

  const std::unordered_map<std::string, std::vector<uint32_t>> PotionColorsByName =
  {
    { "Red Potion",     { 0xFF0000, 0x440000 } },
    { "Green Potion",   { 0x00FF00, 0x004400 } },
    { "Blue Potion",    { 0x4444FF, 0x000044 } },
    { "Cyan Potion",    { 0x00FFFF, 0x004444 } },
    { "Magenta Potion", { 0xFF00FF, 0x440044 } },
    { "Yellow Potion",  { 0xFFFF00, 0x444400 } },
    { "Radiant Potion", { 0xFFFF88, 0x777700 } },
    { "Morbid Potion",  { 0xAA6622, 0x880000 } },
    { "Clear Potion",   { 0xFFFFFF, 0x000000 } },
    { "Black Potion",   { 0x000000, 0x444444 } },
    { "Watery Potion",  { 0xBBBBBB, 0x000000 } }
  };

  const std::unordered_map<WandMaterials, std::pair<uint32_t, uint32_t>> WandColorsByMaterial =
  {
    { WandMaterials::YEW_1,    { 0xD2AB7C, 0x8A8B5C } },
    { WandMaterials::IVORY_2,  { 0xFFFFFF, 0x9A9A9A } },
    { WandMaterials::EBONY_3,  { 0x888888, 0x555D50 } },
    { WandMaterials::ONYX_4,   { 0x666666, 0x2F2F2F } },
    { WandMaterials::GLASS_5,  { 0x000000, 0xFFFFFF } },
    { WandMaterials::COPPER_6, { 0xFF8C00, 0xB87333 } },
    { WandMaterials::GOLDEN_7, { 0xFFFF00, 0xAAA700 } }
  };

  const std::unordered_map<GemType, std::string> GemColorNameByType =
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
}

// =============================================================================

namespace Strings
{
  const std::string HexChars    = "0123456789ABCDEF";
  const std::string Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

  const std::string AlphabetUppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  const std::string AlphabetLowercase = "abcdefghijklmnopqrstuvwxyz";

  const std::string Vowels     = "aeiouy";
  const std::string Consonants = "bcdfghjklmnpqrstvwxz";

  const std::string Numbers   = "0123456789";
  const std::string MoneyName = "Crowns";

  const std::string NoActionText           = "Nothing happens";
  const std::string UnidentifiedEffectText = "?not sure?";

  const std::string BlackboardKeyPlayerPos    = "pl_pos";
  const std::string BlackboardKeyLastMinedPos = "last_mined_pos";
  const std::string BlackboardKeyObjectId     = "object_id";

  const std::string MessageBoxInformationHeaderText = "Information";
  const std::string MessageBoxEpicFailHeaderText    = "Epic Fail!";
  // ---------------------------------------------------------------------------
  const char InventoryEmptySlotChar = '.';
  // ---------------------------------------------------------------------------
  const std::string ItemDefaultDescAccessory = "Could be magical or just a trinket.";
  const std::string ItemDefaultDescGem       = "Is this valuable?";
  const std::string ItemDefaultDescWeaponDmg = "You think it'll do %d damage on average.";
  const std::string ItemDefaultDescWeaponEnd = "You can't tell anything else.";
  // ---------------------------------------------------------------------------
  const std::string MsgWait              = "You waited...";
  const std::string MsgNotInWater        = "You're swimming!";
  const std::string MsgStairsDown        = "There are stairs leading down here";
  const std::string MsgStairsUp          = "There are stairs leading up here";
  const std::string MsgInventoryFull     = "Inventory is full!";
  const std::string MsgNothingHere       = "There's nothing here";
  const std::string MsgSomethingLying    = "There's something else lying here";
  const std::string MsgItemsLyingHere    = "There are several items lying here";
  const std::string MsgNoStairsDown      = "There are no stairs leading down here";
  const std::string MsgNoStairsUp        = "There are no stairs leading up here";
  const std::string MsgNotRangedWeapon   = "Not a ranged weapon!";
  const std::string MsgEquipWeapon       = "Equip a weapon first!";
  const std::string MsgWhatToShoot       = "What are you going to shoot with?";
  const std::string MsgWrongAmmo         = "Wrong ammunition type!";
  const std::string MsgNoAmmo            = "No ammunition!";
  const std::string MsgNoCharges         = "No charges left!";
  const std::string MsgCancelled         = "Cancelled";
  const std::string MsgUnequipFirst      = "Unequip first!";
  const std::string MsgLooksImportant    = "This looks important - better hold on to it";
  const std::string MsgNoRoomInInventory = "No room in inventory!";
  const std::string MsgNotEnoughMoney    = "Not enough money!";
  const std::string MsgInteractDir       = "Interact in which direction?";
  const std::string MsgItemUndamaged     = "This item looks undamaged";
  const std::string MsgCantBeUsed        = "Can't be used!";
  const std::string MsgNoMoney           = "You have no money, ha ha ha!";
  // ---------------------------------------------------------------------------
  const std::string FmtPickedUpIS        = "Picked up: %i %s";
  const std::string FmtPickedUpS         = "Picked up: %s";
  const std::string FmtDroppedIS         = "Dropped: %i %s";
  const std::string FmtDroppedS          = "Dropped: %s";
  const std::string FmtSMissed           = "%s missed";
  const std::string FmtSNotEvenScratched = "%s not even scratched!";
  // ---------------------------------------------------------------------------
  const std::vector<std::string> MsgNotInTown = { "Not here", "Not in town" };
  // ---------------------------------------------------------------------------
  const std::string SaveFileName        = "save.nrg";
  // ---------------------------------------------------------------------------
  namespace TileNames
  {
    const std::string MineWallText     = "Mine Wall";
    const std::string CaveWallText     = "Cave Wall";
    const std::string ObsidianWallText = "Obsidian Wall";
    const std::string GroundText       = "Ground";
    const std::string WallsText        = "Walls";
    const std::string FlagstoneText    = "Flagstone";
    const std::string StoneText        = "Stone";
    const std::string StoneFloorText   = "Stone Floor";
    const std::string StoneWallText    = "Stone Wall";
    const std::string StoneTilesText   = "Stone Tiles";
    const std::string StonePillarText  = "Stone Pillar";
    const std::string StainedGlassText = "Stained Glass";
    const std::string WoodenFloorText  = "Wooden Floor";
    const std::string BrickWallText    = "Brick Wall";
    const std::string WorkbenchText    = "Workbench";
    const std::string ForgeText        = "Forge";
    const std::string BedText          = "Bed";
    const std::string FountainText     = "Fountain";
    const std::string DirtText         = "Dirt";
    const std::string RocksText        = "Rocks";
    const std::string AbyssalRocksText = "Abyssal Rocks";
    const std::string AbyssalFloorText = "Abyssal Floor";
    const std::string MarbleFenceText  = "Marble Fence";
    const std::string MarbleColumnText = "Marble Column";
    const std::string DeepWaterText    = "Deep Water";
    const std::string ShallowWaterText = "Shallow Water";
    const std::string LavaText         = "Lava";
    const std::string ChasmText        = "Chasm";
    const std::string GatesText        = "Gates";
    const std::string StashText        = "Stash";
    const std::string WindowText       = "Window";
    const std::string WoodenBenchText  = "Wooden Bench";
    const std::string RoyalGatesText   = "Royal Gates";
    const std::string TreeText         = "Tree";
    const std::string WitheredTreeText = "Withered Tree";
    const std::string GrassText        = "Grass";
    const std::string BloodText        = "Blood";
    const std::string HellrockText     = "Hellrock";
    const std::string HellstoneText    = "Hellstone";
    const std::string TiledFloorText   = "Tiled Floor";
  }
  // ---------------------------------------------------------------------------
  namespace SerializationKeys
  {
    //
    // NOTE: maybe shorten everything afterwards to one letter
    // to save space and parsing time.
    //
    const std::string Root       = "save";
    const std::string Gid        = "gid";
    const std::string Seed       = "seed";
    const std::string Name       = "name";
    const std::string Value      = "value";
    const std::string Size       = "size";
    const std::string Type       = "type";
    const std::string Zone       = "zone";
    const std::string Visibility = "vis";
    const std::string Respawn    = "respawn";
    const std::string MapObjects = "mapObjects";
    const std::string Image      = "image";
    const std::string Color      = "color";
    const std::string FowName    = "fowName";
    const std::string Player     = "player";
    const std::string Border     = "border";
    const std::string Class      = "class";
    const std::string Pos        = "pos";
    const std::string Owner      = "owner";
    const std::string Ground     = "ground";
    const std::string Id         = "id";
    const std::string Mask       = "mask";
    const std::string Layout     = "layout";
    const std::string Objects    = "staticObjects";
  }
}

namespace GlobalConstants
{
  const std::unordered_map<RoomEdgeEnum, std::string> DirNamesByDir =
  {
    { RoomEdgeEnum::NORTH, "NORTH" },
    { RoomEdgeEnum::EAST,  "EAST"  },
    { RoomEdgeEnum::SOUTH, "SOUTH" },
    { RoomEdgeEnum::WEST,  "WEST"  }
  };

  const std::unordered_map<PlayerClass, std::string> PlayerClassNameByType =
  {
    { PlayerClass::SOLDIER,  "Soldier"  },
    { PlayerClass::THIEF,    "Thief"    },
    { PlayerClass::ARCANIST, "Arcanist" },
    { PlayerClass::CUSTOM,   "Custom"   },
  };

#ifdef USE_SDL
  //
  // To avoid C-style casting from enum
  //
  std::unordered_map<NameCP437, int> CP437IndexByType;
#endif

  int TerminalWidth  = 80;
  int TerminalHeight = 25;

  const size_t OpenedByAnyone = 0;
  const size_t OpenedByNobody = 1;

  const size_t InventoryMaxNameLength   = 20;

  const int HPMPBarLength               = 20;
  const int AttributeMinimumRaiseChance = 5;
  const int AttributeIncreasedRaiseStep = 25;
  const int AwardedExperienceStep       = 5;
  const int AwardedExpMiniboss          = 10;
  const int AwardedExpBoss              = 20;
  const int AwardedExpMax               = 40;
  const int MinHitChance                = 1;
  const int MaxHitChance                = 99;
  const int DisplayAttackDelayMs        = 50;
  const int InventoryMaxSize            = 20;
  const int MaxNameLength               = 21;
  const int MaxSeedStringLength         = 65;

  //
  // Default terminal window is 80x25,
  // so to overcompensate assume 40x40
  //
  const int MaxVisibilityRadius = 160;

  //
  // Character at level 20 will have his 3 star stat around 20.
  // So for "ready" amount of energy half of that value was chosen.
  // Tick value is 2 because it is a multiple of 10.
  // (no special reasoning apart from that,
  // but what if that will come in handy someday?)
  //
  const int TurnReadyValue = 10;
  const int TurnTickValue  = 2;

  //
  // 11 is because if < StarvationDamageTimeout
  //
  const int StarvationDamageTimeout = 11;

  //
  // Default number of turn passes
  // after which monster is attepmted to be spawned.
  //
  const int MonstersRespawnTimeout = 250;

  //
  // Also the number of action increments (end turns)
  // for player before he can act (for SPD = 0)
  //
  const int EffectDurationSkipsForTurn = ((double)TurnReadyValue / (double)TurnTickValue) + 1;

  //
  // Default duration is 20 SPD=0 player end turns.
  //
  const int EffectDefaultDuration = EffectDurationSkipsForTurn * 20;

  //
  // std::map is sorted by key.
  //
  const std::map<int, std::pair<StatsEnum, std::string>> AllStatNames =
  {
    { 0, { StatsEnum::STR, "STR" } },
    { 1, { StatsEnum::DEF, "DEF" } },
    { 2, { StatsEnum::MAG, "MAG" } },
    { 3, { StatsEnum::RES, "RES" } },
    { 4, { StatsEnum::SKL, "SKL" } },
    { 5, { StatsEnum::SPD, "SPD" } }
  };

  const std::unordered_map<ItemBonusType, std::string> BonusNameByType =
  {
    { ItemBonusType::STR, "STR" },
    { ItemBonusType::DEF, "DEF" },
    { ItemBonusType::MAG, "MAG" },
    { ItemBonusType::RES, "RES" },
    { ItemBonusType::SPD, "SPD" },
    { ItemBonusType::SKL, "SKL" },
    { ItemBonusType::HP,  "HP"  },
    { ItemBonusType::MP,  "MP"  }
  };

  const std::unordered_map<ItemQuality, std::string> QualityNameByQuality =
  {
    { ItemQuality::DAMAGED,     "Damaged"     },
    { ItemQuality::FLAWED,      "Flawed"      },
    { ItemQuality::NORMAL,      ""            },
    { ItemQuality::FINE,        "Fine"        },
    { ItemQuality::EXCEPTIONAL, "Exceptional" }
  };

  const std::unordered_map<PlayerSkills, std::string> SkillNameByType =
  {
    { PlayerSkills::REPAIR,       "Repair"       },
    { PlayerSkills::RECHARGE,     "Recharge"     },
    { PlayerSkills::SPELLCASTING, "Spellcasting" },
    { PlayerSkills::AWARENESS,    "Awareness"    }
  };

  const std::unordered_map<TraderRole, std::string> ShopNameByType =
  {
    { TraderRole::BLACKSMITH, "Armory"    },
    { TraderRole::CLERIC,     "Sanctuary" },
    { TraderRole::COOK,       "Grocery"   },
    { TraderRole::JUNKER,     "Junkyard"  }
  };

  const std::unordered_map<PotionType, int> PotionsWeightTable =
  {
    { PotionType::HEALING_POTION, 50 },
    { PotionType::MANA_POTION,    50 },
    { PotionType::NP_POTION,      50 },
    { PotionType::JUICE_POTION,   20 },
    { PotionType::STR_POTION,      1 },
    { PotionType::DEF_POTION,      1 },
    { PotionType::MAG_POTION,      1 },
    { PotionType::RES_POTION,      1 },
    { PotionType::SKL_POTION,      1 },
    { PotionType::SPD_POTION,      1 },
    { PotionType::EXP_POTION,      5 },
    { PotionType::CW_POTION,       5 },
    { PotionType::RA_POTION,       5 }
  };

  const std::unordered_map<PlayerStats, std::string> StatNameByType =
  {
    { PlayerStats::STR, "STR" },
    { PlayerStats::DEF, "DEF" },
    { PlayerStats::MAG, "MAG" },
    { PlayerStats::RES, "RES" },
    { PlayerStats::SKL, "SKL" },
    { PlayerStats::SPD, "SPD" },
    { PlayerStats::HP,  "HP"  },
    { PlayerStats::MP,  "MP"  }
  };

  const std::unordered_map<PotionType, std::string> PotionNameByType =
  {
    { PotionType::HEALING_POTION, "Healing Potion"    },
    { PotionType::MANA_POTION,    "Mana Potion"       },
    { PotionType::JUICE_POTION,   "Fruit Juice"       },
    { PotionType::NP_POTION,      "Neutralize Poison" },
    { PotionType::STR_POTION,     "STR Potion"        },
    { PotionType::DEF_POTION,     "DEF Potion"        },
    { PotionType::MAG_POTION,     "MAG Potion"        },
    { PotionType::RES_POTION,     "RES Potion"        },
    { PotionType::SKL_POTION,     "SKL Potion"        },
    { PotionType::SPD_POTION,     "SPD Potion"        },
    { PotionType::EXP_POTION,     "EXP Potion"        },
    { PotionType::RA_POTION,      "Restore Ability"   },
    { PotionType::CW_POTION,      "Cure Weakness"     }
  };

  const std::unordered_map<PotionType, std::string> StatNameByPotionType =
  {
    { PotionType::STR_POTION, "STR" },
    { PotionType::DEF_POTION, "DEF" },
    { PotionType::MAG_POTION, "MAG" },
    { PotionType::RES_POTION, "RES" },
    { PotionType::SKL_POTION, "SKL" },
    { PotionType::SPD_POTION, "SPD" },
  };

  const std::unordered_map<WeaponType, std::string> WeaponNameByType =
  {
    { WeaponType::DAGGER,       "Dagger"         },
    { WeaponType::SHORT_SWORD,  "Short Sword"    },
    { WeaponType::ARMING_SWORD, "Arming Sword"   },
    { WeaponType::LONG_SWORD,   "Longsword"      },
    { WeaponType::GREAT_SWORD,  "Great Sword"    },
    { WeaponType::STAFF,        "Battle Staff"   },
    { WeaponType::PICKAXE,      "Pickaxe"        }
  };

  const std::unordered_map<RangedWeaponType, std::string> RangedWeaponNameByType =
  {
    { RangedWeaponType::SHORT_BOW,  "Short Bow"   },
    { RangedWeaponType::LONGBOW,    "Longbow"     },
    { RangedWeaponType::WAR_BOW,    "War Bow"     },
    { RangedWeaponType::LIGHT_XBOW, "L. Crossbow" },
    { RangedWeaponType::XBOW,       "Crossbow"    },
    { RangedWeaponType::HEAVY_XBOW, "H. Crossbow" }
  };

  const std::unordered_map<ArrowType, std::string> ArrowNameByType =
  {
    { ArrowType::ARROWS, "Arrows" },
    { ArrowType::BOLTS,  "Bolts"  }
  };

  const std::unordered_map<ArmorType, std::string> ArmorNameByType =
  {
    { ArmorType::PADDING, "Padded Surcoat" },
    { ArmorType::LEATHER, "Leather Jacket" },
    { ArmorType::MAIL,    "Mail Hauberk"   },
    { ArmorType::SCALE,   "Scale Armor"    },
    { ArmorType::PLATE,   "Coat of Plates" }
  };

  const std::unordered_map<ArmorType, int> ArmorDurabilityByType =
  {
    { ArmorType::PADDING, 25 },
    { ArmorType::LEATHER, 50 },
    { ArmorType::MAIL,    80 },
    { ArmorType::SCALE,  120 },
    { ArmorType::PLATE,  180 }
  };

  const std::unordered_map<FoodType, std::pair<std::string, int>> FoodHungerPercentageByName =
  {
    { FoodType::APPLE,        { "Apple",         10 } },
    { FoodType::BREAD,        { "Bread",         20 } },
    { FoodType::FISH,         { "Fish",          20 } },
    { FoodType::CHEESE,       { "Cheese",        30 } },
    { FoodType::PIE,          { "Cream Pie",     40 } },
    { FoodType::MEAT,         { "Meat",          50 } },
    { FoodType::TIN,          { "Canned Food",   60 } },
    { FoodType::RATIONS,      { "Rations",       75 } },
    { FoodType::IRON_RATIONS, { "Iron Rations", 100 } }
  };

  const std::unordered_map<GemType, std::string> GemNameByType =
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

  const std::unordered_map<GemType, StringV> GemDescriptionByType =
  {
    {
      GemType::BLACK_JETSTONE,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Jetstone is an organic rock created ",
        "when pieces of woody material are buried,",
        "compacted, and then go through organic degradation.",
        "It can be easily cut or carved, and it has a uniform texture",
        "that makes it possible to carve with accuracy.",
      }
    },
    {
      GemType::BLACK_OBSIDIAN,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Obsidian is a naturally occurring volcanic glass",
        "formed when lava extruded from a volcano cools rapidly",
        "with minimal crystal growth. It is an igneous rock."
      }
    },
    {
      GemType::BLUE_AQUAMARINE,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Aquamarine is a pale-blue to light-green variety of beryl.",
        "The color of aquamarine can be changed by heat."
      }
    },
    {
      GemType::BLUE_SAPPHIRE,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Sapphire is a gemstone associated with royalty.",
        "It is said to be the wisdom stone,",
        "stimulating concentration, enhancing creativity",
        "and promoting purity and depth of thought."
      }
    },
    {
      GemType::GREEN_EMERALD,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Emerald is the bluish green to green variety of beryl,",
        "a mineral species that includes aquamarine.",
        "Highly saturated green color defines high quality emerald."
      }
    },
    {
      GemType::GREEN_JADE,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Jade is a type of gemstone that refers to",
        "two different mineral stones that are typically tough,",
        "durable, compact, and green. Both jadeite and nephrite",
        "have inspired beautiful works of art and a rich body of lore."
      }
    },
    {
      GemType::ORANGE_AMBER,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Amber is considered a gem because it glows and glistens",
        "when polished, but it is not actually a gemstone.",
        "It is the hardened resin of certain types of ancient trees",
        "that have been fossilized over millennia."
      }
    },
    {
      GemType::ORANGE_JACINTH,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Jacinth gemstone, often known as hyacinth, is a semi-precious stone.",
        "It is a gleaming orange-yellow, orange-red, or yellow-brown zircon.",
        "Jacinth is beneficial in the treatment of insomnia.",
        "Encourages spiritual awareness and comprehension."
      }
    },
    {
      GemType::PURPLE_AMETHYST,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Amethyst is a violet variety of quartz.",
        "Cross-culturally, this popular gemstone was used",
        "as a symbol of peace and unification."
      }
    },
    {
      GemType::PURPLE_FLUORITE,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Purple Fluorite is a light to very dark grape-purple crystal",
        "ranging from translucent to opaque. It has a glassy luster",
        "and often forms as isometric cubes, nodules, or clusters.",
        "Purple Fluorite enhances intuition and brings order out of chaos."
      }
    },
    {
      GemType::RED_GARNET,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Garnets come in a variety of colors and have many different varieties.",
        "However, the most widely-known color of Garnet gemstones is dark red.",
        "Red Garnet Gems have always been associated with journeys",
        "and were carried by travelers, traders and journeymen."
      }
    },
    {
      GemType::RED_RUBY,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "A ruby is a pinkish red to blood-red colored gemstone,",
        "a variety of the mineral corundum.",
        "Ruby is one of the most popular traditional jewelry gems",
        "and is very durable. Other varieties of gem-quality corundum",
        "are called sapphires."
      }
    },
    {
      GemType::WHITE_DIAMOND,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Diamond, a mineral composed of pure carbon.",
        "It is the hardest naturally occurring substance known.",
        "It is also the most popular gemstone.",
        "Because of their extreme hardness, ",
        "diamonds have a number of important industrial applications."
      }
    },
    {
      GemType::WHITE_OPAL,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "An amorphous non-crystalline gem mineral",
        "solidified from gelatinous or liquid silica",
        "deposited in cracks and cavities left by",
        "decaying vegetation, wood, crustaceans and bones."
      }
    },
    {
      GemType::YELLOW_CITRINE,
      {
      // ---------1---------2---------3---------4---------5---------6---------7---------8
        "Citrine is a semiprecious gem that is valued for its yellow",
        "to brownish colour and its resemblance to the rarer topaz.",
        "It is a popular gemstone that has been used for centuries",
        "as an adornment and symbol of wealth due to its association with gold."
      }
    }
  };

  const std::unordered_map<GemType, int> GemCostByType =
  {
    { GemType::WORTHLESS_GLASS,    0 },
    { GemType::BLACK_OBSIDIAN,   100 },
    { GemType::BLACK_JETSTONE,   125 },
    { GemType::YELLOW_CITRINE,   150 },
    { GemType::ORANGE_AMBER,     200 },
    { GemType::PURPLE_AMETHYST,  225 },
    { GemType::RED_GARNET,       250 },
    { GemType::GREEN_JADE,       400 },
    { GemType::BLUE_AQUAMARINE,  425 },
    { GemType::PURPLE_FLUORITE,  450 },
    { GemType::BLUE_SAPPHIRE,    800 },
    { GemType::GREEN_EMERALD,    825 },
    { GemType::WHITE_OPAL,       850 },
    { GemType::ORANGE_JACINTH,  1000 },
    { GemType::RED_RUBY,        1250 },
    { GemType::WHITE_DIAMOND,   1500 }
  };

  const std::unordered_map<ItemQuality, std::string> GemRatingByQuality =
  {
    { ItemQuality::DAMAGED,     " --" },
    { ItemQuality::FLAWED,      " -"  },
    { ItemQuality::NORMAL,      ""    },
    { ItemQuality::FINE,        " +"  },
    { ItemQuality::EXCEPTIONAL, " ++" },
  };

  const std::unordered_map<WandMaterials, int> WandCapacityByMaterial =
  {
    { WandMaterials::YEW_1,    100 },
    { WandMaterials::IVORY_2,  150 },
    { WandMaterials::EBONY_3,  200 },
    { WandMaterials::ONYX_4,   300 },
    { WandMaterials::GLASS_5,  450 },
    { WandMaterials::COPPER_6, 600 },
    { WandMaterials::GOLDEN_7, 800 }
  };

  //
  // Divide wand capacity by this value to get amount of charges.
  // Total amount of charges is affected by material, BUC status and RNG god.
  // (see GameObjectsFactory::CreateWand())
  //
  const std::unordered_map<SpellType, int> WandSpellCapacityCostByType =
  {
    { SpellType::NONE,          100 },
    { SpellType::LIGHT,         50  },
    { SpellType::STRIKE,        60  },
    { SpellType::FROST,         80  },
    { SpellType::TELEPORT,      75  },
    { SpellType::FIREBALL,      180 },
    { SpellType::LASER,         180 },
    { SpellType::LIGHTNING,     150 },
    { SpellType::MAGIC_MISSILE, 70  }
  };

  const std::unordered_map<WandMaterials, int> WandRangeByMaterial =
  {
    { WandMaterials::YEW_1,     5 },
    { WandMaterials::IVORY_2,  10 },
    { WandMaterials::EBONY_3,  15 },
    { WandMaterials::ONYX_4,   20 },
    { WandMaterials::GLASS_5,  25 },
    { WandMaterials::COPPER_6, 30 },
    { WandMaterials::GOLDEN_7, 35 }
  };

  //
  // Don't forget to add a new entry in the
  // ScrollUnidentifiedNames vector for each new spell.
  //
  const std::vector<SpellType> ScrollValidSpellTypes =
  {
    SpellType::LIGHT,
    SpellType::IDENTIFY,
    SpellType::MAGIC_MAPPING,
    SpellType::TELEPORT,
    SpellType::DETECT_MONSTERS,
    SpellType::TRUE_SEEING,
    SpellType::REMOVE_CURSE,
    SpellType::REPAIR,
    SpellType::TOWN_PORTAL,
    SpellType::HEAL,
    SpellType::NEUTRALIZE_POISON,
    SpellType::MANA_SHIELD
  };

  //
  // Not including combat spells.
  //
  // Two separate vectors are because unidentified names
  // can be different scroll every time you run the game.
  //
  const std::vector<std::string> ScrollUnidentifiedNames =
  {
    "TAMAM SHUD",
    "GACHIMUCHI",
    "XYETA",
    "ZA WARUDO",
    "ZELGO MER",
    "LOREM IPSUM",
    "GHOTI",
    "LALIHO",
    "SHAN DONG",
    "SUIL A RUIN",
    "KOBAYASHI MARU",
    "LLORCS"
  };

  const std::unordered_map<SpellType, std::string> SpellShortNameByType =
  {
    { SpellType::NONE,              "-"  },
    { SpellType::LIGHT,             "L"  },
    { SpellType::STRIKE,            "S"  },
    { SpellType::FROST,             "F"  },
    { SpellType::FIREBALL,          "Fl" },
    { SpellType::LASER,             "P"  },
    { SpellType::LIGHTNING,         "Lg" },
    { SpellType::MAGIC_MISSILE,     "Mg" },
    { SpellType::IDENTIFY,          "Id" },
    { SpellType::MAGIC_MAPPING,     "MM" },
    { SpellType::TELEPORT,          "Te" },
    { SpellType::TOWN_PORTAL,       "TP" },
    { SpellType::DETECT_MONSTERS,   "DM" },
    { SpellType::TRUE_SEEING,       "TS" },
    { SpellType::REMOVE_CURSE,      "RC" },
    { SpellType::REPAIR,            "R"  },
    { SpellType::HEAL,              "H"  },
    { SpellType::NEUTRALIZE_POISON, "NP" },
    { SpellType::MANA_SHIELD,       "MS" }
  };

  const std::unordered_map<WandMaterials, std::string> WandMaterialNamesByMaterial =
  {
    { WandMaterials::YEW_1,    "Yew"    },
    { WandMaterials::IVORY_2,  "Ivory"  },
    { WandMaterials::EBONY_3,  "Ebony"  },
    { WandMaterials::ONYX_4,   "Onyx"   },
    { WandMaterials::GLASS_5,  "Glass"  },
    { WandMaterials::COPPER_6, "Copper" },
    { WandMaterials::GOLDEN_7, "Golden" }
  };

  const std::unordered_map<ShrineType, std::string> ShrineSaintByType =
  {
    { ShrineType::MIGHT,      "St. George the Soldier"     },
    { ShrineType::SPIRIT,     "St. Mary the Mother"        },
    { ShrineType::KNOWLEDGE,  "St. Nestor the Scribe"      },
    { ShrineType::PERCEPTION, "St. Justin the Philosopher" },
    { ShrineType::HEALING,    "St. Luke the Healer"        }
  };

  const std::unordered_map<ShrineType, std::string> ShrineNameByType =
  {
    { ShrineType::MIGHT,       "Shrine of Might"       },
    { ShrineType::SPIRIT,      "Shrine of Spirit"      },
    { ShrineType::TRANQUILITY, "Shrine of Tranquility" },
    { ShrineType::KNOWLEDGE,   "Shrine of Knowledge"   },
    { ShrineType::PERCEPTION,  "Shrine of Perception"  },
    { ShrineType::HEALING,     "Shrine of Healing"     },
    { ShrineType::FORGOTTEN,   "Forgotten Shrine"      },
    { ShrineType::ABYSSAL,     "Abyssal Shrine"        },
    { ShrineType::DESECRATED,  "Desecrated Shrine"     },
    { ShrineType::DISTURBING,  "Disturbing Shrine"     },
    { ShrineType::RUINED,      "Ruined Shrine"         },
    { ShrineType::POTENTIAL,   "Shrine of Potential"   },
    { ShrineType::HIDDEN,      "Hidden Shrine"         },
    { ShrineType::HOLY,        "Holy Shrine"           }
  };

  const std::vector<std::string> RandomNames =
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

  const std::vector<std::string> TownNameEndings =
  {
    "ir",
    "ire",
    "yr",
    "yre",
    "no",
    "ow",
    "to",
    "nd",
    "nt",
    "sh",
    "rd",
    "rn",
    "rsh",
    "urgh",
    "urg",
    "stan",
    "heim",
    "town",
    "on",
    "ton",
    "shire",
    "pool",
    "glade",
    "bury",
    "gow"
  };

  const std::vector<std::string> NameEndings =
  {
    "d",
    "t",
    "r",
    "rt",
    "na",
    "ko",
    "ya",
    "in",
    "an",
    "in",
    "to",
    "om",
    "mo",
    "mi",
    "el"
  };

  const std::unordered_map<MapType, std::string> MapLevelNames =
  {
    // -------------------------------------------------------------------------
    // Abandoned Mines
    // -------------------------------------------------------------------------
    { MapType::MINES_1,     { HIDE("Mine Entrance")      } },
    { MapType::MINES_2,     { HIDE("Forsaken Prospects") } },
    { MapType::MINES_3,     { HIDE("Abandoned Mines")    } },
    { MapType::MINES_4,     { HIDE("Deep Mines")         } },
    { MapType::MINES_5,     { HIDE("Sealed Chamber")     } },
    // -------------------------------------------------------------------------
    // Caves of Circe
    // -------------------------------------------------------------------------
    { MapType::CAVES_1,     { HIDE("Catacombs")         } },
    { MapType::CAVES_2,     { HIDE("Caves of Circe")    } },
    { MapType::CAVES_3,     { HIDE("Corridors of Time") } },
    { MapType::CAVES_4,     { HIDE("Windy Tunnels")     } },
    { MapType::CAVES_5,     { HIDE("Vertigo")           } },
    // -------------------------------------------------------------------------
    // Lost City
    // -------------------------------------------------------------------------
    { MapType::LOST_CITY,   { HIDE("Lost City") } },
    // -------------------------------------------------------------------------
    // Deep Dark
    // -------------------------------------------------------------------------
    { MapType::DEEP_DARK_1, { HIDE("Underdark")           } },
    { MapType::DEEP_DARK_2, { HIDE("Embrace of Darkness") } },
    { MapType::DEEP_DARK_3, { HIDE("Deep Dark")           } },
    { MapType::DEEP_DARK_4, { HIDE("Starless Night")      } },
    { MapType::DEEP_DARK_5, { HIDE("Sinister")            } },
    // -------------------------------------------------------------------------
    // Stygian Abyss
    // -------------------------------------------------------------------------
    { MapType::ABYSS_1,     { HIDE("Plains of Desolation") } },
    { MapType::ABYSS_2,     { HIDE("Grey Wastes")          } },
    { MapType::ABYSS_3,     { HIDE("Soul Pastures")        } },
    { MapType::ABYSS_4,     { HIDE("Stygian Abyss")        } },
    { MapType::ABYSS_5,     { HIDE("Hell's Maw")           } },
    // -------------------------------------------------------------------------
    // Nether
    // -------------------------------------------------------------------------
    { MapType::NETHER_1,    { HIDE("Blazing Gates") } },
    { MapType::NETHER_2,    { HIDE("River of Fire") } },
    { MapType::NETHER_3,    { HIDE("Red Wastes")    } },
    { MapType::NETHER_4,    { HIDE("Citadel")       } },
    { MapType::NETHER_5,    { HIDE("The Hearth")    } },
    // -------------------------------------------------------------------------
    // The End
    // -------------------------------------------------------------------------
    { MapType::THE_END,     { "???" } },
#ifdef BUILD_TESTS
    // -------------------------------------------------------------------------
    // Test level
    // -------------------------------------------------------------------------
    { MapType::TEST_LEVEL, { "Test Level" } },
#endif
  };

  const std::unordered_map<ItemBonusType, std::string> ItemBonusPrefixes =
  {
    { ItemBonusType::STR,            { HIDE("Heavy")       } },
    { ItemBonusType::DEF,            { HIDE("Rampart")     } },
    { ItemBonusType::MAG,            { HIDE("Magical")     } },
    { ItemBonusType::RES,            { HIDE("Electrum")    } },
    { ItemBonusType::SKL,            { HIDE("Expert")      } },
    { ItemBonusType::SPD,            { HIDE("Swift")       } },
    { ItemBonusType::HP,             { HIDE("Vital")       } },
    { ItemBonusType::MP,             { HIDE("Soulful")     } },
    { ItemBonusType::INDESTRUCTIBLE, { HIDE("Everlasting") } },
    { ItemBonusType::SELF_REPAIR,    { HIDE("Reliable")    } },
    { ItemBonusType::VISIBILITY,     { HIDE("Shining")     } },
    { ItemBonusType::INVISIBILITY,   { HIDE("Concealing")  } },
    { ItemBonusType::DAMAGE,         { HIDE("Deadly")      } },
    { ItemBonusType::REMOVE_HUNGER,  { HIDE("Sustaining")  } },
    { ItemBonusType::FREE_ACTION,    { HIDE("Unbound")     } },
    { ItemBonusType::POISON_IMMUNE,  { HIDE("Viridian")    } },
    { ItemBonusType::IGNORE_DEFENCE, { HIDE("Defiant")     } },
    { ItemBonusType::IGNORE_ARMOR,   { HIDE("Piercing")    } },
    { ItemBonusType::KNOCKBACK,      { HIDE("Mighty")      } },
    { ItemBonusType::MANA_SHIELD,    { HIDE("Spiritual")   } },
    { ItemBonusType::REGEN,          { HIDE("Restorative") } },
    { ItemBonusType::REFLECT,        { HIDE("Silver")      } },
    { ItemBonusType::LEECH,          { HIDE("Vampiric")    } },
    { ItemBonusType::DMG_ABSORB,     { HIDE("Protective")  } },
    { ItemBonusType::MAG_ABSORB,     { HIDE("Shielding")   } },
    { ItemBonusType::THORNS,         { HIDE("Spiked")      } },
    { ItemBonusType::TELEPATHY,      { HIDE("Cautious")    } },
    { ItemBonusType::TRUE_SEEING,    { HIDE("Clear")       } },
    { ItemBonusType::LEVITATION,     { HIDE("Celestial")   } }
  };

  const std::unordered_map<ItemBonusType, std::string> ItemBonusSuffixes =
  {
    { ItemBonusType::STR,            { HIDE("of Strength")    } },
    { ItemBonusType::DEF,            { HIDE("of Defence")     } },
    { ItemBonusType::MAG,            { HIDE("of Magic")       } },
    { ItemBonusType::RES,            { HIDE("of Resistance")  } },
    { ItemBonusType::SKL,            { HIDE("of Skill")       } },
    { ItemBonusType::SPD,            { HIDE("of Speed")       } },
    { ItemBonusType::HP,             { HIDE("of Life")        } },
    { ItemBonusType::MP,             { HIDE("of Mana")        } },
    { ItemBonusType::INDESTRUCTIBLE, { HIDE("of Ages")        } },
    { ItemBonusType::SELF_REPAIR,    { HIDE("of Reliability") } },
    { ItemBonusType::VISIBILITY,     { HIDE("of the Sun")     } },
    { ItemBonusType::INVISIBILITY,   { HIDE("of the Unseen")  } },
    { ItemBonusType::DAMAGE,         { HIDE("of Destruction") } },
    { ItemBonusType::REMOVE_HUNGER,  { HIDE("of Satiation")   } },
    { ItemBonusType::FREE_ACTION,    { HIDE("of Free Action") } },
    { ItemBonusType::POISON_IMMUNE,  { HIDE("of Cleansing")   } },
    { ItemBonusType::IGNORE_DEFENCE, { HIDE("of the Master")  } },
    { ItemBonusType::IGNORE_ARMOR,   { HIDE("of the Awl")     } },
    { ItemBonusType::KNOCKBACK,      { HIDE("of the Bear")    } },
    { ItemBonusType::MANA_SHIELD,    { HIDE("of the Force")   } },
    { ItemBonusType::REGEN,          { HIDE("of the Undying") } },
    { ItemBonusType::REFLECT,        { HIDE("of Reflection")  } },
    { ItemBonusType::LEECH,          { HIDE("of the Blood")   } },
    { ItemBonusType::DMG_ABSORB,     { HIDE("of Protection")  } },
    { ItemBonusType::MAG_ABSORB,     { HIDE("of Shielding")   } },
    { ItemBonusType::THORNS,         { HIDE("of the Ivy")     } },
    { ItemBonusType::TELEPATHY,      { HIDE("of the Oracle")  } },
    { ItemBonusType::TRUE_SEEING,    { HIDE("of True Seeing") } },
    { ItemBonusType::LEVITATION,     { HIDE("of the Angel")   } }
  };

  const std::unordered_map<ItemBonusType, int> MoneyCostIncreaseByBonusType =
  {
    { ItemBonusType::NONE,              0 },
    { ItemBonusType::STR,             200 },
    { ItemBonusType::DEF,             200 },
    { ItemBonusType::MAG,             200 },
    { ItemBonusType::RES,             200 },
    { ItemBonusType::SKL,             200 },
    { ItemBonusType::SPD,             200 },
    { ItemBonusType::HP,              100 },
    { ItemBonusType::MP,              100 },
    { ItemBonusType::INDESTRUCTIBLE, 5000 },
    { ItemBonusType::SELF_REPAIR,     500 },
    { ItemBonusType::VISIBILITY,       50 },
    { ItemBonusType::INVISIBILITY,   2000 },
    { ItemBonusType::DAMAGE,          300 },
    { ItemBonusType::REMOVE_HUNGER,   500 },
    { ItemBonusType::FREE_ACTION,     750 },
    { ItemBonusType::POISON_IMMUNE,   750 },
    { ItemBonusType::IGNORE_DEFENCE,  750 },
    { ItemBonusType::IGNORE_ARMOR,    650 },
    { ItemBonusType::KNOCKBACK,       100 },
    { ItemBonusType::MANA_SHIELD,     500 },
    { ItemBonusType::REGEN,          1000 },
    { ItemBonusType::REFLECT,        1000 },
    { ItemBonusType::LEECH,            20 },
    { ItemBonusType::DMG_ABSORB,      450 },
    { ItemBonusType::MAG_ABSORB,      450 },
    { ItemBonusType::THORNS,           40 },
    { ItemBonusType::TELEPATHY,       250 },
    { ItemBonusType::TRUE_SEEING,     350 },
    { ItemBonusType::LEVITATION,      500 }
  };

  const std::unordered_map<ItemBonusType, std::string> BonusDisplayNameByType =
  {
    { ItemBonusType::STR,          "+ST" },
    { ItemBonusType::DEF,          "+DF" },
    { ItemBonusType::MAG,          "+MG" },
    { ItemBonusType::RES,          "+RS" },
    { ItemBonusType::SKL,          "+SK" },
    { ItemBonusType::SPD,          "+SP" },
    { ItemBonusType::INVISIBILITY, "Hid" },
    { ItemBonusType::MANA_SHIELD,  "Shi" },
    { ItemBonusType::REGEN,        "Reg" },
    { ItemBonusType::REFLECT,      "Ref" },
    { ItemBonusType::DMG_ABSORB,   "PAb" },
    { ItemBonusType::MAG_ABSORB,   "MAb" },
    { ItemBonusType::THORNS,       "Ths" },
    { ItemBonusType::PARALYZE,     "Par" },
    { ItemBonusType::TELEPATHY,    "Tel" },
    { ItemBonusType::TRUE_SEEING,  "See" },
    { ItemBonusType::LEVITATION,   "Fly" },
    { ItemBonusType::BLINDNESS,    "Bli" },
    { ItemBonusType::FROZEN,       "Frz" },
    { ItemBonusType::BURNING,      "Bur" },
    { ItemBonusType::ILLUMINATED,  "Lgt" },
    { ItemBonusType::POISONED,     "Psd" },
    { ItemBonusType::WEAKNESS,     "Wea" }
  };

  const std::unordered_map<std::string, ScriptTaskNames> BTSTaskNamesByName =
  {
    { "TREE", ScriptTaskNames::TREE },
    { "SEL",  ScriptTaskNames::SEL  },
    { "SEQ",  ScriptTaskNames::SEQ  },
    { "FAIL", ScriptTaskNames::FAIL },
    { "SUCC", ScriptTaskNames::SUCC },
    { "TASK", ScriptTaskNames::TASK },
    { "COND", ScriptTaskNames::COND }
  };

  const std::unordered_map<std::string, ScriptParamNames> BTSParamNamesByName =
  {
    { "idle",                 ScriptParamNames::IDLE                 },
    { "move_rnd",             ScriptParamNames::MOVE_RND             },
    { "move_smart",           ScriptParamNames::MOVE_SMART           },
    { "move_away",            ScriptParamNames::MOVE_AWAY            },
    { "attack",               ScriptParamNames::ATTACK               },
    { "attack_ranged",        ScriptParamNames::ATTACK_RANGED        },
    { "break_stuff",          ScriptParamNames::BREAK_STUFF          },
    { "pick_items",           ScriptParamNames::PICK_ITEMS           },
    { "chase_player",         ScriptParamNames::CHASE_PLAYER         },
    { "save_player_pos",      ScriptParamNames::SAVE_PLAYER_POS      },
    { "goto_last_player_pos", ScriptParamNames::GOTO_LAST_PLAYER_POS },
    { "goto_last_mined_pos",  ScriptParamNames::GOTO_LAST_MINED_POS  },
    { "mine_tunnel",          ScriptParamNames::MINE_TUNNEL          },
    { "mine_block",           ScriptParamNames::MINE_BLOCK           },
    { "apply_effect",         ScriptParamNames::APPLY_EFFECT         },
    { "drink_potion",         ScriptParamNames::DRINK_POTION         },
    { "print_message",        ScriptParamNames::PRINT_MESSAGE        },
    { "end",                  ScriptParamNames::END                  },
    { "d100",                 ScriptParamNames::D100                 },
    { "player_visible",       ScriptParamNames::PLAYER_VISIBLE       },
    { "player_can_move",      ScriptParamNames::PLAYER_CAN_MOVE      },
    { "player_in_range",      ScriptParamNames::PLAYER_IN_RANGE      },
    { "player_energy",        ScriptParamNames::PLAYER_ENERGY        },
    { "player_next_turn",     ScriptParamNames::PLAYER_NEXT_TURN     },
    { "turns_left",           ScriptParamNames::TURNS_LEFT           },
    { "turns_check",          ScriptParamNames::TURNS_CHECK          },
    { "has_effect",           ScriptParamNames::HAS_EFFECT           },
    { "has_equipped",         ScriptParamNames::HAS_EQUIPPED         },
    { "hp_low",               ScriptParamNames::HP_LOW               },
    { "eq",                   ScriptParamNames::EQ                   },
    { "ge",                   ScriptParamNames::GE                   },
    { "le",                   ScriptParamNames::LE                   },
    { "gt",                   ScriptParamNames::GT                   },
    { "lt",                   ScriptParamNames::LT                   },
    { "player",               ScriptParamNames::PLAYER               },
    { "self",                 ScriptParamNames::SELF                 },
    { "HEA",                  ScriptParamNames::HEA                  },
    { "NCK",                  ScriptParamNames::NCK                  },
    { "TRS",                  ScriptParamNames::TRS                  },
    { "BTS",                  ScriptParamNames::BTS                  },
    { "MAG",                  ScriptParamNames::MAG                  },
    { "WPN",                  ScriptParamNames::WPN                  },
    { "SLD",                  ScriptParamNames::SLD                  },
    { "RNG",                  ScriptParamNames::RNG                  },
    { "-",                    ScriptParamNames::ANY                  },
    { "HP",                   ScriptParamNames::HP                   },
    { "MP",                   ScriptParamNames::MP                   },
    { "Hid",                  ScriptParamNames::HID                  },
    { "Shi",                  ScriptParamNames::SHI                  },
    { "Reg",                  ScriptParamNames::REG                  },
    { "Ref",                  ScriptParamNames::REF                  },
    { "PAb",                  ScriptParamNames::PAB                  },
    { "MAb",                  ScriptParamNames::MAB                  },
    { "Ths",                  ScriptParamNames::THS                  },
    { "Par",                  ScriptParamNames::PAR                  },
    { "Tel",                  ScriptParamNames::TEL                  },
    { "Fly",                  ScriptParamNames::FLY                  },
    { "Bli",                  ScriptParamNames::BLI                  },
    { "Frz",                  ScriptParamNames::FRZ                  },
    { "Bur",                  ScriptParamNames::BUR                  },
    { "Lgt",                  ScriptParamNames::LGT                  },
    { "Psd",                  ScriptParamNames::PSD                  },
    { "nop",                  ScriptParamNames::NOP                  },
  };

  //
  // Different game levels may create different themed
  // tiles in place of '.' or ' '
  //
  const std::vector<StringV> ShrineLayouts =
  {
    // 0
    //
    // Might, Spirit, Knowledge, Holy,
    // Healing, Potential, Tranquility, Perception
    //
    {
      { HIDE("##.##") },
      { HIDE("#...#") },
      { HIDE("../..") },
      { HIDE("#...#") },
      { HIDE("##.##") }
    },
    // 1
    //
    // Holy, Healing, Potential, Tranquility, Perception
    //
    {
      { HIDE("wg gw") },
      { HIDE("gg gg") },
      { HIDE("  /  ") },
      { HIDE("gg gg") },
      { HIDE("wg gw") }
    },
    // 2
    //
    // Holy, Healing, Potential, Tranquility, Perception
    //
    {
      { HIDE("ww1ww") },
      { HIDE("w121w") },
      { HIDE("12/21") },
      { HIDE("w121w") },
      { HIDE("ww1ww") }
    },
    // 3
    //
    // Forgotten, Hidden, Ruined, Desecrated, Disturbing
    //
    {
      { HIDE(".#+.#") },
      { HIDE(".   #") },
      { HIDE("../..") },
      { HIDE("#.  .") },
      { HIDE("##..#") }
    },
    // 4
    //
    // Abyssal
    //
    {
      { HIDE("ll ll") },
      { HIDE("l   l") },
      { HIDE("  /  ") },
      { HIDE("l   l") },
      { HIDE("ll ll") }
    }
  };

  const std::vector<StringV> PondLayouts =
  {
    {
      { HIDE("         ") },
      { HIDE(" www www ") },
      { HIDE(" wgg ggw ") },
      { HIDE(" wgg ggw ") },
      { HIDE("    W    ") },
      { HIDE(" wgg ggw ") },
      { HIDE(" wgg ggw ") },
      { HIDE(" www www ") },
      { HIDE("         ") }
    },
    //
    // 1 - black, 2 - white
    //
    {
      { HIDE("121212121") },
      { HIDE("2ww121ww2") },
      { HIDE("1w12121w1") },
      { HIDE("212WWW212") },
      { HIDE("121WWW121") },
      { HIDE("212WWW212") },
      { HIDE("1w12121w1") },
      { HIDE("2ww121ww2") },
      { HIDE("121212121") }
    },
    {
      { HIDE("121212121") },
      { HIDE("2www2www2") },
      { HIDE("1wWw1wWw1") },
      { HIDE("2www2www2") },
      { HIDE("121212121") },
      { HIDE("2www2www2") },
      { HIDE("1wWw1wWw1") },
      { HIDE("2www2www2") },
      { HIDE("121212121") }
    },
    {
      { HIDE("ggggggggg") },
      { HIDE("ggggggggg") },
      { HIDE("gg     gg") },
      { HIDE("gg WWW gg") },
      { HIDE("gg WWW gg") },
      { HIDE("gg WWW gg") },
      { HIDE("gg     gg") },
      { HIDE("ggggggggg") },
      { HIDE("ggggggggg") }
    },
    {
      { HIDE("         ") },
      { HIDE(" ### ### ") },
      { HIDE(" #ggggg# ") },
      { HIDE(" #gwwwg# ") },
      { HIDE("  gwWwg  ") },
      { HIDE(" #gwwwg# ") },
      { HIDE(" #ggggg# ") },
      { HIDE(" ### ### ") },
      { HIDE("         ") }
    }
  };

  const std::vector<StringV> FountainLayouts =
  {
    {
      { HIDE(".... ....") },
      { HIDE(".##. .##.") },
      { HIDE(".#.   .#.") },
      { HIDE(".. www ..") },
      { HIDE("   wFw   ") },
      { HIDE(".. www ..") },
      { HIDE(".#.   .#.") },
      { HIDE(".##. .##.") },
      { HIDE(".... ....") }
    },
    {
      { HIDE("ggggggggg") },
      { HIDE("gwwwgwwwg") },
      { HIDE("gwFwgwFwg") },
      { HIDE("gwwwgwwwg") },
      { HIDE("ggggggggg") },
      { HIDE("gwwwgwwwg") },
      { HIDE("gwFwgwFwg") },
      { HIDE("gwwwgwwwg") },
      { HIDE("ggggggggg") }
    }
  };

  const std::vector<StringV> GardenLayouts =
  {
    {
      { HIDE("ggggggggg") },
      { HIDE("ggggTgggg") },
      { HIDE("ggTgggTgg") },
      { HIDE("ggggggggg") },
      { HIDE("gTggTggTg") },
      { HIDE("ggggggggg") },
      { HIDE("ggTgggTgg") },
      { HIDE("ggggTgggg") },
      { HIDE("ggggggggg") }
    },
    {
      { HIDE("gggg gggg") },
      { HIDE("gwww wwwg") },
      { HIDE("gwTg gTwg") },
      { HIDE("gwgg ggwg") },
      { HIDE("         ") },
      { HIDE("gwgg ggwg") },
      { HIDE("gwTg gTwg") },
      { HIDE("gwww wwwg") },
      { HIDE("gggg gggg") }
    },
    {
      { HIDE("gg ggg ggg gg") },
      { HIDE("g   g   g   g") },
      { HIDE("  T   T   T  ") },
      { HIDE("g   g   g   g") },
      { HIDE("gg ggg ggg gg") },
      { HIDE("g   g   g   g") },
      { HIDE("  T   T   T g") },
      { HIDE("g   g   g   g") },
      { HIDE("gg ggg ggg gg") },
      { HIDE("g   g   g   g") },
      { HIDE("  T   T   T g") },
      { HIDE("g   g   g   g") },
      { HIDE("gg ggg ggg gg") }
    }
  };

  const std::vector<StringV> PillarsLayouts =
  {
    {
      { HIDE("...........") },
      { HIDE("..#.....#..") },
      { HIDE(".###...###.") },
      { HIDE("..#.....#..") },
      { HIDE(".....#.....") },
      { HIDE("....###....") },
      { HIDE(".....#.....") },
      { HIDE("..#.....#..") },
      { HIDE(".###...###.") },
      { HIDE("..#.....#..") },
      { HIDE("...........") }
    },
    {
      { HIDE("gggggggggggggggggggg") },
      { HIDE("g   ggggg##ggggg   g") },
      { HIDE("g T gggg####gggg T g") },
      { HIDE("g   gggg####gggg   g") },
      { HIDE("gg gggggg##gggggg gg") },
      { HIDE("                    ") },
      { HIDE("                    ") },
      { HIDE("ggggggg      ggggggg") },
      { HIDE("gg##ggg      ggg##gg") },
      { HIDE("g####gg      gg####g") },
      { HIDE("g####gg      gg####g") },
      { HIDE("gg##ggg      ggg##gg") },
      { HIDE("ggggggg      ggggggg") },
      { HIDE("                    ") },
      { HIDE("                    ") },
      { HIDE("gg gggggg##gggggg gg") },
      { HIDE("g   gggg####gggg   g") },
      { HIDE("g T gggg####gggg T g") },
      { HIDE("g   ggggg##ggggg   g") },
      { HIDE("gggggggggggggggggggg") }
    },
    {
      { HIDE("....................") },
      { HIDE("..##.....##.....##..") },
      { HIDE(".####...####...####.") },
      { HIDE(".####...####...####.") },
      { HIDE("..##.....##.....##..") },
      { HIDE("....................") },
      { HIDE("....................") },
      { HIDE("....................") },
      { HIDE("..##.....##.....##..") },
      { HIDE(".####...####...####.") },
      { HIDE(".####...####...####.") },
      { HIDE("..##.....##.....##..") },
      { HIDE("....................") },
      { HIDE("....................") },
      { HIDE("....................") },
      { HIDE("..##.....##.....##..") },
      { HIDE(".####...####...####.") },
      { HIDE(".####...####...####.") },
      { HIDE("..##.....##.....##..") },
      { HIDE("....................") }
    }
  };

  const std::unordered_map<ShrineType, std::vector<StringV>> ShrineLayoutsByType =
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

  //
  // NOTE: for future use (maybe).
  //
  const std::vector<std::vector<std::string>> SpecialRooms =
  {
    // Castle
    {
      { HIDE("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW") },
      { HIDE("W###-###WWWWWWWWWWWWWWW###-###W") },
      { HIDE("W#     #WWWWWWWWWWWWWWW#     #W") },
      { HIDE("W#     #################     #W") },
      { HIDE("W|                           |W") },
      { HIDE("W#     #################     #W") },
      { HIDE("W#     #.#######.......#     #W") },
      { HIDE("W### #+#.#hh   S.......#+# ###W") },
      { HIDE("WWW# #...+   / S.........# #WWW") },
      { HIDE("WWW# #...#hh   S..######## #WWW") },
      { HIDE("WWW# #...#######..#     ## #WWW") },
      { HIDE("WWW# #............#     ## #WWW") },
      { HIDE(".#######..........+     ## #WWW") },
      { HIDE(".+.....+...ggg....+     ## #WWW") },
      { HIDE(".+.....+...gFg....#     ## #WWW") },
      { HIDE(".#######...ggg....#     ## #WWW") },
      { HIDE("WWW# #............######## #WWW") },
      { HIDE("WWW# #...................# #WWW") },
      { HIDE("WWW# #..##+###+###+##....# #WWW") },
      { HIDE("WWW# #..#   #   #   #....# #WWW") },
      { HIDE("W### #+##   #   #   #..#+# ###W") },
      { HIDE("W#     ##############..#     #W") },
      { HIDE("W#     #################     #W") },
      { HIDE("W|                           |W") },
      { HIDE("W#     #################     #W") },
      { HIDE("W#     #WWWWWWWWWWWWWWW#     #W") },
      { HIDE("W###-###WWWWWWWWWWWWWWW###-###W") },
      { HIDE("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW") },
    }
  };
};
