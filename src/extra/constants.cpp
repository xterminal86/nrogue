#include "constants.h"
#include "base64-strings.h"

namespace Colors
{
  namespace ShadesOfGrey
  {
    const std::string One      = "#111111";
    const std::string Two      = "#222222";
    const std::string Three    = "#333333";
    const std::string Four     = "#444444";
    const std::string Five     = "#555555";
    const std::string Six      = "#666666";
    const std::string Seven    = "#777777";
    const std::string Eight    = "#888888";
    const std::string Nine     = "#999999";
    const std::string Ten      = "#AAAAAA";
    const std::string Eleven   = "#BBBBBB";
    const std::string Twelve   = "#CCCCCC";
    const std::string Thirteen = "#DDDDDD";
    const std::string Fourteen = "#EEEEEE";
  }

  const std::string BlackColor                = "#000000";
  const std::string WhiteColor                = "#FFFFFF";
  const std::string RedColor                  = "#FF0000";
  const std::string GreenColor                = "#00FF00";
  const std::string BlueColor                 = "#0000FF";
  const std::string CyanColor                 = "#00FFFF";
  const std::string MagentaColor              = "#FF00FF";
  const std::string YellowColor               = "#FFFF00";
  const std::string DandelionYellowColor      = "#F0E130";
  const std::string RedPoppyColor             = "#E42F0C";
  const std::string MarbleColor               = "#FFE0E0";
  const std::string CaveWallColor             = "#964B00";
  const std::string ObsidianColorHigh         = "#5B4965";
  const std::string ObsidianColorLow          = "#3D354B";
  const std::string GrassColor                = "#006600";
  const std::string GrassDotColor             = "#009900";
  const std::string DirtColor                 = "#9B7653";
  const std::string DirtDotColor              = "#7B5633";
  const std::string PlayerColor               = "#00FFFF";
  const std::string MonsterColor              = "#FF0000";
  const std::string MonsterUniqueColor        = "#FF8888";
  const std::string ShallowWaterColor         = "#9999FF";
  const std::string DeepWaterColor            = "#3333FF";
  const std::string IronColor                 = "#CBCDCD";
  const std::string RoomFloorColor            = "#692E11";
  const std::string WoodColor                 = "#DCB579";
  const std::string ChestColor                = "#A0793D";
  const std::string BrickColor                = "#DC5539";
  const std::string LavaColor                 = "#E36517";
  const std::string LavaWavesColor            = "#E3A917";
  const std::string FogOfWarColor             = "#202020";
  const std::string CoinsColor                = "#FFD700";
  const std::string DoorHighlightColor        = "#666600";
  const std::string MessageBoxRedBorderColor  = "#880000";
  const std::string MessageBoxBlueBorderColor = "#8888FF";
  const std::string MessageBoxHeaderBgColor   = "#4444AA";
  const std::string ItemMagicColor            = "#4169E1";
  const std::string ItemRareColor             = "#EEEE52";
  const std::string ItemUniqueColor           = "#A59263";
  const std::string ItemMixedColor            = "#AA7700";
  const std::string ItemCursedColor           = "#AA0000";

  const std::map<GemType, std::pair<std::string, std::string>> GemColorByType =
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

