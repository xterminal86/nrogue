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

  bool IsObjectInRange(GameObject* checker,
                       GameObject* checked,
                       int range)
  {
    if (checker == nullptr || checked == nullptr)
    {
      DebugLog("[WAR] Util::IsObjectInRange() checker: [0x%X] checked: [0x%X]", checker, checked);
      return false;
    }

    return IsObjectInRange(checker->GetPosition(),
                           checked->GetPosition(),
                           range,
                           range);
  }

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

  std::vector<GameObject*> FindObjectsInRange(GameObject* aroundWho,
                                               const std::vector<std::vector<std::unique_ptr<GameObject>>>& where,
                                               int range)
  {
    std::vector<GameObject*> res;

    if (aroundWho == nullptr)
    {
      return res;
    }

    auto& mapSize = Map::Instance().CurrentLevel->MapSize;

    int lx = aroundWho->PosX - range;
    int hx = aroundWho->PosX + range;
    int ly = aroundWho->PosY - range;
    int hy = aroundWho->PosY + range;

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        if (!IsInsideMap({ x, y}, mapSize))
        {
          continue;
        }

        if (x == aroundWho->PosX && y == aroundWho->PosY)
        {
          continue;
        }

        auto go = where[x][y].get();
        if (go != nullptr && go->PosX == x && go->PosY == y)
        {
          res.push_back(go);
        }
      }
    }

    return res;
  }

  std::vector<GameObject*> FindObjectsInRange(GameObject* aroundWho,
                                              const std::vector<std::unique_ptr<GameObject>>& where,
                                              int range)
  {
    std::vector<GameObject*> res;

    if (aroundWho == nullptr)
    {
      return res;
    }

    auto& mapSize = Map::Instance().CurrentLevel->MapSize;

    int lx = aroundWho->PosX - range;
    int hx = aroundWho->PosX + range;
    int ly = aroundWho->PosY - range;
    int hy = aroundWho->PosY + range;

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        if (!IsInsideMap({ x, y}, mapSize))
        {
          continue;
        }

        if (x == aroundWho->PosX && y == aroundWho->PosY)
        {
          continue;
        }

        for (auto& i : where)
        {
          if (i->PosX == x && i->PosY == y)
          {
            res.push_back(i.get());
          }
        }
      }
    }

    return res;
  }

  //
  // https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
  //
  bool IsBase64(unsigned char c)
  {
    auto res = std::find(Strings::Base64Chars.begin(),
                         Strings::Base64Chars.end(),
                         c);

    return (res != Strings::Base64Chars.end());
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
          ret += Strings::Base64Chars[char_array_4[i]];
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
        ret += Strings::Base64Chars[char_array_4[j]];
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
          char_array_4[i] = Strings::Base64Chars.find(char_array_4[i]);
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
        char_array_4[j] = Strings::Base64Chars.find(char_array_4[j]);
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

  double Log2(uint64_t n)
  {
    //
    // log_2(n) = log_e(n) / log_e(2)
    //
    return (std::log(n) / std::log(2));
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

  std::vector<GameObject*> GetActorsInRange(GameObject* from, int range)
  {
    std::vector<GameObject*> res;

    if (from == nullptr)
    {
      DebugLog("[WAR] Util::GetActorsInRange() from is null!");
      return res;
    }

    auto curLvl = Map::Instance().CurrentLevel;

    if (curLvl == nullptr)
    {
      DebugLog("[WAR] Util::GetActorsInRange() curLvl is null!");
      return res;
    }

    int lx = from->PosX - range;
    int ly = from->PosY - range;
    int hx = from->PosX + range;
    int hy = from->PosY + range;

    for (auto& a : curLvl->ActorGameObjects)
    {
      if (a->PosX >= lx &&
          a->PosY >= ly &&
          a->PosX <= hx &&
          a->PosY <= hy)
      {
        res.push_back(a.get());
      }
    }

    return res;
  }

  std::vector<GameObject*> GetContainersInRange(GameObject* from, int range)
  {
    std::vector<GameObject*> res;

    if (from == nullptr)
    {
      DebugLog("[WAR] Util::GetActorsInRange() from is null!");
      return res;
    }

    auto curLvl = Map::Instance().CurrentLevel;

    if (curLvl == nullptr)
    {
      DebugLog("[WAR] Util::GetActorsInRange() curLvl is null!");
      return res;
    }

    int lx = from->PosX - range;
    int ly = from->PosY - range;
    int hx = from->PosX + range;
    int hy = from->PosY + range;

    for (int x = lx; x <= hx; x++)
    {
      for (int y = ly; y <= hy; y++)
      {
        if (!IsInsideMap({ x, y }, curLvl->MapSize))
        {
          continue;
        }

        auto so = curLvl->StaticMapObjects[x][y].get();
        if (so != nullptr)
        {
          ContainerComponent* cc = so->GetComponent<ContainerComponent>();
          if (cc != nullptr)
          {
            res.push_back(so);
          }
        }
      }
    }

    return res;
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

  int RollDices(int numRolls, int diceSides)
  {
    int totalDamage = 0;

    for (int i = 0; i < numRolls; i++)
    {
      int dmg = RNG::Instance().RandomRange(1, diceSides + 1);
      totalDamage += dmg;
    }

    return totalDamage;
  }

  int GetExpForNextLevel(int curLvl)
  {
    float p = std::pow(1.3f, curLvl)*20;
    return (int)p;
  }

  void RecalculateWandStats(ItemComponent* wand)
  {
    if (wand == nullptr)
    {
      DebugLog("[WAR] Util::RecalculateWandStats() wand is null!");
      return;
    }

    auto spellType = wand->Data.SpellHeld.SpellType_;

    SpellInfo* sip = SpellsDatabase::Instance().GetSpellInfoFromDatabase(spellType);
    if (sip == nullptr)
    {
      DebugLog("[WAR] Util::RecalculateWandStats() no such spell %i in database!", (int)spellType);
      return;
    }

    SpellInfo si = *sip;

    auto material = wand->Data.WandMaterial;

    int capacity = GlobalConstants::WandCapacityByMaterial.at(material);

    int capacityRandomness = RNG::Instance().RandomRange(0, capacity + 1);

    int qualityModifier = (int)wand->Data.ItemQuality_;

    capacityRandomness += (capacity / 10) * qualityModifier;

    capacity += capacityRandomness;

    wand->Data.WandCapacity.Set(capacity);

    int spellCost = GlobalConstants::WandSpellCapacityCostByType.at(spellType);
    int charges = capacity / spellCost;

    int wandRange = GlobalConstants::WandRangeByMaterial.at(material);
    int wrh = wandRange / 2;
    int rangeRandomness = RNG::Instance().RandomRange(-wrh, wrh + 1);

    wandRange += rangeRandomness;

    switch (wand->Data.Prefix)
    {
      case ItemPrefix::BLESSED:
      {
        capacity *= 2;

        si.SpellBaseDamage.first++;
        si.SpellBaseDamage.second++;
      }
      break;

      case ItemPrefix::UNCURSED:
      {
        wandRange /= 1.5f;
      }
      break;

      case ItemPrefix::CURSED:
      {
        wandRange /= 2;

        capacity /= 2;

        si.SpellBaseDamage.first--;
        si.SpellBaseDamage.second--;
      }
      break;
    }

    if (wandRange <= 0)
    {
      wandRange = 1;
    }

    //
    // Because 0 dice rolls makes no sense
    //
    if (si.SpellBaseDamage.first <= 0)
    {
      si.SpellBaseDamage.first = 1;
    }

    if (si.SpellBaseDamage.second <= 0)
    {
      si.SpellBaseDamage.second = 1;
    }

    wand->Data.SpellHeld = si;
    wand->Data.Range = wandRange;
    wand->Data.WandCapacity.Set(capacity);
    wand->Data.Amount = charges;

    //
    // Actual cost is going to be calculated in GetCost()
    //
    wand->Data.Cost = si.SpellBaseCost;
  }

  void BlessItem(ItemComponent* item)
  {
    if (item == nullptr)
    {
      DebugLog("[WAR] Util::BlessItem() item is null!");
      return;
    }

    for (auto& bonus : item->Data.Bonuses)
    {
      switch (bonus.Type)
      {
        case ItemBonusType::STR:
        case ItemBonusType::DEF:
        case ItemBonusType::MAG:
        case ItemBonusType::RES:
        case ItemBonusType::SKL:
        case ItemBonusType::SPD:
          bonus.BonusValue += 1;
          break;

        case ItemBonusType::HP:
        case ItemBonusType::MP:
          bonus.BonusValue += (bonus.BonusValue / 2);
          break;

        case ItemBonusType::DMG_ABSORB:
        case ItemBonusType::MAG_ABSORB:
          bonus.BonusValue *= 2;
          break;

        case ItemBonusType::DAMAGE:
          bonus.BonusValue += (bonus.BonusValue / 2);
          break;

        case ItemBonusType::VISIBILITY:
          bonus.BonusValue += (bonus.BonusValue / 2);
          break;

        case ItemBonusType::LEECH:
        case ItemBonusType::THORNS:
          bonus.BonusValue += (bonus.BonusValue / 2);
          break;

        case ItemBonusType::SELF_REPAIR:
        case ItemBonusType::REGEN:
          bonus.Period -= (bonus.Period / 2);
          break;

        case ItemBonusType::KNOCKBACK:
          bonus.BonusValue += (bonus.BonusValue / 2);
          break;

        default:
          DebugLog("[WAR] Util::BlessItem() bonus type %i not handled!", (int)bonus.Type);
          break;
      }
    }
  }

  int Rolld100()
  {
    int dice = RNG::Instance().RandomRange(0, 100);

    auto str = StringFormat("\t%s: rolled = %i", __PRETTY_FUNCTION__, dice);
    Logger::Instance().Print(str);

    return dice;
  }

  bool Rolld100(int successChance, bool twoRN)
  {
    #if DEBUG_BUILD
    std::string roll;
    #endif

    bool success = false;

    if (twoRN)
    {
      int rn1 = RNG::Instance().RandomRange(0, 100);
      int rn2 = RNG::Instance().RandomRange(0, 100);

      double avg = (double)(rn1 + rn2) / 2.0;

      success = (avg < successChance);

      #if DEBUG_BUILD
      roll = StringFormat("%.2f", avg);
      #endif
    }
    else
    {
      int result = RNG::Instance().RandomRange(0, 100);

      //
      // In case of clamping by MaxHitChance,
      // 99 is not less than 99, so it's OK.
      //
      success = (result < successChance);

      #if DEBUG_BUILD
      roll = StringFormat("%d", result);
      #endif
    }

    #if DEBUG_BUILD
    std::string succStr   = std::to_string(successChance);
    std::string spaces1(6 - roll.length(), ' ');
    std::string spaces2(6 - succStr.length(), ' ');
    auto logMsg = StringFormat("\t%s: %s %s< %i %s- %s",
                               __PRETTY_FUNCTION__,
                               roll.data(),
                               spaces1.data(),
                               successChance,
                               spaces2.data(),
                               success ? "passed" : "failed");

    Logger::Instance().Print(logMsg);
    #endif

    return success;
  }

  void Sleep(int delayMs)
  {
    //
    // NOTE: Application can freeze on SDL_Delay().
    //
    // At least on Windows if you listen through a long NPC text
    // and at the same time click on other window to lose focus of
    // this one, text (and the whole window) will freeze and then "unfreeze"
    // possibly after specified delay passes in "background".
    // It seems there's no difference in whether you use SDL_Delay() or C++ chrono library.
    //
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

    #ifdef DEBUG_BUILD
    for (auto& p : v)
    {
      DebugLog("[%i;%i] ", p.X, p.Y);
    }
    #endif

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

  uint32_t GetItemInventoryColor(const ItemData& data)
  {
    uint32_t textColor = Colors::WhiteColor;

    bool isBlessed = (data.Prefix == ItemPrefix::BLESSED);
    bool isCursed  = (data.Prefix == ItemPrefix::CURSED);
    bool isMagic   = (data.Rarity == ItemRarity::MAGIC);
    bool isRare    = (data.Rarity == ItemRarity::RARE);
    bool isUnique  = (data.Rarity == ItemRarity::UNIQUE);
    bool isMixed   = (isCursed && (isMagic || isRare));

    // Iterate over this map and select color
    // for the first entry found with bool key == true.
    std::map<int, std::pair<bool, uint32_t>> itemFirstColorToChoose =
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
      textColor = Colors::WhiteColor;
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
            vowelIndex = RNG::Instance().RandomRange(0, Strings::Vowels.length());
            consIndex  = RNG::Instance().RandomRange(0, Strings::Consonants.length());

            vowel = { Strings::Vowels[vowelIndex]    };
            cons  = { Strings::Consonants[consIndex] };

            syl = { cons + vowel };
          }
          break;

          case 1:
          {
            consIndex = RNG::Instance().RandomRange(0, Strings::Consonants.length());
            cons      = Strings::Consonants[consIndex];

            syl = cons;

            char lastVowel = 0;

            for (int i = 0; i < 2; i++)
            {
              vowelIndex = RNG::Instance().RandomRange(0, Strings::Vowels.length());

              char v = Strings::Vowels[vowelIndex];
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

  std::string GetCurrentDateTimeString()
  {
    std::stringstream ss;

    time_t now = time(nullptr);
    tm *ltm = localtime(&now);

    ss << std::put_time(ltm, "%Y-%m-%d_%H-%M-%S");

    return ss.str();
  }

  std::string GetGameObjectDisplayCharacter(GameObject* obj)
  {
    std::string res = "?";

    if (obj == nullptr ||
       (obj->HasEffect(ItemBonusType::INVISIBILITY)
     && !Application::Instance().PlayerInstance.HasEffect(ItemBonusType::TELEPATHY)))
    {
      return res;
    }

    if (IsPlayer(obj))
    {
      res = "@";
    }
    else
    {
      if (obj->Image >= 32 && obj->Image < 128)
      {
        res = { (char)obj->Image };
      }
      else
      {
        if (!obj->ObjectName.empty())
        {
          res = { obj->ObjectName[0] };
        }
      }
    }

    return res;
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

  void LaunchProjectile(const std::vector<Position>& line,
                        char image,
                        const uint32_t& fgColor,
                        const uint32_t& bgColor)
  {
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
    }
  }


  void LaunchProjectile(const Position& from,
                        const Position& to,
                        char image,
                        const uint32_t& fgColor,
                        const uint32_t& bgColor)
  {
    auto line = BresenhamLine(from, to);

    LaunchProjectile(line, image, fgColor, bgColor);
  }

  void KnockBack(GameObject* sender,
                 GameObject* receiver,
                 const Position& attackDir,
                 int tiles)
  {
    auto& mapRef = Map::Instance().CurrentLevel->MapArray;
    auto curLvl = Map::Instance().CurrentLevel;

    Position newPos = receiver->GetPosition();

    int attackDirClampedX = Clamp(attackDir.X, -1, 1);
    int attackDirClampedY = Clamp(attackDir.Y, -1, 1);

    for (int i = 1; i <= tiles; i++)
    {
      //
      // If attack dir is not defined for some reason,
      // don't do any movement.
      //
      if (attackDirClampedX == 0
       && attackDirClampedY == 0)
      {
        DebugLog("Knockback direction is zero!");
        break;
      }

      newPos.X += attackDirClampedX;
      newPos.Y += attackDirClampedY;

      if (mapRef[newPos.X][newPos.Y]->Occupied
       || curLvl->IsCellBlocking(newPos))
      {
        break;
      }

      receiver->MoveTo(newPos, true);

      Application::Instance().ForceDrawCurrentState();

      //
      // Ground units should perish on dangerous tiles.
      //
      bool isFlying = receiver->HasEffect(ItemBonusType::LEVITATION);
      bool danger   = Map::Instance().IsTileDangerous(receiver->GetPosition());
      if (!isFlying && danger)
      {
        Application::Instance().DisplayAttack(receiver, GlobalConstants::DisplayAttackDelayMs, std::string());
        receiver->Attrs.HP.Reset(0);
        break;
      }
    }

    //
    // Knocked back units are stunned
    //

    ItemBonusStruct bs;

    bs.Type       = ItemBonusType::PARALYZE;
    bs.BonusValue = 1;
    bs.Duration   = tiles * 2;
    bs.Id         = sender->ObjectId();

    receiver->AddEffect(bs);
    receiver->CheckPerish();
  }

  void ReapplyBonuses(GameObject* onWho, ItemComponent* item)
  {
    onWho->UnapplyBonuses(item);
    onWho->ApplyBonuses(item);
  }

  std::string ProcessTeleport(GameObject* target)
  {
    std::string msg;

    Position p = target->GetPosition();

    GameObject* actor = nullptr;
    if (!Util::IsPlayer(target))
    {
      actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
    }

    if (actor != nullptr)
    {
      auto pos = Map::Instance().GetRandomEmptyCell();
      MapType mt = Map::Instance().CurrentLevel->MapType_;
      Map::Instance().TeleportToExistingLevel(mt, pos, actor);

      msg = Util::StringFormat("%s suddenly disappears!", actor->ObjectName.data());
    }

    return msg;
  }

  std::pair<char, uint32_t> GetProjectileImageAndColor(ItemComponent* weapon,
                                                       bool throwingFromInventory)
  {
    std::pair<char, uint32_t> res;

    char projectile = ' ';

    uint32_t projColor = Colors::WhiteColor;

    if (weapon == nullptr)
    {
      DebugLog("[WAR] Util::GetProjectileImageAndColor() weapon is null!");
      res = { projectile, projColor };
      return res;
    }

    bool isWand   = (weapon->Data.ItemType_ == ItemType::WAND);
    bool isWeapon = (weapon->Data.ItemType_ == ItemType::RANGED_WEAPON);

    if (throwingFromInventory)
    {
      projectile = weapon->OwnerGameObject->Image;
    }
    else
    {
      if (isWand)
      {
        projectile = '*';

        SpellType spell = weapon->Data.SpellHeld.SpellType_;
        SpellInfo* si = SpellsDatabase::Instance().GetSpellInfoFromDatabase(spell);
        if (si->SpellProjectileColor != Colors::None)
        {
          projColor = si->SpellProjectileColor;
        }
      }
      else if (isWeapon)
      {
        projectile = '+';
      }
    }

    res = { projectile, projColor };

    return res;
  }

  std::vector<GameObject*> GetObjectsOnTheLine(const std::vector<Position>& line)
  {
    std::vector<GameObject*> res;

    auto player = &Application::Instance().PlayerInstance;

    Position mapSize = Map::Instance().CurrentLevel->MapSize;

    //
    // Do not include object on starting point
    //
    for (size_t i = 1; i < line.size(); i++)
    {
      auto& p = line[i];

      if (IsInsideMap(p, mapSize))
      {
        //
        // Right now the functionality of this method is based
        // on assumption, that certain items, that use this functionality
        // (e.g. wand of piercing), always strike through the whole
        // targeting line, ignoring any items lying on the ground,
        // until their ray is blocked or their piercing power dissipates.
        //
        if (player->PosX == p.X && player->PosY == p.Y)
        {
          res.push_back(player);
        }

        auto actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
        if (actor != nullptr)
        {
          res.push_back(actor);
        }
        else
        {
          auto so = Map::Instance().GetStaticGameObjectAtPosition(p.X, p.Y);
          if (so != nullptr)
          {
            res.push_back(so);
          }
        }
      }
    }

    return res;
  }

  GameObject* GetFirstObjectOnTheLine(const std::vector<Position>& line)
  {
    GameObject* res = nullptr;

    auto player = &Application::Instance().PlayerInstance;

    Position mapSize = Map::Instance().CurrentLevel->MapSize;

    for (size_t i = 1; i < line.size(); i++)
    {
      auto& p = line[i];

      if (IsInsideMap(p, mapSize))
      {
        if (player->PosX == p.X
         && player->PosY == p.Y)
        {
          res = player;
          break;
        }

        auto actor = Map::Instance().GetActorAtPosition(p.X, p.Y);
        if (actor != nullptr)
        {
          res = actor;
          break;
        }
        else
        {
          auto so = Map::Instance().GetStaticGameObjectAtPosition(p.X, p.Y);
          if (so != nullptr)
          {
            res = so;
            break;
          }
        }
      }
    }

    return res;
  }

  // ===========================================================================

  std::pair<bool, std::string> TryToDamageObject(GameObject* object,
                                                 GameObject* from,
                                                 int amount,
                                                 bool againstRes)
  {
    std::pair<bool, std::string> res;

    res.first = false;

    if (object != nullptr)
    {
      int dmgHere = amount;

      if (againstRes)
      {
        dmgHere -= object->Attrs.Res.Get();
      }

      if (dmgHere <= 0)
      {
        res.second = Util::StringFormat("%s seems unaffected!", object->ObjectName.data());;
      }
      else
      {
        object->ReceiveDamage(from, dmgHere, againstRes, false, false, !IsPlayer(from));
        res.first = true;
      }
    }

    return res;
  }

  std::string TryToDamageEquipment(GameObject* actor,
                                   EquipmentCategory cat,
                                   int damage)
  {
    std::string msg;

    if (cat == EquipmentCategory::NOT_EQUIPPABLE)
    {
      DebugLog("[WAR] Util::TryToDamageEquipment() category is NOT_EQUIPPABLE!");
      return msg;
    }

    if (actor == nullptr)
    {
      DebugLog("[WAR] Util::TryToDamageEquipment() actor is null!");
      return msg;
    }

    EquipmentComponent* ec = actor->GetComponent<EquipmentComponent>();
    if (ec != nullptr)
    {
      ItemComponent* ic = ec->EquipmentByCategory[cat][0];
      return TryToDamageEquipment(actor, ic, damage);
    }
    else
    {
      DebugLog("[WAR] Util::TryToDamageEquipment() equipment is null on %s!", actor->ObjectName.data());
    }

    return msg;
  }

  std::string TryToDamageEquipment(GameObject* actor,
                                   ItemComponent* item,
                                   int damage)
  {
    std::string msg;

    if (actor == nullptr)
    {
      DebugLog("[WAR] Util::TryToDamageEquipment() actor is null!");
      return msg;
    }

    //
    // NOTE: item can be legitimately null
    // if actor is a monster like a rat, for example,
    // who doesn't have equipped items.
    //

    if (item == nullptr)
    {
      //DebugLog("[WAR] Util::TryToDamageEquipment() item is null on %s!", actor->ObjectName.data());
      return msg;
    }

    if (item->Data.HasBonus(ItemBonusType::INDESTRUCTIBLE))
    {
      return msg;
    }

    item->Data.Durability.AddMin(damage);

    if (item->Data.Durability.Min().Get() <= 0)
    {
      msg = StringFormat("%s breaks!", item->OwnerGameObject->ObjectName.data());
      item->Break(actor);
    }

    return msg;
  }

  int CalculateDamageValue(GameObject* attacker,
                           GameObject* defender,
                           ItemComponent* weapon,
                           bool meleeAttackWithRangedWeapon)
  {
    int totalDmg = 0;

    auto GetUnarmedDamage = [&]()
    {
      int dmg = RollDices(1, 2);
      dmg += attacker->Attrs.Str.Get() - defender->Attrs.Def.Get();

      if (dmg <= 0)
      {
        dmg = 1;
      }

      return dmg;
    };

    // Unarmed strike
    if (weapon == nullptr)
    {
      totalDmg = GetUnarmedDamage();
    }
    else
    {
      //
      // Melee attack with ranged weapon in hand
      // fallbacks to unarmed attack
      //
      if (meleeAttackWithRangedWeapon)
      {
        totalDmg = GetUnarmedDamage();
        return totalDmg;
      }

      int weaponDamage = RollDices(weapon->Data.Damage.Min().Get(),
                                   weapon->Data.Damage.Max().Get());

      totalDmg = weaponDamage;

      int targetDef = weapon->Data.HasBonus(ItemBonusType::IGNORE_DEFENCE)
                    ? 0
                    : defender->Attrs.Def.Get();

      ItemBonusStruct* res = weapon->Data.GetBonus(ItemBonusType::DAMAGE);
      if (res != nullptr)
      {
        totalDmg += res->BonusValue;
      }

      totalDmg += attacker->Attrs.Str.Get() - targetDef;

      if (totalDmg <= 0)
      {
        totalDmg = 1;
      }
    }

    return totalDmg;
  }

  int CalculateHitChanceMelee(GameObject* attacker,
                              GameObject* defender)
  {
    // Amount of addition to hit chance
    // times SKL difference of attacker and defender
    int attackChanceScale = 3;

    int defaultHitChance = 50;
    int hitChance = defaultHitChance;

    int skillDiff = attacker->Attrs.Skl.Get() - defender->Attrs.Skl.Get();
    int difficulty = (skillDiff * attackChanceScale);

    hitChance += difficulty;

    hitChance = Clamp(hitChance,
                      GlobalConstants::MinHitChance,
                      GlobalConstants::MaxHitChance);

    if (attacker->HasEffect(ItemBonusType::BLINDNESS))
    {
      hitChance /= 2;
    }

    return hitChance;
  }

  int CalculateHitChanceRanged(const Position& start,
                               const Position& end,
                               GameObject* user,
                               ItemComponent* weapon,
                               bool isThrowing)
  {
    int baseChance = 50;
    int chance = 100;
    int attackChanceScale = 5;
    int distanceChanceDrop = 2;
    int skl = user->Attrs.Skl.Get();
    int d = (int)LinearDistance(start, end);

    auto GetSkillBasedChance = [&](int baseChance)
    {
      int ch = baseChance;

      ch += (attackChanceScale * skl);
      ch -= (distanceChanceDrop * d);

      return ch;
    };

    if (weapon == nullptr)
    {
      chance = GetSkillBasedChance(baseChance);
    }
    else
    {
      if (weapon->Data.ItemType_ == ItemType::WAND || isThrowing)
      {
        chance = 100;
      }
      else
      {
        bool isXBow = (weapon->Data.RangedWeaponType_ == RangedWeaponType::LIGHT_XBOW
                    || weapon->Data.RangedWeaponType_ == RangedWeaponType::XBOW
                    || weapon->Data.RangedWeaponType_ == RangedWeaponType::HEAVY_XBOW);

        baseChance = isXBow ? (baseChance + 15) : baseChance;

        chance = GetSkillBasedChance(baseChance);

        EquipmentComponent* ec = user->GetComponent<EquipmentComponent>();
        if (ec != nullptr)
        {
          //
          // Assuming valid projectiles are already equipped
          //
          ItemComponent* arrows = ec->EquipmentByCategory[EquipmentCategory::SHIELD][0];
          if (arrows != nullptr)
          {
            if (arrows->Data.ItemType_ != ItemType::ARROWS)
            {
              DebugLog("[WAR] off-hand slot is not arrows!");
            }

            ItemPrefix ammoPrefix = ec->EquipmentByCategory[EquipmentCategory::SHIELD][0]->Data.Prefix;
            switch (ammoPrefix)
            {
              case ItemPrefix::BLESSED:
                chance *= 2;
                break;

              case ItemPrefix::CURSED:
                chance /= 2;
                break;
            }
          }
        }

        chance = Clamp(chance,
                       GlobalConstants::MinHitChance,
                       GlobalConstants::MaxHitChance);
      }
    }

    /*
    #ifdef DEBUG_BUILD
    auto str = StringFormat("Calculated hit chance: %i (SKL: %i, SKL bonus: %i, distance: -%i)",
                            chance,
                            skl,
                            (attackChanceScale * skl),
                            (distanceChanceDrop * d));

    Logger::Instance().Print(str);

    DebugLog("%s\n%i + %i - %i = %i\n", __PRETTY_FUNCTION__,
                                        baseChance,
                                        (attackChanceScale * skl),
                                        (distanceChanceDrop * d),
                                        chance);

    str = StringFormat("Total chance: %i", chance);
    Logger::Instance().Print(str);

    DebugLog("%s", str.data());
    #endif
    */

    return chance;
  }

  int GetTotalDamageAbsorptionValue(GameObject* who, bool magic)
  {
    int res = 0;

    ItemBonusType t = magic ? ItemBonusType::MAG_ABSORB : ItemBonusType::DMG_ABSORB;

    if (who != nullptr)
    {
      //
      // Check for inherent absorption
      //
      for (auto& kvp : who->GetActiveEffects())
      {
        for (const ItemBonusStruct& e : kvp.second)
        {
          if (e.Type == t)
          {
            res += e.BonusValue;
          }
        }
      }

      EquipmentComponent* ec = who->GetComponent<EquipmentComponent>();
      if (ec != nullptr)
      {
        for (auto& kvp : ec->EquipmentByCategory)
        {
          for (ItemComponent* item : kvp.second)
          {
            if (item != nullptr && item->Data.HasBonus(t))
            {
              ItemBonusStruct* ibs = item->Data.GetBonus(t);
              res += ibs->BonusValue;
            }
          }
        }
      }
    }

    return res;
  }

  std::vector<std::string> ProcessPhysicalDamage(GameObject* who,
                                                 GameObject* from,
                                                 int& amount,
                                                 bool ignoreArmor)
  {
    std::vector<std::string> res;

    if (who == nullptr)
    {
      DebugLog("[WAR] Util::ProcessPhysicalDamage() who is null!");
      return res;
    }

    int abs = GetTotalDamageAbsorptionValue(who, false);

    amount -= abs;

    if (amount < 0)
    {
      amount = 0;
    }

    std::vector<std::string> armorMsgs;

    if (!ignoreArmor)
    {
      armorMsgs = DamageArmor(who, from, amount);
    }

    std::string whoImg  = GetGameObjectDisplayCharacter(who);
    std::string fromImg = GetGameObjectDisplayCharacter(from);

    //
    // If no armor present
    //
    if (armorMsgs.empty())
    {
      if (who->HasEffect(ItemBonusType::MANA_SHIELD))
      {
        ProcessManaShield(who, amount);
      }
      else
      {
        who->Attrs.HP.AddMin(-amount);
      }

      res.push_back(StringFormat("%s => %s (%i)",
                                 fromImg.data(),
                                 whoImg.data(),
                                 amount));
    }
    else
    {
      for (auto& m : armorMsgs)
      {
        res.push_back(m);
      }
    }

    return res;
  }

  std::string ProcessMagicalDamage(GameObject* who,
                                   GameObject* from,
                                   int& amount)
  {
    if (who == nullptr)
    {
      DebugLog("[WAR] Util::ProcessMagicalDamage() who is null!");
      return std::string();
    }

    std::string logMsg;

    std::string whoImg  = GetGameObjectDisplayCharacter(who);
    std::string fromImg = GetGameObjectDisplayCharacter(from);

    if (IsPlayer(who) && IsPlayer(from))
    {
      logMsg = StringFormat("You hit yourself for %i damage!", amount);
    }
    else
    {
      logMsg = StringFormat("%s => %s (%i)",
                            fromImg.data(),
                            whoImg.data(),
                            amount);
    }

    int abs = GetTotalDamageAbsorptionValue(who, true);

    amount -= abs;

    if (amount < 0)
    {
      amount = 0;
    }

    if (who->HasEffect(ItemBonusType::MANA_SHIELD))
    {
      ProcessManaShield(who, amount);
    }
    else
    {
      who->Attrs.HP.AddMin(-amount);
    }

    return logMsg;
  }

  void ProcessManaShield(GameObject* who,
                         int amount)
  {
    if (who == nullptr)
    {
      DebugLog("[WAR] Util::ProcessManaShield() who is null!");
      return;
    }

    if (amount == 0)
    {
      return;
    }

    int overkill = who->Attrs.MP.Min().Get() - amount;

    who->Attrs.MP.AddMin(-amount);

    if (overkill <= 0)
    {
      who->DispelEffectFirstFound(ItemBonusType::MANA_SHIELD);
      who->Attrs.HP.AddMin(overkill);
    }
  }

  int ProcessThorns(GameObject* who,
                    int damageReceived)
  {
    int dmgReturned = 0;

    auto thorns = GetItemsWithBonus(who, ItemBonusType::THORNS);
    for (auto& i : thorns)
    {
      auto b = i->Data.GetBonus(ItemBonusType::THORNS);
      float fract = (float)b->BonusValue * 0.01f;
      int dmg = (int)((float)damageReceived * fract);
      dmgReturned += dmg;
    }

    return dmgReturned;
  }

  std::vector<std::string> DamageArmor(GameObject* who,
                                       GameObject* from,
                                       int amount)
  {
    std::vector<std::string> logMsgs;
    std::vector<std::string> logMsgsRec;

    if (who == nullptr)
    {
      std::string err = "[WAR] Util::DamageArmor() who is null!";
      DebugLog("%s", err.data());
      logMsgs.push_back(err);
      return logMsgs;
    }

    std::string whoStr  = GetGameObjectDisplayCharacter(who);
    std::string fromStr = GetGameObjectDisplayCharacter(from);

    EquipmentComponent* ec = who->GetComponent<EquipmentComponent>();
    if (ec != nullptr)
    {
      ItemComponent* armor = ec->EquipmentByCategory[EquipmentCategory::TORSO][0];
      if (armor != nullptr)
      {
        logMsgs.push_back(StringFormat("%s => [ (%i)", fromStr.data(), amount));

        if (armor->Data.HasBonus(ItemBonusType::INDESTRUCTIBLE)
         || amount == 0)
        {
          return logMsgs;
        }

        int durabilityLeft = armor->Data.Durability.Min().Get();
        int overkill = durabilityLeft - amount;

        armor->Data.Durability.AddMin(-amount);

        if (armor->Data.Durability.Min().Get() <= 0)
        {
          if (IsPlayer(who))
          {
            logMsgs.push_back(StringFormat("%s breaks!", armor->OwnerGameObject->ObjectName.data()));
          }

          armor->Break(who);
        }

        if (overkill < 0)
        {
          int hpDamage = std::abs(overkill);

          // Recursion in production code, yay!
          logMsgsRec = ProcessPhysicalDamage(who, from, hpDamage, true);
        }
      }
    }

    for (auto& msg : logMsgsRec)
    {
      logMsgs.push_back(msg);
    }

    return logMsgs;
  }

  Position GetRandomPointAround(GameObject* user,
                                ItemComponent* weapon,
                                const Position& aroundThis)
  {
    Position pos = { -1, -1 };

    auto rect = GetEightPointsAround(aroundThis,
                                     Map::Instance().CurrentLevel->MapSize);

    bool outOfRange = false;

    // If we shoot from point blank in a corridor,
    // we shouldn't accidentaly target ourselves
    // due to lack of skill.
    for (size_t i = 0; i < rect.size(); i++)
    {
      // Do not include points above weapon's maximum range as well.
      int d = LinearDistance(user->GetPosition(), rect[i]);
      if (weapon != nullptr && d > weapon->Data.Range)
      {
        outOfRange = true;
      }

      // If random point from lack of skill is the one
      // we're standing on, ignore it.
      bool targetSelf = (rect[i].X == user->PosX
                      && rect[i].Y == user->PosY);

      if (targetSelf || outOfRange)
      {
        rect.erase(rect.begin() + i);
        break;
      }
    }

    // Just in case
    if (!rect.empty())
    {
      int index = RNG::Instance().RandomRange(0, rect.size());
      pos = rect[index];
    }

    return pos;
  }

  std::vector<Position> ProcessLaserAttack(GameObject* user,
                                           ItemComponent* weapon,
                                           const Position& end)
  {
    std::vector<Position> line;

    if (user == nullptr)
    {
      DebugLog("[WAR] Util::ProcessLaserAttack() user is nullptr!");
      return line;
    }

    if (weapon == nullptr)
    {
      DebugLog("[WAR] Util::ProcessLaserAttack() weapon is nullptr!");
      return line;
    }

    if (weapon->Data.Amount <= 0)
    {
      return line;
    }

    int minDmg = weapon->Data.SpellHeld.SpellBaseDamage.first;
    int maxDmg = weapon->Data.SpellHeld.SpellBaseDamage.second;

    line = ProcessLaserAttack(user, { minDmg, maxDmg }, end);

    weapon->Data.Amount--;

    return line;
  }

  std::vector<Position> ProcessLaserAttack(GameObject* user,
                                           const std::pair<int, int>& damageRange,
                                           const Position& end)
  {
    std::vector<Position> lineRes;

    if (user == nullptr)
    {
      DebugLog("[WAR] Util::ProcessLaserAttack() user is nullptr!");
      return lineRes;
    }

    Position startPoint = user->GetPosition();
    Position endPoint   = end;

    auto line = BresenhamLine(startPoint, endPoint);
    auto objects = GetObjectsOnTheLine(line);

    int distanceCovered = 0;

    int numRolls  = damageRange.first;
    int diceSides = damageRange.second;

    int power = RollDices(numRolls, diceSides);

    int userMagic = user->Attrs.Mag.Get();

    power += userMagic;

    bool shouldStop = false;

    // Start from 1 to exclude user's position
    for (size_t i = 1; i < line.size(); i++)
    {
      if (power <= 0)
      {
        break;
      }

      int mx = line[i].X;
      int my = line[i].Y;

      for (auto& obj : objects)
      {
        if (obj->PosX == mx && obj->PosY == my)
        {
          // Laser stops when it hits indestructible object
          // or its power has dissipated.
          if (obj->Attrs.Indestructible || power <= 0)
          {
            shouldStop = true;
            break;
          }

          int def = obj->Attrs.Def.Get();
          int dmgDone = power - def;

          obj->ReceiveDamage(user,
                             dmgDone,
                             false,
                             false,
                             false,
                             false);

          power -= (def <= 0) ? 1 : def;
        }
      }

      int drawingPosX = mx + Map::Instance().CurrentLevel->MapOffsetX;
      int drawingPosY = my + Map::Instance().CurrentLevel->MapOffsetY;

      lineRes.push_back({ drawingPosX, drawingPosY });

      if (shouldStop)
      {
        break;
      }

      distanceCovered++;

      power -= distanceCovered;
    }

    return lineRes;
  }

  void DrawLaserAttack(const std::vector<Position>& line)
  {
    for (auto& p : line)
    {
      Printer::Instance().PrintFB(p.X,
                                  p.Y,
                                  '*',
                                  Colors::YellowColor,
                                  Colors::RedColor);
    }

    Printer::Instance().Render();
    Sleep(100);
    Application::Instance().ForceDrawMainState();
  }

  // ===========================================================================

  std::vector<Position> GetAreaDamagePointsFrom(const Position& from, int range)
  {
    std::vector<Position> res;

    int lx = from.X - range;
    int ly = from.Y - range;
    int hx = from.X + range;
    int hy = from.Y + range;

    std::vector<Position> perimeterPoints;

    for (int x = lx; x <= hx; x++)
    {
      Position p1 = { x, ly };
      Position p2 = { x, hy };

      perimeterPoints.push_back(p1);
      perimeterPoints.push_back(p2);
    }

    for (int y = ly + 1; y <= hy - 1; y++)
    {
      Position p1 = { lx, y };
      Position p2 = { hx, y };

      perimeterPoints.push_back(p1);
      perimeterPoints.push_back(p2);
    }

    for (auto& p : perimeterPoints)
    {
      // Different lines can go through the same points
      // so a check if a point was already added is needed.
      auto line = BresenhamLine(from, p);
      for (auto& point : line)
      {
        if (!IsInsideMap(point, Map::Instance().CurrentLevel->MapSize))
        {
          continue;
        }

        auto it = std::find_if(res.begin(), res.end(),
                  [&point](const Position& p) ->
                  bool { return (p == point); });

        // If point was already added, skip it.
        if (it != res.end())
        {
          continue;
        }

        int d = LinearDistance(from, point);

        auto cell = Map::Instance().CurrentLevel->MapArray[point.X][point.Y].get();
        auto obj  = Map::Instance().CurrentLevel->StaticMapObjects[point.X][point.Y].get();

        bool cellOk = (!cell->Blocking);
        bool objOk  = (obj == nullptr);

        if (cellOk && objOk && d <= range)
        {
          res.push_back(cell->GetPosition());
        }
        else
        {
          if (obj != nullptr && !obj->Attrs.Indestructible)
          {
            res.push_back(cell->GetPosition());
          }

          break;
        }
      }
    }

    return res;
  }

  std::vector<ItemComponent*> GetItemsWithBonus(GameObject* actor,
                                                ItemBonusType bonus)
  {
    std::vector<ItemComponent*> res;

    if (actor == nullptr)
    {
      DebugLog("[WAR] Util::GetItemsWithBonus() actor is null!");
      return res;
    }

    EquipmentComponent* ec = actor->GetComponent<EquipmentComponent>();
    if (ec != nullptr)
    {
      for (auto& item : ec->EquipmentByCategory)
      {
        for (auto& e : item.second)
        {
          if (e != nullptr && e->Data.HasBonus(bonus))
          {
            res.push_back(e);
          }
        }
      }
    }

    return res;
  }

  Position InvertDirection(const Position& dir)
  {
    return { -dir.X, -dir.Y };
  }

  bool IsPlayer(GameObject* obj)
  {
    return (obj == &Application::Instance().PlayerInstance);
  }

  bool CanBeSpawned(ItemComponent* ic)
  {
    int lvl = (int)Map::Instance().CurrentLevel->MapType_;
    if (ic != nullptr)
    {
      return (lvl >= (int)ic->Data.GeneratedAfter);
    }

    return false;
  }

  size_t CalculateItemHash(ItemComponent* item)
  {
    if (item == nullptr)
    {
      DebugLog("[WAR] CalculateItemHash() - item is null!");
      return 0;
    }

    std::hash<std::string> hasher;
    std::string str = std::to_string((int)item->Data.Prefix) + item->OwnerGameObject->ObjectName;
    return hasher(str);
  }
}
