#include "util.h"

#include "application.h"
#include "printer.h"
#include "logger.h"
#include "timer.h"
#include "map.h"

namespace Util
{
  std::vector<std::string> StringSplit(const std::string& str, char delim)
  {
    std::vector<std::string> res;

    std::string toSearch = str;

    while (true)
    {
      bool found = false;

      for (size_t i = 0; i < toSearch.length(); i++)
      {
        if (toSearch[i] == delim)
        {
          found = true;

          std::string before = toSearch.substr(0, i);

          if (i + 1 < toSearch.length())
          {
            toSearch = toSearch.substr(i + 1, toSearch.length() - i + 1);
          }
          else
          {
            found = false;
            break;
          }

          res.push_back(before);

          break;
        }
      }

      if (!found)
      {
        if (res.empty())
        {
          res = { str };
        }
        else
        {
          res.push_back(toSearch);
        }

        break;
      }
    }

    return res;
  }

  // ===========================================================================
  //
  // Makes no sense since it's open source, but anyway.
  // At least we can cover every not-so-important stuff with a fig leaf.
  //
  // ===========================================================================
  std::vector<std::string> DecodeMap(const CM& map)
  {
    std::vector<std::string> res;

    for (auto& block : map)
    {
      std::string line;

      for (auto& pair : block)
      {
        uint8_t charIndex = (pair.first & 0x000000FF);
        uint8_t charCount = (pair.second & 0x000000FF);
        char c = Application::Instance().CharByCharIndex(charIndex);
        line.append(charCount, c);
      }

      res.push_back(line);
    }

    return res;
  }

  std::string DecodeString(const CS& str)
  {
    std::string res;

    for (auto& i : str)
    {
      uint8_t charIndex = (i & 0x000000FF);
      char c = Application::Instance().CharByCharIndex(charIndex);
      res.push_back(c);
    }

    return res;
  }

  // ===========================================================================

  bool IsObjectInRange(const Position& posToCheckFrom,
                       const Position& objectPositionToCheck,
                       int rangeX,
                       int rangeY)
  {
    int px = posToCheckFrom.X;
    int py = posToCheckFrom.Y;

    int x = objectPositionToCheck.X;
    int y = objectPositionToCheck.Y;

    int lx = x - rangeX;
    int ly = y - rangeY;
    int hx = x + rangeX;
    int hy = y + rangeY;

    return (px >= lx && px <= hx
         && py >= ly && py <= hy);
  }

  // *** https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c

  bool IsBase64(unsigned char c)
  {
    auto res = std::find(GlobalConstants::Base64Chars.begin(),
                         GlobalConstants::Base64Chars.end(),
                         c);

    return (res != GlobalConstants::Base64Chars.end());
  }

  std::string Base64_Encode(unsigned char const* bytes_to_encode, unsigned int in_len)
  {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--)
    {
      char_array_3[i++] = *(bytes_to_encode++);
      if (i == 3)
      {
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for(i = 0; (i < 4) ; i++)
        {
          ret += GlobalConstants::Base64Chars[char_array_4[i]];
        }

        i = 0;
      }
    }

    if (i)
    {
      for(j = i; j < 3; j++)
      {
        char_array_3[j] = '\0';
      }

      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for (j = 0; (j < i + 1); j++)
      {
        ret += GlobalConstants::Base64Chars[char_array_4[j]];
      }

      while((i++ < 3))
      {
        ret += '=';
      }
    }

