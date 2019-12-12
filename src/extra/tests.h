#ifndef TESTS_H
#define TESTS_H

#include "util.h"
#include "player.h"
#include "level-builder.h"
#include "pathfinder.h"

#include <sstream>

namespace Tests
{  
  /// Test Bresenham line return vector in all directions
  inline void TestLoS(std::stringstream& ss, int x, int y, int range)
  {
    //printf("Starting Bresenham LoS test from (%i %i) in range %i...\n", x, y, range);
    //printf("The path should always start from (%i %i)\n\n", x, y);
    ss << Util::StringFormat("Starting Bresenham LoS test from (%i %i) in range %i...\n", x, y, range);
    ss << Util::StringFormat("The path should always start from (%i %i)\n\n", x, y);

    int sx = x - range;
    int sy = y - range;
    int ex = x + range;
    int ey = y + range;

    for (int fx = sx; fx <= ex; fx++)
    {
      for (int fy = sy; fy <= ey; fy++)
      {
        auto res = Util::BresenhamLine(x, y, fx, fy);
    
        //printf ("[%i;%i] -> [%i;%i]\n", x, y, fx, fy);
        ss << Util::StringFormat("[%i;%i] -> [%i;%i]\n", x, y, fx, fy);
        
        for (auto& i : res)
        {
          //printf("(%i %i) - ", i.X, i.Y);
          ss << Util::StringFormat("(%i %i) - ", i.X, i.Y);
        }
        
        bool cond = (res[0].X == x && res[0].Y == y);
        if (!cond)
        {
          //printf("*** FAILED! ***\n");
          ss << "*** FAILED! ***\n";
        }
        
        //printf("\n");
        ss << "\n";
      }
    }    
  }

  inline void RoomTests(std::stringstream& ss)
  {
    //printf("\nRoom layouts rotations:\n\n");
    ss << "\nRoom layouts rotations:\n\n";

    for (auto& room : GlobalConstants::DungeonRooms)
    {
      //printf("Layout:\n");
      ss << "Layout:\n";

      for (auto& row : room)
      {
        //printf("%s\n", row.data());
        ss << Util::StringFormat("%s\n", row.data());
      }

      //printf("CCW_90:\n");
      ss << "CCW_90:\n";

      auto res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_90);
      for (auto& s : res)
      {
        //printf("\t%s\n", s.data());
        ss << Util::StringFormat("\t%s\n", s.data());
      }

      //printf("CCW_180:\n");
      ss << "CCW_180:\n";

      res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_180);
      for (auto& s : res)
      {
        //printf("\t%s\n", s.data());
        ss << Util::StringFormat("\t%s\n", s.data());
      }

      //printf("CCW_270:\n");
      ss << "CCW_270:\n";

