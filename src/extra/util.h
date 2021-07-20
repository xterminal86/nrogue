#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <climits>

#include "rng.h"
#include "position.h"
#include "room-helper.h"
#include "item-data.h"

// We surround statements with one-shot do ... while loop
// to prevent accidental bugs due to unexpected execution
// after macro expansion.
// E.g. after no-curly-braced 'if' condition,
// only first expanded macro statement will be executed,
// which may be not what we wanted.
// Also this will make function-like macro usage syntax
// uniform with regular function syntax, which will
// be impossible if we just used { } braces, since, for example,
// trying to write 'DebugLog("хуй");' inside if...else{}
// won't compile, because it will be unfolded into compound
// statement with ; at the end of } brace, which will invalidate
// syntax of if...else form.
#ifndef USE_SDL
#define DebugLog(format, ...)       \
  do {                               \
  printf(format, ##__VA_ARGS__);     \
  fflush(stdout);                    \
  } while (false)
#else
#include "SDL2/SDL.h"
#define DebugLog(format, ...) SDL_Log(format, ##__VA_ARGS__)
#endif

namespace Util
{
  extern std::vector<std::string> StringSplit(const std::string& str, char delim);

  extern bool IsObjectInRange(const Position& posToCheckFrom,
                              const Position& objectToCheck,
                              int rangeX,
                              int rangeY);

  extern bool IsBase64(unsigned char c);
  extern std::string Base64_Encode(unsigned char const* bytes_to_encode,
                                   unsigned int in_len);
  extern std::string Base64_Decode(const std::string& encoded_string);

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

  extern float LinearDistance(int x1, int y1, int x2, int y2);
  extern float LinearDistance(const Position& s, const Position& e);

  extern std::vector<Position> GetEightPointsAround(const Position& pos,
                                                    const Position& mapSize);

  extern std::vector<Position> GetScreenRect(int x1, int y1, int x2, int y2);
  extern std::vector<Position> GetScreenRectPerimeter(int x1, int y1, int x2, int y2, bool includeCorners = true);
  extern std::vector<Position> GetScreenRectAroundPoint(int pointX, int pointY, int rangeX, int rangeY);
  extern std::vector<Position> GetRectAroundPoint(int pointX, int pointY, int rangeX, int rangeY, const Position& mapSize);

  extern std::vector<Position> GetPerimeter(int x, int y, int w, int h, bool includeCorners = true);
  extern std::vector<Position> GetPerimeterAroundPoint(int x, int y, int w, int h, bool includeCorners = true);

  extern std::string ChooseRandomName();

  extern std::vector<std::string> RotateRoomLayout(const std::vector<std::string>& layout, RoomLayoutRotation r);

  extern int Rolld100();
  extern bool Rolld100(int successChance);

  extern int RollDamage(int numRolls, int diceSides);

  extern void Sleep(int delayMs);

  extern bool WaitForMs(uint64_t delayMs, bool reset = false);

  extern void PrintVector(const std::string& title, const std::vector<Position>& v);
  extern void PrintLayout(const std::vector<std::string>& l);

  extern CharArray2D StringsArray2DToCharArray2D(const StringsArray2D& map);

  extern std::string GetItemInventoryColor(const ItemData& data);

  extern std::string GenerateName(bool allowDoubleVowels = false);

  extern std::string ReplaceItemPrefix(const std::string& oldIdentifiedName,
                                       const std::vector<std::string>& anyOf,
                                       const std::string& replaceWith);

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
  inline std::pair<T, int> WeightedRandom(const std::map<T, int>& weightsByType)
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

  template <typename key, typename value>
  inline std::map<value, key> FlipMap(const std::map<key, value>& src)
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
  inline std::string StringFormat(const std::string& format, Args ...args)
  {
    // Extra space for '\0'
    size_t size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );

    // We don't want the '\0' inside
    return std::string( buf.get(), buf.get() + size - 1 );
  }
}

#endif
