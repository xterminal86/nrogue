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
#include "room-helper.h"
#include "item-data.h"
#include "singleton.h"

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

// Coded Map
using CM = std::vector<std::vector<std::pair<uint32_t, uint32_t>>>;

// Coded String
using CS = std::vector<uint32_t>;

class GameObject;

class Util : public Singleton<Util>
{
  public:
    std::vector<std::string> StringSplit(const std::string& str,
                                          char delim);

    std::vector<std::string> DecodeMap(const CM& map);
    std::string DecodeString(const CS& str);

    bool IsObjectInRange(GameObject* checker,
                          GameObject* checked,
                          int range);

    bool IsObjectInRange(const Position& posToCheckFrom,
                          const Position& objectPositionToCheck,
                          int rangeX,
                          int rangeY);

    std::vector<GameObject*> FindObjectsInRange(GameObject* aroundWho,
                                                 const std::vector<std::vector<std::unique_ptr<GameObject>>>& where,
                                                 int range);

    std::vector<GameObject*> FindObjectsInRange(GameObject* aroundWho,
                                                 const std::vector<std::unique_ptr<GameObject>>& where,
                                                 int range);

    bool IsBase64(unsigned char c);
    std::string Base64_Encode(unsigned char const* bytes_to_encode,
                               unsigned int in_len);
    std::string Base64_Decode(const std::string& encoded_string);

    std::vector<unsigned char> ConvertStringToBytes(const std::string& encodedStr);

    bool CheckLimits(const Position& posToCheck, const Position& limits);

    std::vector<Position> BresenhamLine(int sx, int sy, int ex, int ey);
    std::vector<Position> BresenhamLine(const Position& start,
                                         const Position& end);

    bool IsInsideMap(const Position& pos,
                      const Position& mapSize,
                      bool leaveBorders = true);

    int Clamp(int value, int min, int max);

    int BlockDistance(int x1, int y1, int x2, int y2);
    int BlockDistance(const Position& from, const Position& to);

    float LinearDistance(int x1, int y1, int x2, int y2);
    float LinearDistance(const Position& s, const Position& e);

    double Log2(uint64_t n);

    std::vector<Position> GetEightPointsAround(const Position& pos,
                                                 const Position& mapSize);

    std::vector<Position> GetScreenRect(int x1, int y1, int x2, int y2);
    std::vector<Position> GetScreenRectPerimeter(int x1, int y1, int x2, int y2, bool includeCorners = true);
    std::vector<Position> GetScreenRectAroundPoint(int pointX, int pointY, int rangeX, int rangeY);
    std::vector<Position> GetRectAroundPoint(int pointX, int pointY, int rangeX, int rangeY, const Position& mapSize);

    std::vector<Position> GetPerimeter(int x, int y, int w, int h, bool includeCorners = true);
    std::vector<Position> GetPerimeterAroundPoint(int x, int y, int w, int h, bool includeCorners = true);
    std::vector<GameObject*> GetActorsInRange(GameObject* from, int range);
    std::vector<GameObject*> GetContainersInRange(GameObject* from, int range);

    std::string ChooseRandomName();
    std::string NumberToHexString(int num);

    std::vector<std::string> RotateRoomLayout(const std::vector<std::string>& layout, RoomLayoutRotation r);

    int Rolld100();

    bool Rolld100(int successChance);

    int RollDices(int numRolls, int diceSides);

    int GetExpForNextLevel(int curLvl);

    void RecalculateWandStats(ItemComponent* wand);
    void BlessItem(ItemComponent* item);

    void Sleep(int delayMs);

    bool WaitForMs(uint64_t delayMs, bool reset = false);

    void PrintVector(const std::string& title, const std::vector<Position>& v);
    void PrintLayout(const std::vector<std::string>& l);

    void LaunchProjectile(const Position& from,
                           const Position& to,
                           char image,
                           const uint32_t& fgColor,
                           const uint32_t& bgColor = Colors::BlackColor);

    void LaunchProjectile(const std::vector<Position>& line,
                           char image,
                           const uint32_t& fgColor,
                           const uint32_t& bgColor = Colors::BlackColor);

    void KnockBack(GameObject* sender,
                    GameObject* receiver,
                    const Position& attackDir,
                    int tiles);

    void ReapplyBonuses(GameObject* onWho, ItemComponent* item);

    std::string ProcessTeleport(GameObject* target);

    std::pair<char, uint32_t> GetProjectileImageAndColor(ItemComponent* weapon,
                                                           bool throwingFromInventory);

    std::vector<GameObject*> GetObjectsOnTheLine(const std::vector<Position>& line);
    GameObject* GetFirstObjectOnTheLine(const std::vector<Position>& line);

    // ===========================================================================

    std::pair<bool, std::string> TryToDamageObject(GameObject* object,
                                                    GameObject* from,
                                                    int amount,
                                                    bool againstRes);

    std::string TryToDamageEquipment(GameObject* actor,
                                       EquipmentCategory cat,
                                       int damage);