  const std::map<ShrineType, std::pair<std::string, std::string>> ShrineColorsByType =
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
    { ShrineType::HIDDEN,      { "#888888", "#333333" } },
    { ShrineType::HOLY,        { "#FFFF00", "#888888" } },
  };

  const std::map<std::string, std::vector<std::string>> PotionColorsByName =
  {
    { "Red Potion",     { "#FF0000", "#440000" } },
    { "Green Potion",   { "#00FF00", "#004400" } },
    { "Blue Potion",    { "#4444FF", "#000044" } },
    { "Cyan Potion",    { "#00FFFF", "#004444" } },
    { "Magenta Potion", { "#FF00FF", "#440044" } },
    { "Yellow Potion",  { "#FFFF00", "#444400" } },
    { "Radiant Potion", { "#FFFF88", "#777700" } },
    { "Morbid Potion",  { "#AA6622", "#880000" } },
    { "Clear Potion",   { "#FFFFFF", "#000000" } },
    { "Black Potion",   { "#000000", "#444444" } },
    { "Watery Potion",  { "#BBBBBB", "#000000" } }
  };

  const std::map<WandMaterials, std::pair<std::string, std::string>> WandColorsByMaterial =
  {
    { WandMaterials::YEW_1,    { "#D2AB7C", "#8A8B5C" } },
    { WandMaterials::IVORY_2,  { "#FFFFFF", "#9A9A9A" } },
    { WandMaterials::EBONY_3,  { "#888888", "#555D50" } },
    { WandMaterials::ONYX_4,   { "#666666", "#2F2F2F" } },
    { WandMaterials::GLASS_5,  { "#000000", "#FFFFFF" } },
    { WandMaterials::COPPER_6, { "#FF8C00", "#B87333" } },
    { WandMaterials::GOLDEN_7, { "#FFFF00", "#AAA700" } }
  };

  const std::map<GemType, std::string> GemColorNameByType =
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
  // ===========================================================================
  const char InventoryEmptySlotChar = '.';
  // ===========================================================================
  const std::string ItemDefaultDescAccessory = "Could be magical or just a trinket.";
  const std::string ItemDefaultDescGem       = "Is this valuable?";
  const std::string ItemDefaultDescWeaponDmg = "You think it'll do %d damage on average.";
  const std::string ItemDefaultDescWeaponEnd = "You can't tell anything else.";
  // ===========================================================================
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
  // ===========================================================================
  const std::string FmtPickedUpIS = "Picked up: %i %s";
  const std::string FmtPickedUpS  = "Picked up: %s";
  const std::string FmtDroppedIS  = "Dropped: %i %s";
  const std::string FmtDroppedS   = "Dropped: %s";
  const std::string FmtSMissed    = "%s missed";
  // ===========================================================================
  const std::vector<std::string> MsgNotInTown = { "Not here", "Not in town" };
  // ===========================================================================
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
    const std::string RoyalGateText    = "Royal Gate";
    const std::string TreeText         = "Tree";
    const std::string WitheredTreeText = "Withered Tree";
    const std::string GrassText        = "Grass";
    const std::string BloodText        = "Blood";
    const std::string HellrockText     = "Hellrock";
    const std::string HellstoneText    = "Hellstone";
  }
}

namespace GlobalConstants
{
  std::map<RoomEdgeEnum, std::string> DirNamesByDir =
  {
    { RoomEdgeEnum::NORTH, "NORTH" },
    { RoomEdgeEnum::EAST,  "EAST"  },
    { RoomEdgeEnum::SOUTH, "SOUTH" },
    { RoomEdgeEnum::WEST,  "WEST"  }
  };

#ifdef USE_SDL
  //
  // To avoid C-style casting from enum
  //
  std::map<NameCP437, int> CP437IndexByType;
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
  const int AwardedExpDefault           = 20;
  const int AwardedExpMax               = 40;
  const int MinHitChance                = 5;
  const int MaxHitChance                = 95;
  const int DisplayAttackDelayMs        = 50;
  const int InventoryMaxSize            = 20;

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

  // 11 is because if < StarvationDamageTimeout
  const int StarvationDamageTimeout = 11;

  // Default number of turn passes
  // after which monster is attepmted to be spawned.
  const int MonstersRespawnTimeout = 250;

  // Also the number of action increments (end turns)
  // for player before he can act (for SPD = 0)
  const int EffectDurationSkipsForTurn = ((float)TurnReadyValue / (float)TurnTickValue) + 1;

  // Default duration is 20 SPD=0 player end turns
  const int EffectDefaultDuration = EffectDurationSkipsForTurn * 20;

  // std::map is sorted by key
  const std::map<int, std::pair<StatsEnum, std::string>> AllStatNames =
  {
    { 0, { StatsEnum::STR, "STR" } },
    { 1, { StatsEnum::DEF, "DEF" } },
    { 2, { StatsEnum::MAG, "MAG" } },
    { 3, { StatsEnum::RES, "RES" } },
    { 4, { StatsEnum::SKL, "SKL" } },
    { 5, { StatsEnum::SPD, "SPD" } }
  };

  const std::map<ItemBonusType, std::string> BonusNameByType =
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

  const std::map<ItemQuality, std::string> QualityNameByQuality =
  {
    { ItemQuality::DAMAGED,     "Damaged"     },
    { ItemQuality::FLAWED,      "Flawed"      },
    { ItemQuality::NORMAL,      ""            },
    { ItemQuality::FINE,        "Fine"        },
    { ItemQuality::EXCEPTIONAL, "Exceptional" }
  };