    return ret;
  }

  std::string Base64_Decode(const std::string& encoded_string)
  {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && ( encoded_string[in_] != '=') && IsBase64(encoded_string[in_]))
    {
      char_array_4[i++] = encoded_string[in_]; in_++;
      if (i ==4)
      {
        for (i = 0; i <4; i++)
        {
          char_array_4[i] = GlobalConstants::Base64Chars.find(char_array_4[i]);
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (i = 0; (i < 3); i++)
        {
          ret += char_array_3[i];
        }

        i = 0;
      }
    }

    if (i)
    {
      for (j = i; j <4; j++)
      {
        char_array_4[j] = 0;
      }

      for (j = 0; j <4; j++)
      {
        char_array_4[j] = GlobalConstants::Base64Chars.find(char_array_4[j]);
      }

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (j = 0; (j < i - 1); j++)
      {
        ret += char_array_3[j];
      }
    }

    return ret;
  }

  // ===========================================================================

  std::vector<unsigned char> ConvertStringToBytes(const std::string& encodedStr)
  {
    std::vector<unsigned char> byteArray;
    for (auto& c : encodedStr)
    {
        byteArray.push_back(c);
    }

    return byteArray;
  }

  bool CheckLimits(const Position& posToCheck, const Position& limits)
  {
    if (posToCheck.X >= 0 && posToCheck.X < limits.X
     && posToCheck.Y >= 0 && posToCheck.Y < limits.Y)
    {
      return true;
    }

    return false;
  }

  /// Bresenham line including end point
  std::vector<Position> BresenhamLine(int sx, int sy, int ex, int ey)
  {
    int x1 = sx;
    int y1 = sy;
    int x2 = ex;
    int y2 = ey;

    std::vector<Position> result;

    // Bresenham's line algorithm
    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));

    if(steep)
    {
      std::swap(x1, y1);
      std::swap(x2, y2);
    }

    if(x1 > x2)
    {
      std::swap(x1, x2);
      std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    Position pos;

    for(int x = (int)x1; x < maxX; x++)
    {
      if(steep)
      {
        pos.X = y;
        pos.Y = x;
      }
      else
      {
        pos.X = x;
        pos.Y = y;
      }

      result.push_back(pos);

      error -= dy;
      if(error < 0)
      {
        y += ystep;
        error += dx;
      }
    }

    if(steep)
    {
      pos.X = y2;
      pos.Y = x2;
    }
    else
    {
      pos.X = x2;
      pos.Y = y2;
    }

    result.push_back(pos);

    // Ensure that line always goes from source point

    bool cond = (result[0].X == sx && result[0].Y == sy);
    if (!cond)
    {
      std::reverse(result.begin(), result.end());
    }

    // result.erase(result.begin());

    return result;
  }

  std::vector<Position> BresenhamLine(const Position& start, const Position& end)
  {
    return BresenhamLine(start.X, start.Y, end.X, end.Y);
  }

  bool IsInsideMap(const Position& pos, const Position& mapSize, bool leaveBorders)
  {
    int offset = leaveBorders ? 1 : 0;

    return (pos.X >= offset
         && pos.X < mapSize.X - offset
         && pos.Y >= offset
         && pos.Y < mapSize.Y - offset);
  }

  int Clamp(int value, int min, int max)
  {
    return std::max(min, std::min(value, max));
  }

  int BlockDistance(int x1, int y1, int x2, int y2)
  {
     return abs(y2 - y1) + abs(x2 - x1);
  }

  int BlockDistance(const Position& from, const Position& to)
  {
     return abs(to.Y - from.Y) + abs(to.X - from.X);
  }

  float LinearDistance(int x1, int y1, int x2, int y2)
  {
    float d = std::sqrt(std::pow(x2 - x1, 2) + std::pow(y2 - y1, 2));
    return d;
  }

  float LinearDistance(const Position& s, const Position& e)
  {
    return LinearDistance(s.X, s.Y, e.X, e.Y);
  }

  std::vector<Position> GetEightPointsAround(const Position& pos, const Position& mapSize)
  {
    std::vector<Position> result;

    int lx = pos.X - 1;
    int ly = pos.Y - 1;
    int hx = pos.X + 1;
    int hy = pos.Y + 1;

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        if (x == pos.X && y == pos.Y)
        {
          continue;
        }

        Position p(x, y);

        if (IsInsideMap(p, mapSize))
        {
          result.push_back(p);
        }
      }
    }

    return result;
  }

  std::vector<Position> GetScreenRect(int x1, int y1, int x2, int y2)
  {
    std::vector<Position> result;

    int tw = Printer::TerminalWidth;
    int th = Printer::TerminalHeight;

    int lx = x1;
    int ly = y1;
    int hx = x2;
    int hy = y2;

    lx = Clamp(lx, 0, tw - 1);
    ly = Clamp(ly, 0, th - 1);
    hx = Clamp(hx, 0, tw - 1);
    hy = Clamp(hy, 0, th - 1);

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        Position p(x, y);
        result.push_back(p);
      }
    }

    return result;
  }

  std::vector<Position> GetScreenRectPerimeter(int x1, int y1, int x2, int y2, bool includeCorners)
  {
    std::vector<Position> res;

    int tw = Printer::TerminalWidth;
    int th = Printer::TerminalHeight;

    int lx = x1;
    int ly = y1;
    int hx = x2;
    int hy = y2;

    lx = Clamp(lx, 0, tw - 1);
    ly = Clamp(ly, 0, th - 1);
    hx = Clamp(hx, 0, tw - 1);
    hy = Clamp(hy, 0, th - 1);

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        bool condCorners = (x == x1 && y == y1)
                        || (x == x1 && y == y2)
                        || (x == x2 && y == y1)
                        || (x == x2 && y == y2);

        if (!includeCorners && condCorners)
        {
          continue;
        }

        bool cond = (x == x1 || x == x2 || y == y1 || y == y2);
        if (cond)
        {
          res.push_back(Position(x, y));
        }
      }
    }

    return res;
  }

  /// Clamps values against terminal size,
  /// useful for drawing GUI related stuff and everything.
  std::vector<Position> GetScreenRectAroundPoint(int pointX, int pointY, int rangeX, int rangeY)
  {
    std::vector<Position> result;

    int tw = Printer::TerminalWidth;
    int th = Printer::TerminalHeight;

    int lx = pointX - rangeX;
    int ly = pointY - rangeY;
    int hx = pointX + rangeX;
    int hy = pointY + rangeY;

    lx = Clamp(lx, 0, tw - 1);
    ly = Clamp(ly, 0, th - 1);
    hx = Clamp(hx, 0, tw - 1);
    hy = Clamp(hy, 0, th - 1);

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        Position p(x, y);
        result.push_back(p);
      }
    }

    return result;
  }

  std::vector<Position> GetRectAroundPoint(int pointX, int pointY, int rangeX, int rangeY, const Position& mapSize)
  {
    std::vector<Position> result;

    int mw = mapSize.X;
    int mh = mapSize.Y;

    int lx = pointX - rangeX;
    int ly = pointY - rangeY;
    int hx = pointX + rangeX;
    int hy = pointY + rangeY;

    lx = Clamp(lx, 0, mw - 1);
    ly = Clamp(ly, 0, mh - 1);
    hx = Clamp(hx, 0, mw - 1);
    hy = Clamp(hy, 0, mh - 1);

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        Position p(x, y);
        result.push_back(p);
      }
    }

    return result;
  }

  std::vector<Position> GetPerimeter(int x, int y, int w, int h, bool includeCorners)
  {
    std::vector<Position> res;

    int x1 = x;
    int x2 = x + w;
    int y1 = y;
    int y2 = y + h;

    for (int i = x1; i <= x2; i++)
    {
      for (int j = y1; j <= y2; j++)
      {
        bool condCorners = (i == x1 && j == y1)
                        || (i == x1 && j == y2)
                        || (i == x2 && j == y1)
                        || (i == x2 && j == y2);

        if (!includeCorners && condCorners)
        {
          continue;
        }

        bool cond = (i == x1 || i == x2 || j == y1 || j == y2);
        if (cond)
        {
          res.push_back(Position(i, j));
        }
      }
    }

    return res;
  }

  std::vector<Position> GetPerimeterAroundPoint(int x, int y, int w, int h, bool includeCorners)
  {
    return GetPerimeter(x - w, y - h, w * 2, h * 2, includeCorners);
  }

  std::string ChooseRandomName()
  {
    int index = RNG::Instance().Random() % GlobalConstants::RandomNames.size();
    return GlobalConstants::RandomNames[index];
  }

  std::string NumberToHexString(int num)
  {
    std::string res;

    std::stringstream ss;
    ss << std::hex << std::uppercase << num;

    res = ss.str();

    if (res.length() == 1)
    {
      res.insert(res.begin(), '0');
    }

    return res;
  }

  /// Rotates room text layout, provided all lines are of equal length
  std::vector<std::string> RotateRoomLayout(const std::vector<std::string>& layout, RoomLayoutRotation r)
  {
    std::vector<std::string> res;

    switch (r)
    {
      case RoomLayoutRotation::CCW_90:
      {
        // Swap dimensions
        int lineLen = layout[0].length();

        for (int i = 0; i < lineLen; i++)
        {
          res.push_back(std::string(layout.size(), '_'));
        }

        int maxX = res.size() - 1;
        //int maxY = res[0].length() - 1;

        // In case of 3x5 size:
        //
        // 0;0 -> 3;0
        // 0;1 -> 2;0
        // 0;2 -> 1;0
        // 0;3 -> 0;0
        //
        // 1;0 -> 3;1
        // 1;1 -> 2;1
        // ...

        for (size_t x = 0; x < layout.size(); x++)
        {
          for (size_t y = 0; y < layout[x].length(); y++)
          {
            char c = layout[x][y];

            switch (c)
            {
              case '_':
              case '-':
                c = '|';
                break;

              case '|':
                c = '-';
                break;

              case '/':
                c = '\\';
                break;

              case '\\':
                c = '/';
                break;
            }

            res[maxX - y][x] = c;
          }
        }
      }
      break;

      case RoomLayoutRotation::CCW_180:
      {
        res = layout;

        int maxY = res[0].length() - 1;

        // Swap columns then rows
        for (size_t x = 0; x < layout.size(); x++)
        {
          for (size_t y = 0; y < layout[x].length(); y++)
          {
            res[x][maxY - y] = layout[x][y];
          }
        }

        std::reverse(res.begin(), res.end());
      }
      break;

      case RoomLayoutRotation::CCW_270:
      {
        // Swap dimensions
        int lineLen = layout[0].length();

        for (int i = 0; i < lineLen; i++)
        {
          res.push_back(std::string(layout.size(), '_'));
        }

        //int maxX = res.size() - 1;
        int maxY = res[0].length() - 1;

        // In case of 3x5 size:
        //
        // 0;0 -> 0;5
        // 0;1 -> 1;5
        // 0;2 -> 2;5
        // 0;3 -> 3;5
        //
        // 1;0 -> 0;4
        // 1;1 -> 1;4
        // ...

        for (size_t x = 0; x < layout.size(); x++)
        {
          for (size_t y = 0; y < layout[x].length(); y++)
          {
            char c = layout[x][y];

            switch (c)
            {
              case '_':
              case '-':
                c = '|';
                break;

              case '|':
                c = '-';
                break;

              case '/':
                c = '\\';
                break;

              case '\\':
                c = '/';
                break;
            }

            res[y][maxY - x] = c;
          }
        }
      }
      break;

      default:
        res = layout;
        break;
    }

    return res;
  }

  bool Rolld100(int successChance)
  {
    int dice = RNG::Instance().RandomRange(1, 101);

    auto logMsg = StringFormat("\t%s: %i <= %i - %s",
                               __PRETTY_FUNCTION__,
                                dice,
                               successChance,
                               (dice <= successChance) ? "passed" : "failed");

    Logger::Instance().Print(logMsg);

    if (dice <= successChance)
    {
      return true;
    }

    return false;
  }

  int RollDamage(int numRolls, int diceSides)
  {
    int totalDamage = 0;

    for (int i = 0; i < numRolls; i++)
    {
      int dmg = RNG::Instance().RandomRange(1, diceSides + 1);
      totalDamage += dmg;
    }

    return totalDamage;
  }

  int Rolld100()
  {
    int dice = RNG::Instance().RandomRange(1, 101);

    auto str = StringFormat("\t%s: rolled = %i", __PRETTY_FUNCTION__, dice);
    Logger::Instance().Print(str);

    return dice;
  }

  void Sleep(int delayMs)
  {
    // NOTE: Application can freeze on SDL_Delay().
    //
    // At least on Windows if you listen through a long NPC text
    // and at the same time click on other window to lose focus of
    // this one, text (and the whole window) will freeze and then "unfreeze"
    // possibly after specified delay passes in "background".
    // It seems there's no difference in whether you use SDL_Delay() or C++ chrono library.

    #ifdef USE_SDL
    SDL_Delay(delayMs);
    #else
    auto tp1 = std::chrono::high_resolution_clock::now();
    auto tp2 = tp1;
    while (std::chrono::duration_cast<Ms>(tp1 - tp2).count() < delayMs)
    {
      tp1 = std::chrono::high_resolution_clock::now();
    }
    #endif
  }

  bool WaitForMs(uint64_t delayMs, bool reset)
  {
    auto timePassed = Timer::Instance().TimePassed();
    Ms s = std::chrono::duration_cast<Ms>(timePassed);
    static uint64_t prevMs = s.count();

    // If WaitForMs() hasn't been called for some time,
    // wthout this check the condition will immediately
    // succeed since s.count() - prevMs will be a very big value,
    // thus the first "run" of this function in some animation
    // loop, for example, will yield wrong result, since
    // at first run there won't be any 'delayMs' waiting.
    if (reset)
    {
      prevMs = s.count();
      return true;
    }

    if (s.count() - prevMs > delayMs)
    {
      prevMs = s.count();
      return true;
    }

    return false;
  }

  void PrintVector(const std::string& title, const std::vector<Position>& v)
  {
    DebugLog("Vector of '%s' (size %u)\n", title.data(), v.size());

    for (auto& p : v)
    {
      DebugLog("[%i;%i] ", p.X, p.Y);
    }

    DebugLog("\n");
  }

  void PrintLayout(const std::vector<std::string>& l)
  {
    std::string dbg = "\n";

    for (size_t x = 0; x < l.size(); x++)
    {
      for (size_t y = 0; y < l[x].length(); y++)
      {
        dbg += l[x][y];
      }

      dbg += "\n";
    }

    Logger::Instance().Print(dbg);
  }

  CharArray2D StringsArray2DToCharArray2D(const StringsArray2D& map)
  {
    CharArray2D res;

    for (auto& row : map)
    {
      std::vector<char> r;

      for (auto& c : row)
      {
        r.push_back(c);
      }

      res.push_back(r);
    }

    return res;
  }

  std::string GetItemInventoryColor(const ItemData& data)
  {
    std::string textColor = "#FFFFFF";

    bool isBlessed = (data.Prefix == ItemPrefix::BLESSED);
    bool isCursed  = (data.Prefix == ItemPrefix::CURSED);
    bool isMagic   = (data.Rarity == ItemRarity::MAGIC);
    bool isRare    = (data.Rarity == ItemRarity::RARE);
    bool isUnique  = (data.Rarity == ItemRarity::UNIQUE);
    bool isMixed   = (isCursed && (isMagic || isRare));

    // Iterate over this map and select color
    // for the first entry found with bool key == true.
    std::map<int, std::pair<bool, std::string>> itemFirstColorToChoose =
    {
      { 0, { isMixed,   Colors::ItemMixedColor   } },
      { 1, { isMagic,   Colors::ItemMagicColor   } },
      { 2, { isRare,    Colors::ItemRareColor    } },
      { 3, { isUnique,  Colors::ItemUniqueColor  } },
      { 4, { isBlessed, Colors::ItemMagicColor   } },
      { 5, { isCursed,  Colors::ItemCursedColor  } }
    };

    for (auto& kvp : itemFirstColorToChoose)
    {
      if (kvp.second.first)
      {
        textColor = kvp.second.second;
        break;
      }
    }

    if (!data.IsIdentified && !data.IsPrefixDiscovered)
    {
      textColor = "#FFFFFF";
    }

    return textColor;
  }

  std::string GenerateName(bool allowDoubleVowels,
                           bool canAddEnding,
                           const std::vector<std::string>& endings)
  {
    std::string name;

    int maxLength = RNG::Instance().RandomRange(2, 4);
    int addEnding = RNG::Instance().RandomRange(0, 2);

    std::string syllable;

    for (int i = 0; i < maxLength; i++)
    {
      while (true)
      {
        int mode = RNG::Instance().RandomRange(0, 2);

        int vowelIndex = 0, consIndex = 0;
        std::string vowel;
        std::string cons;
        std::string syl;

        switch (mode)
        {
          case 0:
          {
            vowelIndex = RNG::Instance().RandomRange(0, GlobalConstants::Vowels.length());
            consIndex  = RNG::Instance().RandomRange(0, GlobalConstants::Consonants.length());

            vowel = { GlobalConstants::Vowels[vowelIndex]    };
            cons  = { GlobalConstants::Consonants[consIndex] };

            syl = { cons + vowel };
          }
          break;

          case 1:
          {
            consIndex = RNG::Instance().RandomRange(0, GlobalConstants::Consonants.length());
            cons      = GlobalConstants::Consonants[consIndex];

            syl = cons;

            char lastVowel = 0;

            for (int i = 0; i < 2; i++)
            {
              vowelIndex = RNG::Instance().RandomRange(0, GlobalConstants::Vowels.length());

              char v = GlobalConstants::Vowels[vowelIndex];
              vowel  = { v };

              if (allowDoubleVowels ||
                (!allowDoubleVowels && (lastVowel != v)))
              {
                syl += vowel;
              }

              lastVowel = v;
            }
          }
          break;
        }

        syllable = syl;
        break;
      }

      name += syllable;
    }

    if (addEnding == 0 && canAddEnding && !endings.empty())
    {
      int endingIndex = RNG::Instance().RandomRange(0, endings.size());
      std::string ending = endings[endingIndex];

      if (!allowDoubleVowels)
      {
        if (name[name.length() - 1] == ending[0])
        {
          name.pop_back();
        }
      }

      name += ending;
    }

    name[0] = std::toupper(name[0]);

    return name;
  }

  std::string ReplaceItemPrefix(const std::string& oldIdentifiedName,
                                const std::vector<std::string>& anyOf,
                                const std::string& replaceWith)
  {
    std::string newIdentifiedName = oldIdentifiedName;

    for (auto& s : anyOf)
    {
      size_t pos = newIdentifiedName.find(s);
      if (pos != std::string::npos)
      {
        newIdentifiedName = newIdentifiedName.replace(pos, s.length(), replaceWith);
        break;
      }
    }

    return newIdentifiedName;
  }

  size_t FindLongestStringLength(const std::vector<std::string>& list)
  {
    size_t res = 0;

    for (auto& i : list)
    {
      if (i.length() > res)
      {
        res = i.length();
      }
    }

    return res;
  }

  void LaunchProjectile(const Position& from,
                               const Position& to,
                               char image,
                               const std::string& fgColor,
                               const std::string& bgColor)
  {
    auto line = BresenhamLine(from, to);

    int distanceCovered = 0;

    // Start from 1 to exclude starting position
    for (size_t i = 1; i < line.size(); i++)
    {
      int mx = line[i].X;
      int my = line[i].Y;

      Application::Instance().ForceDrawCurrentState();

      int drawingPosX = mx + Map::Instance().CurrentLevel->MapOffsetX;
      int drawingPosY = my + Map::Instance().CurrentLevel->MapOffsetY;

      Printer::Instance().PrintFB(drawingPosX,
                                  drawingPosY,
                                  image,
                                  fgColor,
                                  bgColor);

      Printer::Instance().Render();

      // FIXME: debug
      // Util::Sleep(100);

      //#ifndef USE_SDL
      //Util::Sleep(20);
      //#endif

      distanceCovered++;
    }
  }
}
