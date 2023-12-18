#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <climits>
#include <iomanip>

#include "rng.h"
#include "position.h"
#include "item-data.h"

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
    #define LogPrint(str, ...) Logger::Instance().Print(str, ##__VA_ARGS__)
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

  extern std::vector<GameObject*> FindObjectsInRange(GameObject* aroundWho,
                                                     const std::vector<std::vector<std::unique_ptr<GameObject>>>& where,
                                                     int range);

  extern std::vector<GameObject*> FindObjectsInRange(GameObject* aroundWho,
                                                     const std::vector<std::unique_ptr<GameObject>>& where,
                                                     int range);

  extern bool IsBase64(unsigned char c);
  extern std::string Base64_Encode(unsigned char const* bytes_to_encode,
                                   unsigned int in_len);
  extern std::string Base64_Decode(const std::string& encoded_string);

  extern std::string Encrypt(const std::string& str);

  extern std::vector<unsigned char> ConvertStringToBytes(const std::string& encodedStr);

  extern bool CheckLimits(const Position& posToCheck, const Position& limits);

  extern std::vector<Position> BresenhamLine(int sx, int sy, int ex, int ey);
  extern std::vector<Position> BresenhamLine(const Position& start,
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
  extern std::vector<Position> GetScreenRectPerimeter(int x1, int y1, int x2, int y2, bool includeCorners = true);
  extern std::vector<Position> GetScreenRectAroundPoint(int pointX, int pointY, int rangeX, int rangeY);
  extern std::vector<Position> GetRectAroundPoint(int pointX, int pointY, int rangeX, int rangeY, const Position& mapSize);

  extern std::vector<Position> GetPerimeter(int x, int y, int w, int h, bool includeCorners = true);
  extern std::vector<Position> GetPerimeterAroundPoint(int x, int y, int w, int h, bool includeCorners = true);
  extern std::vector<GameObject*> GetActorsInRange(GameObject* from, int range);
  extern std::vector<GameObject*> GetContainersInRange(GameObject* from, int range);

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

  extern void PrintVector(const std::string& title, const std::vector<Position>& v);
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

  extern std::pair<char, uint32_t> GetProjectileImageAndColor(ItemComponent* weapon,
                                                              bool throwingFromInventory);

  extern std::vector<GameObject*> GetObjectsOnTheLine(const std::vector<Position>& line);
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
                                     GameObject* defender);

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

  extern std::vector<Position> ProcessLaserAttack(GameObject* user,
                                                  const std::pair<int, int>& damageRange,
                                                  const Position& end);

  extern std::vector<Position> ProcessLaserAttack(GameObject* user,
                                                  ItemComponent* weapon,
                                                  const Position& end);

  extern void DrawLaserAttack(const std::vector<Position>& line);

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
    int sum = 0;
    for (auto& i : weightsByType)
    {
      sum += i.second;
    }

    int target = RNG::Instance().RandomRange(1, sum + 1);

    for (auto& i : weightsByType)
    {
      if (target <= i.second)
      {
        std::pair<typename Map::key_type, typename Map::mapped_type> res = { i.first, i.second };

        return res;
      }

      target -= i.second;
    }

    return *weightsByType.begin();
  }

  // ===========================================================================

  template <typename T>
  std::unordered_map<T, double> WeightsToProbability(const std::unordered_map<T, int>& weightsMap)
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
  std::unordered_map<T, int> RollWeightsMap(const std::unordered_map<T, int>& weightsMap, int rolls)
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
  std::unordered_map<value, key> FlipMap(const std::unordered_map<key, value>& src)
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

  template <typename ... Args>
  std::string StringFormat(const std::string& format, Args ... args)
  {
    size_t size = snprintf(nullptr, 0, format.data(), args ...);
    std::string s;

    if (!size)
    {
      return s;
    }

    s.resize(size);
    char *buf = (char *)s.data();
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

#endif