    std::string TryToDamageEquipment(GameObject* actor,
                                      ItemComponent* item,
                                      int damage);

    int CalculateDamageValue(GameObject* attacker,
                              GameObject* defender,
                              ItemComponent* weapon,
                              bool meleeAttackWithRangedWeapon);

    int CalculateHitChanceMelee(GameObject* attacker,
                                  GameObject* defender);

    int CalculateHitChanceRanged(const Position& start,
                                   const Position& end,
                                   GameObject* user,
                                   ItemComponent* weapon,
                                   bool isThrowing);

    int GetTotalDamageAbsorptionValue(GameObject* who, bool magic);

    std::vector<std::string> ProcessPhysicalDamage(GameObject* who,
                                                     GameObject* from,
                                                     int& amount,
                                                     bool ignoreArmor);

    std::string ProcessMagicalDamage(GameObject* who,
                                       GameObject* from,
                                       int& amount);

    void ProcessManaShield(GameObject* who,
                            int amount);

    int ProcessThorns(GameObject* who,
                       int damageReceived);

    std::vector<std::string> DamageArmor(GameObject* who,
                                          GameObject* from,
                                          int amount);

    Position GetRandomPointAround(GameObject* user,
                                   ItemComponent* weapon,
                                   const Position& aroundThis);

    std::vector<Position> ProcessLaserAttack(GameObject* user,
                                              const std::pair<int, int>& damageRange,
                                              const Position& end);

    std::vector<Position> ProcessLaserAttack(GameObject* user,
                                              ItemComponent* weapon,
                                              const Position& end);

    void DrawLaserAttack(const std::vector<Position>& line);

    // ===========================================================================

    CharArray2D StringsArray2DToCharArray2D(const StringsArray2D& map);

    uint32_t GetItemInventoryColor(const ItemData& data);

    std::string GenerateName(bool allowDoubleVowels = false,
                              bool canAddEnding = false,
                              const std::vector<std::string>& endings = std::vector<std::string>());

    std::string ReplaceItemPrefix(const std::string& oldIdentifiedName,
                                   const std::vector<std::string>& anyOf,
                                   const std::string& replaceWith);

    std::string GetCurrentDateTimeString();

    std::string GetGameObjectDisplayCharacter(GameObject* obj);

    size_t FindLongestStringLength(const std::vector<std::string>& list);

    std::vector<Position> GetAreaDamagePointsFrom(const Position& from, int range);

    std::vector<ItemComponent*> GetItemsWithBonus(GameObject* actor,
                                                   ItemBonusType bonus);

    Position InvertDirection(const Position& dir);

    bool IsPlayer(GameObject* obj);

    bool CanBeSpawned(ItemComponent* ic);

    size_t CalculateItemHash(ItemComponent* item);

    // ===========================================================================

    /// Produces random pair from pairs list according to weight value:
    /// the greater the number, the more likely item will appear as a result.
    /// Returns result in the form of a pair of value type rolled
    /// and its weight in the list. Example list follows:
    /// weightsByType =
    /// {
    ///   { APPLE,   1 },
    ///   { BANANA,  3 },
    ///   { ORANGE, 10 }
    /// };
    template <typename T>
    std::pair<T, int> WeightedRandom(const std::map<T, int>& weightsByType)
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
          std::pair<T, int> res = { i.first, i.second };
          return res;
        }

        target -= i.second;
      }

      return *weightsByType.begin();
    }

    template <typename T>
    std::map<T, double> WeightsToProbability(const std::map<T, int>& weightsMap)
    {
      std::map<T, double> res;

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

    template <typename T>
    std::map<T, int> RollWeightsMap(const std::map<T, int>& weightsMap, int rolls)
    {
      std::map<T, int> res;

      for (int i = 0; i < rolls; i++)
      {
        auto r = Util::Instance().WeightedRandom(weightsMap);
        res[r.first] += 1;
      }

      return res;
    }


    template <typename key, typename value>
    std::map<value, key> FlipMap(const std::map<key, value>& src)
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

    template<typename ... Args>
    std::string StringFormat(const std::string& format, Args ... args)
    {
      size_t size = snprintf(nullptr, 0, format.c_str(), args ...);
      std::string s;

      if (!size)
      {
        return s;
      }

      s.resize(size);
      char *buf = (char *)s.data();
      snprintf(buf, size + 1, format.c_str(), args ...);
      return s;
    }

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

  protected:
    void InitSpecific() override;

  private:
    const uint8_t ReservationSize = 128;

    std::vector<std::string>    _vectorString;
    std::vector<GameObject*>    _vectorGameObjectP;
    std::vector<ItemComponent*> _vectorItemComponentP;
    std::vector<unsigned char>  _vectorUnsignedChar;
    std::vector<Position>       _vectorPosition;

    std::string _string;

    std::pair<char, uint32_t>    _pairCharUint32;
    std::pair<bool, std::string> _pairBoolString;

    CharArray2D _charArray2D;
};

#endif