  const std::map<PlayerSkills, std::string> SkillNameByType =
  {
    { PlayerSkills::REPAIR,       "Repair"       },
    { PlayerSkills::RECHARGE,     "Recharge"     },
    { PlayerSkills::SPELLCASTING, "Spellcasting" },
    { PlayerSkills::AWARENESS,    "Awareness"    }
  };

  const std::map<TraderRole, std::string> ShopNameByType =
  {
    { TraderRole::BLACKSMITH, "Armory"    },
    { TraderRole::CLERIC,     "Sanctuary" },
    { TraderRole::COOK,       "Grocery"   },
    { TraderRole::JUNKER,     "Junkyard"  }
  };

  const std::map<ItemType, int> PotionsWeightTable =
  {
    { ItemType::HEALING_POTION, 50 },
    { ItemType::MANA_POTION,    50 },
    { ItemType::NP_POTION,      50 },
    { ItemType::JUICE_POTION,   20 },
    { ItemType::STR_POTION,      1 },
    { ItemType::DEF_POTION,      1 },
    { ItemType::MAG_POTION,      1 },
    { ItemType::RES_POTION,      1 },
    { ItemType::SKL_POTION,      1 },
    { ItemType::SPD_POTION,      1 },
    { ItemType::EXP_POTION,      5 },
    { ItemType::CW_POTION,       5 },
    { ItemType::RA_POTION,       5 }
  };

  const std::map<PlayerStats, std::string> StatNameByType =
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

  const std::map<ItemType, std::string> PotionNameByType =
  {
    { ItemType::HEALING_POTION, "Healing Potion"    },
    { ItemType::MANA_POTION,    "Mana Potion"       },
    { ItemType::JUICE_POTION,   "Fruit Juice"       },
    { ItemType::NP_POTION,      "Neutralize Poison" },
    { ItemType::STR_POTION,     "STR Potion"        },
    { ItemType::DEF_POTION,     "DEF Potion"        },
    { ItemType::MAG_POTION,     "MAG Potion"        },
    { ItemType::RES_POTION,     "RES Potion"        },
    { ItemType::SKL_POTION,     "SKL Potion"        },
    { ItemType::SPD_POTION,     "SPD Potion"        },
    { ItemType::EXP_POTION,     "EXP Potion"        },
    { ItemType::RA_POTION,      "Restore Ability"   },
    { ItemType::CW_POTION,      "Cure Weakness"     }
  };

  const std::map<ItemType, std::string> StatNameByPotionType =
  {
    { ItemType::STR_POTION, "STR" },
    { ItemType::DEF_POTION, "DEF" },
    { ItemType::MAG_POTION, "MAG" },
    { ItemType::RES_POTION, "RES" },
    { ItemType::SKL_POTION, "SKL" },
    { ItemType::SPD_POTION, "SPD" },
  };

  const std::map<WeaponType, std::string> WeaponNameByType =
  {
    { WeaponType::DAGGER,       "Dagger"         },
    { WeaponType::SHORT_SWORD,  "Short Sword"    },
    { WeaponType::ARMING_SWORD, "Arming Sword"   },
    { WeaponType::LONG_SWORD,   "Longsword"      },
    { WeaponType::GREAT_SWORD,  "Great Sword"    },
    { WeaponType::STAFF,        "Battle Staff"   },
    { WeaponType::PICKAXE,      "Pickaxe"        }
  };

  const std::map<RangedWeaponType, std::string> RangedWeaponNameByType =
  {
    { RangedWeaponType::SHORT_BOW,  "Short Bow"   },
    { RangedWeaponType::LONGBOW,    "Longbow"     },
    { RangedWeaponType::WAR_BOW,    "War Bow"     },
    { RangedWeaponType::LIGHT_XBOW, "L. Crossbow" },
    { RangedWeaponType::XBOW,       "Crossbow"    },
    { RangedWeaponType::HEAVY_XBOW, "H. Crossbow" }
  };

  const std::map<ArrowType, std::string> ArrowNameByType =
  {
    { ArrowType::ARROWS, "Arrows" },
    { ArrowType::BOLTS,  "Bolts"  }
  };

  const std::map<ArmorType, std::string> ArmorNameByType =
  {
    { ArmorType::PADDING, "Padded Surcoat" },
    { ArmorType::LEATHER, "Leather Jacket" },
    { ArmorType::MAIL,    "Mail Hauberk"   },
    { ArmorType::SCALE,   "Scale Armor"    },
    { ArmorType::PLATE,   "Coat of Plates" }
  };

