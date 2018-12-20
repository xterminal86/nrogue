#ifndef TESTS_H
#define TESTS_H

#include "util.h"
#include "player.h"
#include "level-builder.h"

namespace Tests
{  
  /// Test Bresenham line return vector in all directions
  inline void TestLoS(int x, int y, int range)
  {
    printf("Starting Bresenham LoS test from (%i %i) in range %i...\n", x, y, range);
    printf("The path should always start from (%i %i)\n\n", x, y);

    int sx = x - range;
    int sy = y - range;
    int ex = x + range;
    int ey = y + range;

    for (int fx = sx; fx <= ex; fx++)
    {
      for (int fy = sy; fy <= ey; fy++)
      {
        auto res = Util::BresenhamLine(x, y, fx, fy);
    
        printf ("[%i;%i] -> [%i;%i]\n", x, y, fx, fy);
        
        for (auto& i : res)
        {
          printf("(%i %i) - ", i.X, i.Y);
        }
        
        bool cond = (res[0].X == x && res[0].Y == y);
        if (!cond)
        {
          printf("*** FAILED! ***\n");  
        }
        
        printf("\n");
      }
    }    
  }

  inline void RoomTests()
  {
    printf("\nRoom layouts rotations:\n\n");

    for (auto& room : GlobalConstants::DungeonRooms)
    {
      printf("Layout:\n");

      for (auto& row : room)
      {
        printf("%s\n", row.data());
      }

      printf("CCW_90:\n");

      auto res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_90);
      for (auto& s : res)
      {
        printf("\t%s\n", s.data());
      }

      printf("CCW_180:\n");

      res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_180);
      for (auto& s : res)
      {
        printf("\t%s\n", s.data());
      }

      printf("CCW_270:\n");

      res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_270);
      for (auto& s : res)
      {
        printf("\t%s\n", s.data());
      }
    }
  }

  inline void RNGTests()
  {
    printf("\nRNG tests:\n\n");

    printf("Random.Range(-10, 10):\n");

    int numberOfRolls = 100;

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(-10, 10));
    }

    printf("\n\nRandom.Range(0, 100):\n");

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(0, 100));
    }

    printf("\n\nRandom.Range(-20, -10):\n");

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(-20, -10));
    }

    printf("\n\nRandom.Range(100, 10):\n");

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(100, 10));
    }

    printf("\n\nRandom.Range(-10, -100):\n");

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(-10, -100));
    }

    printf("\n\nRandom.Range(10, -10):\n");

    for (int i = 0; i < numberOfRolls; i++)
    {
      printf("%i ", RNG::Instance().RandomRange(10, -10));
    }
  }

  inline void AutoLevel(Player& p, int level)
  {
    printf("Auto level to %i:\n\n", level);

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

      printf("%s: %i (* %i)\n", kvp.first.data(), kvp.second.CurrentValue, kvp.second.Talents);
    }

    printf("\n");
    printf("HP: %i (* %i)\n", p.Attrs.HP.OriginalValue, p.Attrs.HP.Talents);
    printf("MP: %i (* %i)\n", p.Attrs.MP.OriginalValue, p.Attrs.MP.Talents);
  }

  inline void LevelUpTests()
  {
    printf("\nAuto levelling tests:\n\n");

    Player p1;

    p1.Attrs.Str.Talents = 3;
    p1.Attrs.Def.Talents = 2;
    p1.Attrs.Skl.Talents = 1;

    AutoLevel(p1, 20);
  }

  inline void WeightedRandomTest()
  {
    printf("\nWeighted random test:\n\n");

    std::map<char, int> testMap =
    {
      { 'a', 8 },
      { 'b', 1 },
      { 'c', 2 }
    };

    for (auto& kvp : testMap)
    {
      printf("key: %c, weight: %i\n", kvp.first, kvp.second);
    }

    std::map<char, int> nums;

    nums['a'] = 0;
    nums['b'] = 0;
    nums['c'] = 0;

    int iterations = 10000;

    printf("\n%i iterations:\n", iterations);

    for (int i = 0; i < iterations; i++)
    {
      auto res = Util::WeightedRandom(testMap);
      nums[res.first] += 1;
    }

    printf("\n");

    for (auto& kvp : nums)
    {
      printf("total scores: %c = %i\n", kvp.first, kvp.second);
    }
  }

  inline void LootDropTest()
  {
    printf("\nLoot drop test:\n\n");

    std::map<ItemType, int> scores;

    for (int i = 0; i < 100; i++)
    {
      auto weights = GlobalConstants::LootTable.at(MonsterType::RAT);
      auto kvp = Util::WeightedRandom(weights);

      scores[kvp.first]++;
    }

    for (auto& kvp : scores)
    {
      printf("%i = %i\n", kvp.first, kvp.second);
    }
  }

  inline void LevelBuilderTest()
  {
    std::string str;

    str = "\nLevel builders:\n\n";

    Logger::Instance().Print(str);
    printf("%s", str.data());

    LevelBuilder lb;

    Position s(1, 1);
    Position mapSize(24, 78);
    //Position mapSize(200, 200);

    str = "1) Tunneler:\n\n";
    printf("%s", str.data());
    Logger::Instance().Print(str);

    lb.TunnelerMethod(mapSize, 50, { 10, 20 }, s);
    lb.MapRaw[s.X][s.Y] = 'X';
    lb.LogPrintMapRaw();
    lb.PrintMapRaw();

    str = "2) Tunneler (backtracking):\n\n";
    printf("%s", str.data());
    Logger::Instance().Print(str);

    lb.BacktrackingTunnelerMethod(mapSize, { 5, 20 }, s, true);
    lb.MapRaw[s.X][s.Y] = 'X';
    lb.LogPrintMapRaw();
    lb.PrintMapRaw();

    str = "3) Recursive Backtracker:\n\n";
    printf("%s", str.data());
    Logger::Instance().Print(str);

    lb.RecursiveBacktrackerMethod(mapSize, s);
    lb.MapRaw[s.X][s.Y] = 'X';
    lb.LogPrintMapRaw();
    lb.PrintMapRaw();

    str = "4) Cellular Automata:\n\n";
    Logger::Instance().Print(str);
    printf("%s", str.data());

    lb.CellularAutomataMethod(mapSize, 40, 5, 4, 12);
    lb.LogPrintMapRaw();
    lb.PrintMapRaw();

    str = "5) Digger:\n\n";
    Logger::Instance().Print(str);
    printf("%s", str.data());

    lb.FeatureRoomsMethod(mapSize, { 1, 14 }, mapSize.X * mapSize.Y);
    lb.LogPrintMapRaw();
    lb.PrintMapRaw();
  }

  inline void Run()
  {
    printf("***** START TESTS *****\n\n");

    TestLoS(4, 4, 2);
    printf("\n- o -\n");
    RoomTests();
    printf("\n- o -\n");
    RNGTests();
    printf("\n- o -\n");
    LevelUpTests();
    printf("\n- o -\n");
    WeightedRandomTest();
    printf("\n- o -\n");
    LootDropTest();
    printf("\n- o -\n");
    LevelBuilderTest();

    printf("\n\n***** o *****\n");
  }  
}

#endif
