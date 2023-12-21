#ifndef ENUMERATIONS_H
#define ENUMERATIONS_H

//
// Used in FeatureRooms generator.
//
enum class FeatureRoomType
{
  EMPTY = 0,
  SHRINE,
  GARDEN,
  FLOODED,
  DIAMOND,
  ROUND,
  PILLARS,
  POND,
  FOUNTAIN
};

//
// Used to procedurally convert empty rooms into something interesting.
//
enum class TransformedRoom
{
  UNMARKED = -1,
  EMPTY,
  TREASURY,
  STORAGE,
  FLOODED,
  CHESTROOM,
  GRAVEYARD,
  LIBRARY,
  SHRINE,
  HALLOWED_GROUND,
  CURSED_GROUND,
  BATTLEFIELD,
  ZOO
};

enum class CornerType
{
  UL = 0,
  UR,
  DL,
  DR
};

enum class InteractionResult
{
  SUCCESS = 0,
  FAILURE,
  UNDEFINED
};

enum class UseResult
{
  SUCCESS = 0,
  FAILURE,
  UNUSABLE
};

enum class TriggerUpdateType
{
  GLOBAL = 0,
  FINISH_TURN
};

enum class TriggerType
{
  ONE_SHOT = 0,
  CONSTANT
};

enum class GameStates
{
  UNDEIFNED = -2,
  EXIT_GAME,
  ATTACK_STATE,
  MENU_STATE,
  MAIN_STATE,
  SELECT_CLASS_STATE,
  CUSTOM_CLASS_STATE,
  ENTER_NAME_STATE,
  INTRO_STATE,
  HELP_STATE,
  START_GAME_STATE,
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
  SAVE_GAME_STATE,
  PICKUP_ITEM_STATE,
  SERVICE_STATE,
  TARGET_STATE,
  EXITING_STATE,
  MESSAGE_BOX_STATE,
  DEV_CONSOLE,
  GAMEOVER_STATE
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
  THE_END,
  #ifdef BUILD_TESTS
  TEST_LEVEL
  #endif
};

//
// NOTE: total violation of the classification scheme.
//
enum class GameObjectType
{
  NONE = -1,
  HARMLESS,
  REMAINS,
  PLAYER,
  NPC,
  GROUND,
  SHALLOW_WATER,
  DEEP_WATER,
  LAVA,
  CHASM,
  PICKAXEABLE,
  BORDER,
  TRAP,
  BREAKABLE,
  CONTAINER,
  SHRINE,
  DOOR,
  STAIRS,
  // ---------------------------------------------------------------------------
  RAT,
  BAT,
  VAMPIRE_BAT,
  SPIDER,
  SHELOB,
  TROLL,
  MAD_MINER,
  SKELETON,
  ZOMBIE,           // has a chance to reanimate itself when slain
  NECROMANCER,      // resurrects dead monsters
  KOBOLD,
  HEROBRINE,
  WRAITH,
  STALKER,
  FIRE_MAGE,        // fire magic + wanders with fire elemental
  WATER_MAGE,       // -=-=-=-=-
  AIR_MAGE,         // -=-=-=-=-
  EARTH_MAGE,       // -=-=-=-=-
  ROGUE_WIZARD,     // casts random magic
  FIRE_ELEMENTAL,
  WATER_ELEMENTAL,
  AIR_ELEMENTAL,
  EARTH_ELEMENTAL,
  LOOTER,
  ARCHER,
  FENCER,           // has ignore defence and / or ignore armor sword
  ENUM_END
};

enum class NPCType
{
  UNDEFINED = -1,
  CLAIRE,
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
  SHIELD,   // TODO: add shields
  RING
};