  const std::map<ArmorType, int> ArmorDurabilityByType =
  {
    { ArmorType::PADDING, 25 },
    { ArmorType::LEATHER, 50 },
    { ArmorType::MAIL,    80 },
    { ArmorType::SCALE,  120 },
    { ArmorType::PLATE,  180 }
  };

  const std::map<FoodType, std::pair<std::string, int>> FoodHungerPercentageByName =
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

  const std::map<GemType, std::string> GemNameByType =
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

  const std::map<GemType, int> GemCostByType =
  {
    { GemType::WORTHLESS_GLASS,    0 },  // 0
    { GemType::BLACK_OBSIDIAN,   200 },  // 200
    { GemType::GREEN_JADE,       300 },  // 300
    { GemType::PURPLE_FLUORITE,  400 },  // 400
    { GemType::PURPLE_AMETHYST,  600 },  // 600
    { GemType::RED_GARNET,       700 },  // 700
    { GemType::WHITE_OPAL,       800 },  // 800
    { GemType::BLACK_JETSTONE,   850 },  // 850
    { GemType::ORANGE_AMBER,    1000 },  // 1000
    { GemType::YELLOW_CITRINE,  1500 },  // 1500
    { GemType::BLUE_AQUAMARINE, 1500 },  // 1500
    { GemType::GREEN_EMERALD,   2500 },  // 2500
    { GemType::BLUE_SAPPHIRE,   3000 },  // 3000
    { GemType::ORANGE_JACINTH,  3250 },  // 3250
    { GemType::RED_RUBY,        3500 },  // 3500
    { GemType::WHITE_DIAMOND,   4000 }   // 4000
  };

  const std::map<WandMaterials, int> WandCapacityByMaterial =
  {
    { WandMaterials::YEW_1,    100 },
    { WandMaterials::IVORY_2,  150 },
    { WandMaterials::EBONY_3,  200 },
    { WandMaterials::ONYX_4,   300 },
    { WandMaterials::GLASS_5,  450 },
    { WandMaterials::COPPER_6, 600 },
    { WandMaterials::GOLDEN_7, 800 }
  };

  // Divide wand capacity by this value to get amount of charges.
  // Total amount of charges is affected by material, BUC status and RNG god.
  // (see GameObjectsFactory::CreateWand())
  const std::map<SpellType, int> WandSpellCapacityCostByType =
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

  const std::map<WandMaterials, int> WandRangeByMaterial =
  {
    { WandMaterials::YEW_1,     5 },
    { WandMaterials::IVORY_2,  10 },
    { WandMaterials::EBONY_3,  15 },
    { WandMaterials::ONYX_4,   20 },
    { WandMaterials::GLASS_5,  25 },
    { WandMaterials::COPPER_6, 30 },
    { WandMaterials::GOLDEN_7, 35 }
  };