      res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_270);
      for (auto& s : res)
      {
        //printf("\t%s\n", s.data());
        ss << Util::StringFormat("\t%s\n", s.data());
      }
    }
  }

  inline void RNGTests(std::stringstream& ss)
  {
    //printf("\nRNG tests:\n\n");
    ss << "\nRNG tests:\n\n";

    //printf("Random.Range(-10, 10):\n");
    ss << "Random.Range(-10, 10):\n";

    int numberOfRolls = 100;

    for (int i = 0; i < numberOfRolls; i++)
    {
      //printf("%i ", RNG::Instance().RandomRange(-10, 10));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(-10, 10));
    }

    //printf("\n\nRandom.Range(0, 100):\n");
    ss << "\n\nRandom.Range(0, 100):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      //printf("%i ", RNG::Instance().RandomRange(0, 100));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(0, 100));
    }

    //printf("\n\nRandom.Range(-20, -10):\n");
    ss << "\n\nRandom.Range(-20, -10):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      //printf("%i ", RNG::Instance().RandomRange(-20, -10));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(-20, -10));
    }

    //printf("\n\nRandom.Range(100, 10):\n");
    ss << "\n\nRandom.Range(100, 10):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      //printf("%i ", RNG::Instance().RandomRange(100, 10));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(100, 10));
    }

    //printf("\n\nRandom.Range(-10, -100):\n");
    ss << "\n\nRandom.Range(-10, -100):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      //printf("%i ", RNG::Instance().RandomRange(-10, -100));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(-10, -100));
    }

    //printf("\n\nRandom.Range(10, -10):\n");
    ss << "\n\nRandom.Range(10, -10):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      //printf("%i ", RNG::Instance().RandomRange(10, -10));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(10, -10));
    }
  }

  inline void AutoLevel(std::stringstream& ss, Player& p, int level)
  {
    //printf("Auto level to %i:\n\n", level);
    ss << Util::StringFormat("Auto level to %i:\n\n", level);

    std::map<int, std::pair<std::string, Attribute&>> attrsMap =
    {
      { 0, { "STR", p.Attrs.Str } },
      { 1, { "DEF", p.Attrs.Def } },
      { 2, { "MAG", p.Attrs.Mag } },
      { 3, { "RES", p.Attrs.Res } },
      { 4, { "SKL", p.Attrs.Skl } },
      { 5, { "SPD", p.Attrs.Spd } }
    };

    int maxLevel = level;

    for (int i = 0; i < maxLevel; i++)
    {
      p.LevelUp();
    }

    for (auto& a : attrsMap)
    {
      auto kvp = a.second;

      //printf("%s: %i (* %i)\n", kvp.first.data(), kvp.second.OriginalValue(), kvp.second.Talents);
      ss << Util::StringFormat("%s: %i (* %i)\n", kvp.first.data(), kvp.second.OriginalValue(), kvp.second.Talents);
    }

    //printf("\n");
    //printf("HP: %i (* %i)\n", p.Attrs.HP.Max().Get(), p.Attrs.HP.Talents);
    //printf("MP: %i (* %i)\n", p.Attrs.MP.Max().Get(), p.Attrs.MP.Talents);
    ss << "\n";
    ss << Util::StringFormat("HP: %i (* %i)\n", p.Attrs.HP.Max().Get(), p.Attrs.HP.Talents);
    ss << Util::StringFormat("MP: %i (* %i)\n", p.Attrs.MP.Max().Get(), p.Attrs.MP.Talents);
  }

  inline void LevelUpTests(std::stringstream& ss)
  {
    //printf("\nAuto levelling tests:\n\n");
    ss << "\nAuto levelling tests:\n\n";

    Player p1;

    p1.Attrs.Str.Talents = 3;
    p1.Attrs.Def.Talents = 2;
    p1.Attrs.Skl.Talents = 1;

    AutoLevel(ss, p1, 20);
  }

  inline void WeightedRandomTest(std::stringstream& ss)
  {
    //printf("\nWeighted random test:\n\n");
    ss << "\nWeighted random test:\n\n";

    std::map<char, int> testMap =
    {
      { 'a', 8 },
      { 'b', 1 },
      { 'c', 2 }
    };

    for (auto& kvp : testMap)
    {
      //printf("key: %c, weight: %i\n", kvp.first, kvp.second);
      ss << Util::StringFormat("key: %c, weight: %i\n", kvp.first, kvp.second);
    }

    std::map<char, int> nums;

    nums['a'] = 0;
    nums['b'] = 0;
    nums['c'] = 0;

    int iterations = 10000;

    //printf("\n%i iterations:\n", iterations);
    ss << Util::StringFormat("\n%i iterations:\n", iterations);

    for (int i = 0; i < iterations; i++)
    {
      auto res = Util::WeightedRandom(testMap);
      nums[res.first] += 1;
    }

    //printf("\n");
    ss << "\n";

    for (auto& kvp : nums)
    {
      //printf("total scores: %c = %i\n", kvp.first, kvp.second);
      ss << Util::StringFormat("total scores: %c = %i\n", kvp.first, kvp.second);
    }
  }

  inline void LootDropTest(std::stringstream& ss)
  {
    //printf("\nLoot drop test:\n\n");
    ss << "\nLoot drop test:\n\n";

    std::map<ItemType, int> scores;

    for (int i = 0; i < 100; i++)
    {
      auto weights = GlobalConstants::LootTable.at(GameObjectType::RAT);
      auto kvp = Util::WeightedRandom(weights);

      scores[kvp.first]++;
    }

    for (auto& kvp : scores)
    {
      //printf("%i = %i\n", kvp.first, kvp.second);
      ss << Util::StringFormat("%i = %i\n", kvp.first, kvp.second);
    }
  }

  inline void LevelBuilderTest(std::stringstream& ss)
  {
    std::string str;

    str = "\nLevel builders:\n\n";

    //Logger::Instance().Print(str);
    //printf("%s", str.data());
    ss << str;

    LevelBuilder lb;

    Position s(1, 1);
    //Position mapSize(24, 78);
    Position mapSize(200, 200);

    str = "\n1) Tunneler:\n\n";
    //printf("%s", str.data());
    ss << str;
    //Logger::Instance().Print(str);

    int iterations = (mapSize.X * mapSize.Y) / 2;
    lb.TunnelerMethod(mapSize, iterations, { 5, 10 }, s);
    lb.MapRaw[s.X][s.Y] = 'X';
    //lb.LogPrintMapRaw();
    //lb.PrintMapRaw();
    ss << lb.GetMapRawString();

    str = "\n2) Tunneler (backtracking):\n\n";
    //printf("%s", str.data());
    ss << str;
    //Logger::Instance().Print(str);

    lb.BacktrackingTunnelerMethod(mapSize, { 5, 10 }, s, true);
    lb.MapRaw[s.X][s.Y] = 'X';
    //lb.LogPrintMapRaw();
    //lb.PrintMapRaw();
    ss << lb.GetMapRawString();

    str = "\n3) Recursive Backtracker:\n\n";
    //printf("%s", str.data());
    ss << str;
    //Logger::Instance().Print(str);

    lb.RecursiveBacktrackerMethod(mapSize, s);
    lb.MapRaw[s.X][s.Y] = 'X';
    //lb.LogPrintMapRaw();
    //lb.PrintMapRaw();
    ss << lb.GetMapRawString();

    str = "\n4) Cellular Automata:\n\n";
    //Logger::Instance().Print(str);
    //printf("%s", str.data());
    ss << str;

    lb.CellularAutomataMethod(mapSize, 40, 5, 4, 12);
    //lb.LogPrintMapRaw();
    //lb.PrintMapRaw();
    ss << lb.GetMapRawString();

    str = "\n5) Digger:\n\n";
    //Logger::Instance().Print(str);
    //printf("%s", str.data());
    ss << str;

    iterations = (mapSize.X * mapSize.Y) / 10;

    FeatureRoomsWeights weights =
    {
      { FeatureRoomType::EMPTY,    { 10, 0 }  }
    };

    lb.FeatureRoomsMethod(mapSize, { 1, 10 }, weights, 3, iterations);
    //lb.LogPrintMapRaw();
    //lb.PrintMapRaw();
    ss << lb.GetMapRawString();

    str = "\n6) BSP rooms:\n\n";
    //Logger::Instance().Print(str);
    //printf("%s", str.data());
    ss << str;

    lb.RoomsMethod(mapSize, { 45, 55 }, 5);
    //lb.LogPrintMapRaw();
    //lb.PrintMapRaw();
    ss << lb.GetMapRawString();

    str = "\n7) Feature rooms:\n\n";
    //Logger::Instance().Print(str);
    //printf("%s", str.data());
    ss << str;

    weights =
    {
      { FeatureRoomType::EMPTY,    { 10, 0 }  },
      { FeatureRoomType::DIAMOND,  { 3, 3 }   },
      { FeatureRoomType::FLOODED,  { 1, 3 }   },
      { FeatureRoomType::GARDEN,   { 3, 3 }   },
      { FeatureRoomType::PILLARS,  { 5, 0 }   },
      { FeatureRoomType::ROUND,    { 5, 3 }   },
      { FeatureRoomType::POND,     { 3, 3 }   },
      { FeatureRoomType::FOUNTAIN, { 3, 2 }   },
      { FeatureRoomType::SHRINE,   { 10, 1 }  }
    };

    Position roomSize = { 1, 10 };

    lb.FeatureRoomsMethod(mapSize, roomSize, weights, 3, mapSize.X * mapSize.Y);
    //lb.LogPrintMapRaw();
    //lb.PrintMapRaw();
    ss << lb.GetMapRawString();
  }

  inline void Run()
  {
    std::ofstream file;
    std::stringstream ss;

    file.open("tests.txt");

    //printf("***** START TESTS *****\n\n");
    ss << "***** START TESTS *****\n\n";

    TestLoS(ss, 4, 4, 2);
    //printf("\n- o -\n");
    ss << "\n- o -\n";
    RoomTests(ss);
    //printf("\n- o -\n");
    ss << "\n- o -\n";
    RNGTests(ss);
    //printf("\n- o -\n");
    ss << "\n- o -\n";
    LevelUpTests(ss);
    //printf("\n- o -\n");
    ss << "\n- o -\n";
    WeightedRandomTest(ss);
    //printf("\n- o -\n");
    ss << "\n- o -\n";
    LootDropTest(ss);
    //printf("\n- o -\n");
    ss << "\n- o -\n";
    LevelBuilderTest(ss);

    //printf("\n\n***** o *****\n");
    ss << "\n\n***** o *****\n";

    file << ss.str();

    file.close();

    printf("Test results have been written into 'tests.txt'\n\n");
  }  
}

#endif