//
// TODO: loot from kills uses GameObjectsFactory::CreateGameObject()
// which in turn does switch case on this enum.
// Don't forget to add other cases when finished
// with deciding loot tables for monsters.
//
enum class ItemType
{
  NOTHING = 0,
  DUMMY,
  COINS,
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

enum class PotionType
{
  NONE = 0,
  RANDOM,
  HEALING_POTION,
  MANA_POTION,
  JUICE_POTION,
  NP_POTION,
  STR_POTION,
  DEF_POTION,
  MAG_POTION,
  RES_POTION,
  SKL_POTION,
  SPD_POTION,
  EXP_POTION,
  CW_POTION,
  RA_POTION
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
  RANDOM,
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
  RANDOM,
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
  RANDOM,
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
  REMOVE_HUNGER,  // stops hunger counter
  FREE_ACTION,    // immune to paralysis
  POISON_IMMUNE,
  IGNORE_DEFENCE, // ignores DEF for damage calculation
  IGNORE_ARMOR,   // deals damage straight to HP, ignoring any armor
  KNOCKBACK,
  MANA_SHIELD,
  REGEN,
  REFLECT,
  LEECH,          // get hp on hit
  DMG_ABSORB,
  MAG_ABSORB,
  THORNS,         // damages enemy if player was hit
  PARALYZE,
  TRUE_SEEING,    // See anything invisible
  TELEPATHY,      // See monsters (with brains)
  LEVITATION,
  BLINDNESS,      // anti-visibility (-100500 to LightRadius)
  FROZEN,         // SPD penalty
  BURNING,        // inflicts fire damage over several turns + gives illuminated
  ILLUMINATED,    // monsters can see you further away (?) (not implemented)
  POISONED,       // anti-regen
  WEAKNESS        // penalties to STR, DEF, SKL and SPD
};

enum class FoodType
{
  NONE = -1,
  FIRST_ELEMENT,
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
  // TODO: dispel magic?

  NONE = -1,         // Wand of Nothing can be imbued with spell
  STRIKE,            // 1 target, DEF
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
  DETECT_MONSTERS,   // shows living monsters (not all?), temporary
  TRUE_SEEING,       // shows anything invisible
  REMOVE_CURSE,
  REPAIR,
  HEAL,
  NEUTRALIZE_POISON,
  MANA_SHIELD        // nuff said
};

//
// Number means tier.
//
enum class WandMaterials
{
  NONE = -1,
  YEW_1,
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
  NONE = -1,
  MIGHT,       // raises STR or HP
  SPIRIT,      // raises MAG or MP
  TRANQUILITY, // restores MP
  KNOWLEDGE,   // identifies items
  PERCEPTION,  // reveals BUC status
  HEALING,     // restores HP
  DESECRATED,  // random effect + receive curse
  RUINED,      // random effect + low chance to receive negative effect
  DISTURBING,  // random effect + high chance to receive negative effect
  ABYSSAL,     // random stat raise + negative effect
  FORGOTTEN,   // random effect
  POTENTIAL,   // temporary raises stats
  HIDDEN,      // random effect
  HOLY         // removes curse
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

//
// In certain algorithms it is more suitable
// to call the same type by another name.
//
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

enum class ScriptTaskNames
{
  NONE = 0,
  TREE,
  SEL,
  SEQ,
  FAIL,
  SUCC,
  TASK,
  COND
};

enum class ScriptParamNames
{
  IDLE = 101,
  MOVE_RND,
  MOVE_SMART,
  MOVE_AWAY,
  ATTACK,
  ATTACK_RANGED,
  BREAK_STUFF,
  PICK_ITEMS,
  CHASE_PLAYER,
  SAVE_PLAYER_POS,
  GOTO_LAST_PLAYER_POS,
  GOTO_LAST_MINED_POS,
  MINE_TUNNEL,
  MINE_BLOCK,
  APPLY_EFFECT,
  DRINK_POTION,
  PRINT_MESSAGE,
  END,
  D100,
  PLAYER_VISIBLE,
  PLAYER_CAN_MOVE,
  PLAYER_IN_RANGE,
  PLAYER_ENERGY,
  PLAYER_NEXT_TURN,
  TURNS_LEFT,
  TURNS_CHECK,
  HAS_EFFECT,
  HAS_EQUIPPED,
  HP_LOW,
  EQ,
  GE,
  LE,
  GT,
  LT,
  PLAYER,
  SELF,
  HEA,
  NCK,
  TRS,
  BTS,
  MAG,
  WPN,
  SLD,
  RNG,
  ANY,
  HP,
  MP,
  HID,
  SHI,
  REG,
  REF,
  PAB,
  MAB,
  THS,
  PAR,
  TEL,
  FLY,
  BLI,
  FRZ,
  BUR,
  LGT,
  PSD,
  NOP
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
  TRIPLE_HBARS = 240,
  WAVES = 247,
  LAST_ELEMENT = 255
};
#endif

#endif // ENUMERATIONS_H