  const std::vector<SpellType> ScrollValidSpellTypes =
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
    SpellType::MANA_SHIELD
  };

  // Not including combat spells.
  //
  // Two separate vectors are from the times where
  // several unidentified names could map to the same scroll.
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
    "KOBAYASHI MARU"
  };

  const std::map<SpellType, std::string> SpellShortNameByType =
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
    { SpellType::REMOVE_CURSE,      "RC" },
    { SpellType::REPAIR,            "R"  },
    { SpellType::HEAL,              "H"  },
    { SpellType::NEUTRALIZE_POISON, "NP" },
    { SpellType::MANA_SHIELD,       "MS" }
  };

  const std::map<WandMaterials, std::string> WandMaterialNamesByMaterial =
  {
    { WandMaterials::YEW_1,    "Yew"    },
    { WandMaterials::IVORY_2,  "Ivory"  },
    { WandMaterials::EBONY_3,  "Ebony"  },
    { WandMaterials::ONYX_4,   "Onyx"   },
    { WandMaterials::GLASS_5,  "Glass"  },
    { WandMaterials::COPPER_6, "Copper" },
    { WandMaterials::GOLDEN_7, "Golden" }
  };

  const std::map<ShrineType, std::string> ShrineSaintByType =
  {
    { ShrineType::MIGHT,      "St. George the Soldier"     },
    { ShrineType::SPIRIT,     "St. Mary the Mother"        },
    { ShrineType::KNOWLEDGE,  "St. Nestor the Scribe"      },
    { ShrineType::PERCEPTION, "St. Justin the Philosopher" },
    { ShrineType::HEALING,    "St. Luke the Healer"        }
  };

  const std::map<ShrineType, std::string> ShrineNameByType =
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
    "no",
    "ow",
    "to",
    "nd",
    "nt",
    "sh",
    "rsh",
    "urgh",
    "urg",
    "stan",
    "heim",
    "town",
    "ton",
    "shire",
    "pool",
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

  const std::map<MapType, std::vector<std::string>> MapLevelNames =
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
    { MapType::DEEP_DARK_1, { "Underdark"            } },
    { MapType::DEEP_DARK_2, { "Embrace of Darkness"  } },
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

  const std::map<ItemBonusType, std::string> ItemBonusPrefixes =
  {
    { ItemBonusType::STR,            "Heavy"       },
    { ItemBonusType::DEF,            "Rampart"     },
    { ItemBonusType::MAG,            "Magical"     },
    { ItemBonusType::RES,            "Electrum"    },
    { ItemBonusType::SKL,            "Expert"      },
    { ItemBonusType::SPD,            "Swift"       },
    { ItemBonusType::HP,             "Vital"       },
    { ItemBonusType::MP,             "Soulful"     },
    { ItemBonusType::INDESTRUCTIBLE, "Everlasting" },
    { ItemBonusType::SELF_REPAIR,    "Reliable"    },
    { ItemBonusType::VISIBILITY,     "Shining"     },
    { ItemBonusType::INVISIBILITY,   "Concealing"  },
    { ItemBonusType::DAMAGE,         "Deadly"      },
    { ItemBonusType::REMOVE_HUNGER,  "Sustaining"  },
    { ItemBonusType::FREE_ACTION,    "Unbound"     },
    { ItemBonusType::POISON_IMMUNE,  "Viridian"    },
    { ItemBonusType::IGNORE_DEFENCE, "Defiant"     },
    { ItemBonusType::IGNORE_ARMOR,   "Piercing"    },
    { ItemBonusType::KNOCKBACK,      "Mighty"      },
    { ItemBonusType::MANA_SHIELD,    "Spiritual"   },
    { ItemBonusType::REGEN,          "Restorative" },
    { ItemBonusType::REFLECT,        "Silver"      },
    { ItemBonusType::LEECH,          "Vampiric"    },
    { ItemBonusType::DMG_ABSORB,     "Protective"  },
    { ItemBonusType::MAG_ABSORB,     "Shielding"   },
    { ItemBonusType::THORNS,         "Spiked"      },
    { ItemBonusType::TELEPATHY,      "Cautious"    },
    { ItemBonusType::LEVITATION,     "Celestial"   }
  };

  const std::map<ItemBonusType, std::string> ItemBonusSuffixes =
  {
    { ItemBonusType::STR,            "of Strength"    },
    { ItemBonusType::DEF,            "of Defence"     },
    { ItemBonusType::MAG,            "of Magic"       },
    { ItemBonusType::RES,            "of Resistance"  },
    { ItemBonusType::SKL,            "of Skill"       },
    { ItemBonusType::SPD,            "of Speed"       },
    { ItemBonusType::HP,             "of Life"        },
    { ItemBonusType::MP,             "of Mana"        },
    { ItemBonusType::INDESTRUCTIBLE, "of Ages"        },
    { ItemBonusType::SELF_REPAIR,    "of Reliability" },
    { ItemBonusType::VISIBILITY,     "of the Sun"     },
    { ItemBonusType::INVISIBILITY,   "of the Unseen"  },
    { ItemBonusType::DAMAGE,         "of Destruction" },
    { ItemBonusType::REMOVE_HUNGER,  "of Satiation"   },
    { ItemBonusType::FREE_ACTION,    "of Free Action" },
    { ItemBonusType::POISON_IMMUNE,  "of Cleansing"   },
    { ItemBonusType::IGNORE_DEFENCE, "of the Master"  },
    { ItemBonusType::IGNORE_ARMOR,   "of the Awl"     },
    { ItemBonusType::KNOCKBACK,      "of the Bear"    },
    { ItemBonusType::MANA_SHIELD,    "of the Force"   },
    { ItemBonusType::REGEN,          "of the Undying" },
    { ItemBonusType::REFLECT,        "of Reflection"  },
    { ItemBonusType::LEECH,          "of the Blood"   },
    { ItemBonusType::DMG_ABSORB,     "of Protection"  },
    { ItemBonusType::MAG_ABSORB,     "of Shielding"   },
    { ItemBonusType::THORNS,         "of the Ivy"     },
    { ItemBonusType::TELEPATHY,      "of the Oracle"  },
    { ItemBonusType::LEVITATION,     "of the Angel"   }
  };

  const std::map<ItemBonusType, int> MoneyCostIncreaseByBonusType =
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
    { ItemBonusType::LEVITATION,      500 }
  };

  const std::map<ItemBonusType, std::string> BonusDisplayNameByType =
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
    { ItemBonusType::LEVITATION,   "Fly" },
    { ItemBonusType::BLINDNESS,    "Bli" },
    { ItemBonusType::FROZEN,       "Frz" },
    { ItemBonusType::BURNING,      "Bur" },
    { ItemBonusType::ILLUMINATED,  "Lgt" },
    { ItemBonusType::POISONED,     "Psd" },
    { ItemBonusType::WEAKNESS,     "Wea" }
  };

  const std::map<std::string, ScriptTaskNames> BTSTaskNamesByName =
  {
    { "TREE", ScriptTaskNames::TREE },
    { "SEL",  ScriptTaskNames::SEL  },
    { "SEQ",  ScriptTaskNames::SEQ  },
    { "FAIL", ScriptTaskNames::FAIL },
    { "SUCC", ScriptTaskNames::SUCC },
    { "TASK", ScriptTaskNames::TASK },
    { "COND", ScriptTaskNames::COND }
  };

  const std::map<std::string, ScriptParamNames> BTSParamNamesByName =
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
    { "has_effect",           ScriptParamNames::HAS_EFFECT           },
    { "has_equipped",         ScriptParamNames::HAS_EQUIPPED         },
    { "hp_low",               ScriptParamNames::HP_LOW               },
    { "eq",                   ScriptParamNames::EQ                   },
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

  const std::vector<std::vector<std::string>> DungeonRooms =
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

  // Different game levels may create different themed
  // tiles in place of '.' or ' '
  const std::vector<StringsArray2D> ShrineLayouts =
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

  const std::vector<StringsArray2D> PondLayouts =
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

  const std::vector<StringsArray2D> FountainLayouts =
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

  const std::vector<StringsArray2D> GardenLayouts =
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
    },
    {
      "gg ggg ggg gg",
      "g   g   g   g",
      "  T   T   T  ",
      "g   g   g   g",
      "gg ggg ggg gg",
      "g   g   g   g",
      "  T   T   T g",
      "g   g   g   g",
      "gg ggg ggg gg",
      "g   g   g   g",
      "  T   T   T g",
      "g   g   g   g",
      "gg ggg ggg gg"
    }
  };

  const std::vector<StringsArray2D> PillarsLayouts =
  {
    {
      "...........",
      "..#.....#..",
      ".###...###.",
      "..#.....#..",
      ".....#.....",
      "....###....",
      ".....#.....",
      "..#.....#..",
      ".###...###.",
      "..#.....#..",
      "..........."
    },
    {
      "gggggggggggggggggggg",
      "g   ggggg##ggggg   g",
      "g T gggg####gggg T g",
      "g   gggg####gggg   g",
      "gg gggggg##gggggg gg",
      "                    ",
      "                    ",
      "ggggggg      ggggggg",
      "gg##ggg      ggg##gg",
      "g####gg      gg####g",
      "g####gg      gg####g",
      "gg##ggg      ggg##gg",
      "ggggggg      ggggggg",
      "                    ",
      "                    ",
      "gg gggggg##gggggg gg",
      "g   gggg####gggg   g",
      "g T gggg####gggg T g",
      "g   ggggg##ggggg   g",
      "gggggggggggggggggggg"
    },
    {
      "....................",
      "..##.....##.....##..",
      ".####...####...####.",
      ".####...####...####.",
      "..##.....##.....##..",
      "....................",
      "....................",
      "....................",
      "..##.....##.....##..",
      ".####...####...####.",
      ".####...####...####.",
      "..##.....##.....##..",
      "....................",
      "....................",
      "....................",
      "..##.....##.....##..",
      ".####...####...####.",
      ".####...####...####.",
      "..##.....##.....##..",
      "...................."
    }
  };

  const std::map<ShrineType, std::vector<StringsArray2D>> ShrineLayoutsByType =
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

  const std::vector<std::vector<std::string>> SpecialRooms =
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
};
