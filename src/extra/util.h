#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <climits>
#include <iomanip>
#include <random>

#include "globals.h"
#include "position.h"
#include "item-data.h"

#include "rng.h"

//
// We surround statements with one-shot 'do ... while' loop
// to prevent accidental bugs due to unexpected execution
// after macro expansion.
// E.g. after no-curly-braced 'if' condition,
// only first expanded macro statement will be executed,
// which may be not what we wanted.
// Also this will make function-like macro usage syntax
// uniform with regular function syntax, which will
// be impossible if we just used { } braces, since, for example,
// trying to write 'DebugLog("хуй");' inside 'if...else{}'
// won't compile, because it will be unfolded into compound
// statement with ; at the end of } brace, which will invalidate
// the syntax of 'if...else' form.
//
#ifndef USE_SDL
  #ifdef DEBUG_BUILD
    #define DebugLog(format, ...)        \
      do {                               \
      printf(format, ##__VA_ARGS__);     \
      fflush(stdout);                    \
      } while (false)
  #else
    #define DebugLog(format, ...)
  #endif
#else
#include "SDL2/SDL.h"
  #ifdef DEBUG_BUILD
    #define DebugLog(format, ...) SDL_Log(format, ##__VA_ARGS__)
  #else
    #define DebugLog(format, ...)
  #endif
#endif

#define STRINGIFY(ARG) #ARG

#ifdef DEBUG_BUILD
    #define LogPrint(str, ...) Game::gLogger.Print(str, ##__VA_ARGS__)
  #else
    #define LogPrint(str, ...)
#endif

#ifdef USE_SDL
  #define ConsoleLog(format, ...) SDL_Log(format, ##__VA_ARGS__)
#else
  #define ConsoleLog(format, ...)    \
    do                               \
    {                                \
      printf(format, ##__VA_ARGS__); \
      fflush(stdout);                \
    } while(false)
#endif

// Coded Map
using CM = std::vector<std::vector<std::pair<uint16_t, uint16_t>>>;

// Coded String
using CS = std::vector<uint16_t>;

using PositionV = std::vector<Position>;

class GameObject;

namespace Util
{
  extern StringV StringSplit(const std::string& str, char delim);

  extern StringV DecodeMap(const CM& map);
  extern std::string DecodeString(const CS& str);

  extern bool IsObjectInRange(GameObject* checker,
                              GameObject* checked,
                              int range);

  extern bool IsObjectInRange(const Position& posToCheckFrom,
                              const Position& objectPositionToCheck,
                              int rangeX,
                              int rangeY);

  extern std::vector<GameObject*>
  FindObjectsInRange(GameObject* aroundWho,
                     const std::vector<
                     std::vector<std::unique_ptr<GameObject>>
                     >& where,
                     int range);

  extern std::vector<GameObject*>
  FindObjectsInRange(GameObject* aroundWho,
                     const std::vector<std::unique_ptr<GameObject>>& where,
                     int range);

  extern bool IsBase64(unsigned char c);
  extern std::string Base64_Encode(unsigned char const* bytes_to_encode,
                                   unsigned int in_len);
  extern std::string Base64_Decode(const std::string& encoded_string);

  extern std::string Encrypt(const std::string& str);

  extern std::vector<unsigned char>
  ConvertStringToBytes(const std::string& encodedStr);

  extern bool CheckLimits(const Position& posToCheck, const Position& limits);

  //
  // Head-on implementation.
  //
  extern std::vector<Position> BresenhamLine(int sx, int sy, int ex, int ey);
  extern std::vector<Position> BresenhamLine(const Position& start,
                                             const Position& end);
  //
  // Uses precomputed cache.
  // Returns offsets that should be added to starting position for points along
  // the line.
  //
  extern const PositionV& BresenhamLineFast(int32_t sx,
                                              int32_t sy,
                                              int32_t ex,
                                              int32_t ey,
                                              bool truePositions = false);
  extern const PositionV& BresenhamLineFast(const Position& start,
                                              const Position& end,
                                              bool truePositions = false);
  //
  // Returns actual Bresenham line points using cache (slower).
  //
  extern const PositionV& BresenhamLineFastPoints(int32_t sx,
                                                     int32_t sy,
                                                     int32_t ex,
                                                     int32_t ey);
  extern const PositionV& BresenhamLineFastPoints(const Position& start,
                                                     const Position& end);

  extern bool IsInsideMap(const Position& pos,
                          const Position& mapSize,
                          bool leaveBorders = true);

  extern int Clamp(int value, int min, int max);

  extern int BlockDistance(int x1, int y1, int x2, int y2);
  extern int BlockDistance(const Position& from, const Position& to);

  extern double LinearDistance(int x1, int y1, int x2, int y2);
  extern double LinearDistance(const Position& s, const Position& e);

  extern std::vector<Position> GetEightPointsAround(const Position& pos,
                                                    const Position& mapSize);

  extern std::vector<Position> GetScreenRect(int x1, int y1, int x2, int y2);

  extern std::vector<Position>
  GetScreenRectPerimeter(int x1,
                         int y1,
                         int x2,
                         int y2,
                         bool includeCorners = true);

  extern std::vector<Position> GetScreenRectAroundPoint(int pointX,
                                                        int pointY,
                                                        int rangeX,
                                                        int rangeY);

  extern std::vector<Position> GetRectAroundPoint(int pointX,
                                                  int pointY,
                                                  int rangeX,
                                                  int rangeY,
                                                  const Position& mapSize);

  extern std::vector<Position> GetPerimeter(int x,
                                            int y,
                                            int w,
                                            int h,
                                            bool includeCorners = true);

  extern std::vector<Position>
  GetPerimeterAroundPoint(int x,
                          int y,
                          int w,
                          int h,
                          bool includeCorners = true);

  extern std::vector<GameObject*> GetActorsInRange(GameObject* from, int range);
  extern std::vector<GameObject*> GetContainersInRange(GameObject* from,
                                                       int range);

  extern std::string ChooseRandomName();

  extern StringV RotateRoomLayout(const StringV& layout, RoomLayoutRotation r);

  extern int RandomRange(int min, int max, std::mt19937_64& rng);

  extern int Rolld100();

  extern bool Rolld100(int successChance, bool twoRN = false);

  extern int RollDices(int numRolls, int diceSides);

  extern int GetExpForNextLevel(int curLvl);

  extern void RecalculateWandStats(ItemComponent* wand);
  extern void BlessItem(ItemComponent* item);

  extern void Sleep(uint32_t delayMs);

  extern bool WaitForMs(uint64_t delayMs, bool reset = false);
  extern void PrintVector(const std::string& title,
                          const std::vector<Position>& v);

  extern void PrintLayout(const StringV& l);

  extern void LaunchProjectile(const Position& from,
                               const Position& to,
                               char image,
                               const uint32_t& fgColor,
                               const uint32_t& bgColor = Colors::BlackColor);

  extern void LaunchProjectile(const std::vector<Position>& line,
                               char image,
                               const uint32_t& fgColor,
                               const uint32_t& bgColor = Colors::BlackColor);

  extern void KnockBack(GameObject* sender,
                        GameObject* receiver,
                        const Position& attackDir,
                        int tiles);

  extern void ReapplyBonuses(GameObject* onWho, ItemComponent* item);

  extern std::string ProcessTeleport(GameObject* target);

  extern std::pair<char, uint32_t>
  GetProjectileImageAndColor(ItemComponent* weapon,
                             bool throwingFromInventory);

  extern std::vector<GameObject*>
  GetObjectsOnTheLine(const std::vector<Position>& line);

  extern GameObject* GetFirstObjectOnTheLine(const std::vector<Position>& line);

  // ---------------------------------------------------------------------------

  extern std::pair<bool, std::string> TryToDamageObject(GameObject* object,
                                                        GameObject* from,
                                                        int amount,
                                                        bool againstRes);

  extern std::string TryToDamageEquipment(GameObject* actor,
                                          EquipmentCategory cat,
                                          int damage);

  extern std::string TryToDamageEquipment(GameObject* actor,
                                          ItemComponent* item,
                                          int damage);

  extern int CalculateDamageValue(GameObject* attacker,
                                  GameObject* defender,
                                  ItemComponent* weapon,
                                  bool meleeAttackWithRangedWeapon);

  extern int CalculateHitChanceMelee(GameObject* attacker,
                                     GameObject* defender,
                                     bool linear = false);

  extern int CalculateHitChanceRanged(const Position& start,
                                      const Position& end,
                                      GameObject* user,
                                      ItemComponent* weapon,
                                      bool isThrowing);

  extern int GetTotalDamageAbsorptionValue(GameObject* who, bool magic);

  extern StringV ProcessPhysicalDamage(GameObject* who,
                                         GameObject* from,
                                         int& amount,
                                         bool ignoreArmor);

  extern std::string ProcessMagicalDamage(GameObject* who,
                                          GameObject* from,
                                          int& amount);

  extern void ProcessManaShield(GameObject* who,
                                int amount);

  extern int ProcessThorns(GameObject* who,
                           int damageReceived);

  extern StringV DamageArmor(GameObject* who,
                              GameObject* from,
                              int amount);

  extern Position GetRandomPointAround(GameObject* user,
                                       ItemComponent* weapon,
                                       const Position& aroundThis);

  extern std::vector<Position>
  ProcessLaserAttack(GameObject* user,
                     const std::pair<int, int>& damageRange,
                     const Position& end);

  extern std::vector<Position> ProcessLaserAttack(GameObject* user,
                                                  ItemComponent* weapon,
                                                  const Position& end);

  extern void DrawLaserAttack(const std::vector<Position>& line);

  extern bool ShouldAwardExp(GameObjectType type);

  extern std::string GetDestroyedByMapString(GameObject* what,
                                             GameObject* tileRef);

  // ---------------------------------------------------------------------------

  extern uint32_t GetItemInventoryColor(const ItemData& data);

  extern std::string GenerateName(bool allowDoubleVowels = false,
                                  bool canAddEnding = false,
                                  const StringV& endings = StringV());

  extern std::string ReplaceItemPrefix(const std::string& oldIdentifiedName,
                                       const StringV& anyOf,
                                       const std::string& replaceWith);

  extern std::string GetCurrentDateTimeString(bool osFriendly = false);

  extern std::pair<int, int> GetDayAndMonth();

  extern std::string GetTownName(const std::pair<int, int>& dm);

  extern std::string GetGameObjectDisplayCharacter(GameObject* obj);

  extern std::string GetFowName(GameObject* obj);

  extern size_t FindLongestStringLength(const StringV& list);

  extern std::vector<Position> GetAreaDamagePointsFrom(const Position& from,
                                                         int range);

  extern std::vector<ItemComponent*> GetItemsWithBonus(GameObject* actor,
                                                       ItemBonusType bonus);

  extern Position InvertDirection(const Position& dir);

  extern bool IsPlayer(GameObject* obj);

  extern bool CanBeSpawned(ItemComponent* ic);

  extern size_t CalculateItemHash(ItemComponent* item);
  extern void UpdateItemPrefix(ItemComponent* item, ItemPrefix prefixToSet);

  extern std::vector<char> CharByCharIndex;

  extern void PrepareChars();

  extern uint16_t BoolFlagsToMask(const std::vector<bool>& traverse);

  // ===========================================================================

  //
  // Produces random pair from pairs list according to weight value:
  // the greater the number, the more likely item will appear as a result.
  // Returns result in the form of a pair of value type rolled
  // and its weight in the list. Example list follows:
  //
  // weightsByType =
  // {
  //   { APPLE,   1 },
  //   { BANANA,  3 },
  //   { ORANGE, 10 }
  // };
  //
  template <typename Map>
  std::pair<typename Map::key_type, typename Map::mapped_type>
  WeightedRandom(const Map& weightsByType)
  {
    using ResultType = std::pair<typename Map::key_type,
                                 typename Map::mapped_type>;

    ResultType res = *weightsByType.begin();

    int sum = 0;
    for (auto& i : weightsByType)
    {
      sum += i.second;
    }

    int target = Game::gRng.RandomRange(1, sum + 1);

    for (auto& i : weightsByType)
    {
      if (target <= i.second)
      {
        res = { i.first, i.second };
        return res;
      }

      target -= i.second;
    }

    return res;
  }

  // ===========================================================================

  template <typename T>
  std::unordered_map<T, double>
  WeightsToProbability(const std::unordered_map<T, int>& weightsMap)
  {
    std::unordered_map<T, double> res;

    int totalWeight = 0;
    for (auto& kvp : weightsMap)
    {
      totalWeight += kvp.second;
    }

    for (auto& kvp : weightsMap)
    {
      double p = static_cast<double>(kvp.second) / totalWeight;
      res[kvp.first] = p;
    }

    return res;
  }

  // ===========================================================================

  template <typename T>
  std::unordered_map<T, int>
  RollWeightsMap(const std::unordered_map<T, int>& weightsMap, int rolls)
  {
    std::unordered_map<T, int> res;

    for (int i = 0; i < rolls; i++)
    {
      auto r = Util::WeightedRandom(weightsMap);
      res[r.first] += 1;
    }

    return res;
  }

  // ===========================================================================

  template <typename key, typename value>
  std::unordered_map<value, key>
  FlipMap(const std::unordered_map<key, value>& src)
  {
    std::unordered_map<value, key> ret;

    std::transform(src.begin(),
                   src.end(),
                   std::inserter(ret, ret.begin()),
                   [](const std::pair<key, value>& p)
    {
      return std::pair<value, key>(p.second, p.first);
    });

    return ret;
  }

  // ===========================================================================

  template <typename key, typename value>
  std::map<value, key>
  FlipMap(const std::map<key, value>& src)
  {
    std::map<value, key> ret;

    std::transform(src.begin(),
                   src.end(),
                   std::inserter(ret, ret.begin()),
                   [](const std::pair<key, value>& p)
    {
      return std::pair<value, key>(p.second, p.first);
    });

    return ret;
  }

  // ===========================================================================

  template <typename ... Args>
  std::string StringFormat(const std::string& format, Args ... args)
  {
    //
    // People say that making such variables 'static' is actually bad for
    // performance (at least on modern processors). E.g.:
    //
    // https://softwareengineering.stackexchange.com/questions/350501/is-there-any-benefit-to-to-define-constant-local-variables-as-static-c
    //
    // But it looks like this is related to fundamental types like int or char.
    // Don't know about complex types, but I'd assume it doesn't hold.
    // At least not as simple as that.
    //
    // Actually, by making buffer string static we would make things even worse
    // in our case, because at some places in code we *have* to have a copy of
    // resulting string, so if we make buffer string static, we will copy this
    // big-ass buffer everywhere at those places.
    //
    std::string s;

    //
    // The following is a bit confusing, but here goes:
    //
    // snprintf() returns number of bytes WITHOUT \0
    //
    size_t size = snprintf(nullptr, 0, format.data(), args ...);
    if (!size)
    {
      return s;
    }

    //
    // Expand dong if needed.
    //
    s.resize(size);

    //
    // Get a nice pointer to underlying buffer for brevity.
    // And C-style cast it so we can write to it.
    //
    char *buf = (char*)s.data();

    //
    // 1 extra byte for '\0' since snprintf writes at most 'size' bytes
    // INCLUDING '\0'. So if we specify e.g. 3, it means 3 characters with '\0'.
    //
    // E.g. after:
    //
    // snprintf(buf, 3, "abc");
    //
    // buf will contain 'a', 'b' and '\0'
    //
    // std::string always contains \0 implicitly (so to speak), so std::string
    // of size 'size' will always "contain" '\0' at the end, making its "real"
    // size effectively ('size' + 1). This is not technically correct from the
    // std::string's interface point of view, because std::string::size()
    // returns real number of characters contained, without '\0', but you get
    // the idea.
    //
    snprintf(buf, size + 1, format.data(), args ...);

    return s;
  }

  // ===========================================================================

  template <typename T>
  std::string NumberToHexString(T num)
  {
    static_assert(std::is_integral<T>::value, "numbers only");

    std::stringstream ss;
    ss << std::hex << std::uppercase << num;
    return ss.str();
  }

  // ===========================================================================

  template <typename F>
  bool IsFunctionValid(const F& fn)
  {
    //
    // http://www.cplusplus.com/reference/functional/function/target_type/
    //
    // Return value
    // The type_info object that corresponds to the type of the target,
    // or typeid(void) if the object is an empty function.
    //
    return (fn.target_type() != typeid(void));
  }
}

// -----------------------------------------------------------------------------
#ifdef DEBUG_BUILD
//
// Helper macros for creating introspection info.
//
#define I_STR(spaces, value) \
  Util::StringFormat("%s  '%s': '%s',", spaces.data(), #value, value.data())

#define I_STR_NAMED(spaces, name, value) \
  Util::StringFormat("%s  '%s': '%s',", spaces.data(), name, value.data())

#define I_CSTR(spaces, value) \
  Util::StringFormat("%s  '%s': '%s',", spaces.data(), #value, value)

#define I_CSTR_NAMED(spaces, name, value) \
  Util::StringFormat("%s  '%s': '%s',", spaces.data(), name, value)

#define I_ULL(spaces, value) \
  Util::StringFormat("%s  '%s': %llu,", spaces.data(), #value, value)

#define I_CLR(spaces, color) \
  Util::StringFormat("%s  '%s': %06X,", spaces.data(), #color, color)

#define I_INT(spaces, value) \
  Util::StringFormat("%s  '%s': %d,", spaces.data(), #value, value)

#define I_BOOL(spaces, value) \
  Util::StringFormat("%s  '%s': %u,", spaces.data(), #value, value)

#define I_PTR(spaces, addr) \
  Util::StringFormat("%s  '%s': 0x%lX,", spaces.data(), #addr, addr)

#define I_PTR_NAMED(spaces, name, addr) \
  Util::StringFormat("%s  '%s': 0x%lX,", spaces.data(), name, addr)

#define I_OBJ_START(spaces, thisPtr) \
  Util::StringFormat("%s'0x%lX': {", spaces.data(), thisPtr)

#define I_OBJ_START_NAMED(spaces, name) \
  Util::StringFormat("%s'%s': {", spaces.data(), name)

#define I_OBJ_END(spaces) \
  Util::StringFormat("%s},", spaces.data())

#define I_EMPTY(spaces, name) \
  Util::StringFormat("%s  '%s': {},", spaces.data(), name)

#define DUMP_ATTR()                                         \
  [this]()                                                  \
  {                                                         \
    bool firstObj = true;                                   \
                                                            \
    std::string modsByObjs;                                 \
    for (auto& kvp : _modifiersByGoId)                      \
    {                                                       \
      if (!firstObj)                                        \
      {                                                     \
        modsByObjs += ",";                                  \
      }                                                     \
                                                            \
      bool firstMod = true;                                 \
      auto modStr = Util::StringFormat("%llu(", kvp.first); \
      for (const int& modifier : kvp.second)                \
      {                                                     \
        if (!firstMod)                                      \
        {                                                   \
          modStr += ",";                                    \
        }                                                   \
                                                            \
        modStr += Util::StringFormat("%d", modifier);       \
                                                            \
        firstMod = false;                                   \
      }                                                     \
      modStr += ")";                                        \
      modsByObjs += modStr;                                 \
      firstObj = false;                                     \
    }                                                       \
                                                            \
    return modsByObjs;                                      \
  }()

extern StringV DumpObj(void* ptr);
#endif
// -----------------------------------------------------------------------------

#endif
