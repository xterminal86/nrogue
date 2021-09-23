#include "tests.h"

#include "util.h"
#include "player.h"
#include "level-builder.h"
#include "pathfinder.h"

#include <fstream>

namespace Tests
{
  void TestLoS(std::stringstream& ss, int x, int y, int range)
  {
    //DebugLog("Starting Bresenham LoS test from (%i %i) in range %i...\n", x, y, range);
    //DebugLog("The path should always start from (%i %i)\n\n", x, y);
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

        //DebugLog ("[%i;%i] -> [%i;%i]\n", x, y, fx, fy);
        ss << Util::StringFormat("[%i;%i] -> [%i;%i]\n", x, y, fx, fy);

        for (auto& i : res)
        {
          //DebugLog("(%i %i) - ", i.X, i.Y);
          ss << Util::StringFormat("(%i %i) - ", i.X, i.Y);
        }

        bool cond = (res[0].X == x && res[0].Y == y);
        if (!cond)
        {
          //DebugLog("*** FAILED! ***\n");
          ss << "*** FAILED! ***\n";
        }

        //DebugLog("\n");
        ss << "\n";
      }
    }
  }

  void RoomTests(std::stringstream& ss)
  {
    //DebugLog("\nRoom layouts rotations:\n\n");
    ss << "\nRoom layouts rotations:\n\n";

    for (auto& room : GlobalConstants::DungeonRooms)
    {
      //DebugLog("Layout:\n");
      ss << "Layout:\n";

      for (auto& row : room)
      {
        //DebugLog("%s\n", row.data());
        ss << Util::StringFormat("%s\n", row.data());
      }

      //DebugLog("CCW_90:\n");
      ss << "CCW_90:\n";

      auto res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_90);
      for (auto& s : res)
      {
        //DebugLog("\t%s\n", s.data());
        ss << Util::StringFormat("\t%s\n", s.data());
      }

      //DebugLog("CCW_180:\n");
      ss << "CCW_180:\n";

      res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_180);
      for (auto& s : res)
      {
        //DebugLog("\t%s\n", s.data());
        ss << Util::StringFormat("\t%s\n", s.data());
      }

      //DebugLog("CCW_270:\n");
      ss << "CCW_270:\n";

      res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_270);
      for (auto& s : res)
      {
        //DebugLog("\t%s\n", s.data());
        ss << Util::StringFormat("\t%s\n", s.data());
      }
    }
  }

  void RNGTests(std::stringstream& ss)
  {
    //DebugLog("\nRNG tests:\n\n");
    ss << "\nRNG tests:\n\n";

    //DebugLog("Random.Range(-10, 10):\n");
    ss << "Random.Range(-10, 10):\n";

    int numberOfRolls = 100;

    for (int i = 0; i < numberOfRolls; i++)
    {
      //DebugLog("%i ", RNG::Instance().RandomRange(-10, 10));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(-10, 10));
    }

    //DebugLog("\n\nRandom.Range(0, 100):\n");
    ss << "\n\nRandom.Range(0, 100):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      //DebugLog("%i ", RNG::Instance().RandomRange(0, 100));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(0, 100));
    }

    //DebugLog("\n\nRandom.Range(-20, -10):\n");
    ss << "\n\nRandom.Range(-20, -10):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      //DebugLog("%i ", RNG::Instance().RandomRange(-20, -10));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(-20, -10));
    }

    //DebugLog("\n\nRandom.Range(100, 10):\n");
    ss << "\n\nRandom.Range(100, 10):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      //DebugLog("%i ", RNG::Instance().RandomRange(100, 10));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(100, 10));
    }

    //DebugLog("\n\nRandom.Range(-10, -100):\n");
    ss << "\n\nRandom.Range(-10, -100):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      //DebugLog("%i ", RNG::Instance().RandomRange(-10, -100));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(-10, -100));
    }

    //DebugLog("\n\nRandom.Range(10, -10):\n");
    ss << "\n\nRandom.Range(10, -10):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      //DebugLog("%i ", RNG::Instance().RandomRange(10, -10));
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(10, -10));
    }
  }

  void AutoLevel(std::stringstream& ss, Player& p, int level)
  {
    //DebugLog("Auto level to %i:\n\n", level);
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

      //DebugLog("%s: %i (* %i)\n", kvp.first.data(), kvp.second.OriginalValue(), kvp.second.Talents);
      ss << Util::StringFormat("%s: %i (* %i)\n", kvp.first.data(), kvp.second.OriginalValue(), kvp.second.Talents);
    }

    //DebugLog("\n");
    //DebugLog("HP: %i (* %i)\n", p.Attrs.HP.Max().Get(), p.Attrs.HP.Talents);
    //DebugLog("MP: %i (* %i)\n", p.Attrs.MP.Max().Get(), p.Attrs.MP.Talents);
    ss << "\n";
    ss << Util::StringFormat("HP: %i (* %i)\n", p.Attrs.HP.Max().Get(), p.Attrs.HP.Talents);
    ss << Util::StringFormat("MP: %i (* %i)\n", p.Attrs.MP.Max().Get(), p.Attrs.MP.Talents);
  }

  void LevelUpTests(std::stringstream& ss)
  {
    //DebugLog("\nAuto levelling tests:\n\n");
    ss << "\nAuto levelling tests:\n\n";

    Player p1;

    p1.Attrs.Str.Talents = 3;
    p1.Attrs.Def.Talents = 2;
    p1.Attrs.Skl.Talents = 1;

    AutoLevel(ss, p1, 20);
  }

  void WeightedRandomTest(std::stringstream& ss)
  {
    //DebugLog("\nWeighted random test:\n\n");
    ss << "\nWeighted random test:\n\n";

    std::map<char, int> testMap =
    {
      { 'a', 8 },
      { 'b', 1 },
      { 'c', 2 }
    };

    for (auto& kvp : testMap)
    {
      //DebugLog("key: %c, weight: %i\n", kvp.first, kvp.second);
      ss << Util::StringFormat("key: %c, weight: %i\n", kvp.first, kvp.second);
    }

    std::map<char, int> nums;

    nums['a'] = 0;
    nums['b'] = 0;
    nums['c'] = 0;

    int iterations = 10000;

    //DebugLog("\n%i iterations:\n", iterations);
    ss << Util::StringFormat("\n%i iterations:\n", iterations);

    for (int i = 0; i < iterations; i++)
    {
      auto res = Util::WeightedRandom(testMap);
      nums[res.first] += 1;
    }

    //DebugLog("\n");
    ss << "\n";

    for (auto& kvp : nums)
    {
      //DebugLog("total scores: %c = %i\n", kvp.first, kvp.second);
      ss << Util::StringFormat("total scores: %c = %i\n", kvp.first, kvp.second);
    }
  }

  void LootDropTest(std::stringstream& ss)
  {
    //DebugLog("\nLoot drop test:\n\n");
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
      //DebugLog("%i = %i\n", kvp.first, kvp.second);
      ss << Util::StringFormat("%i = %i\n", kvp.first, kvp.second);
    }
  }

  void Tunneler(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss)
  {
    std:: string str = "\nTunneler:\n\n";

    Position start(1, 1);

    ss << str;

    int iterations = (mapSize.X * mapSize.Y) / 2;
    lb.TunnelerMethod(mapSize, iterations, { 5, 10 }, start);
    lb.MapRaw[start.X][start.Y] = 'X';

    ss << lb.GetMapRawString();

    str = "\nTunneler (backtracking):\n\n";

    ss << str;

    lb.BacktrackingTunnelerMethod(mapSize, { 5, 10 }, start, true);
    lb.MapRaw[start.X][start.Y] = 'X';

    ss << lb.GetMapRawString();
  }

  void RecursiveBacktracker(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss)
  {
    std::string str = "\nRecursive Backtracker:\n\n";

    Position start(1, 1);

    ss << str;

    lb.RecursiveBacktrackerMethod(mapSize, start);
    lb.MapRaw[start.X][start.Y] = 'X';

    ss << lb.GetMapRawString();

    str = "\nRecursive Backtracker (end walls removed):\n\n";

    ss << str;

    std::vector<RemovalParams> removalParams =
    {
      // ===============
      // ===============
      { 3, 8, 1 },
      { 4, 8, 1 },
      { 5, 8, 1 },
      { 6, 8, 1 },
      { 7, 8, 1 },
      // ===========
      { 3, 7, 1 },
      { 4, 7, 1 },
      { 5, 7, 1 },
      { 6, 7, 1 },
      // ===========
      { 3, 6, 1 },
      { 4, 6, 1 },
      { 5, 6, 1 },
      // ===========
      { 3, 5, 1 },
      { 4, 5, 1 },
      // ===============
      // ===============
      { 3, 8, 2 },
      { 4, 8, 2 },
      { 5, 8, 2 },
      { 6, 8, 2 },
      { 7, 8, 2 },
      // ===========
      { 3, 7, 2 },
      { 4, 7, 2 },
      { 5, 7, 2 },
      { 6, 7, 2 },
      // ===========
      { 3, 6, 2 },
      { 4, 6, 2 },
      { 5, 6, 2 },
      // ===========
      { 3, 5, 2 },
      { 4, 5, 2 },
      // ===============
      // ===============
      { 3, 8, 3 },
      { 4, 8, 3 },
      { 5, 8, 3 },
      { 6, 8, 3 },
      { 7, 8, 3 },
      // ===========
      { 3, 7, 3 },
      { 4, 7, 3 },
      { 5, 7, 3 },
      { 6, 7, 3 },
      // ===========
      { 3, 6, 3 },
      { 4, 6, 3 },
      { 5, 6, 3 },
      // ===========
      { 3, 5, 3 },
      { 4, 5, 3 },
      // ===============
      // ===============
      { 3, 3, 1 },
      { 4, 4, 1 },
      { 5, 5, 1 },
      { 6, 6, 1 },
      { 7, 7, 1 },
      { 8, 8, 1 },
      // ===========
      { 3, 3, 2 },
      { 4, 4, 2 },
      { 5, 5, 2 },
      { 6, 6, 2 },
      { 7, 7, 2 },
      { 8, 8, 2 },
      // ===========
      { 3, 3, 3 },
      { 4, 4, 3 },
      { 5, 5, 3 },
      { 6, 6, 3 },
      { 7, 7, 3 },
      { 8, 8, 3 }
    };

    for (auto i : removalParams)
    {
      str = "";

      for (size_t i = 0; i < 30; i++)
      {
        str += " ";
      }

      str += Util::StringFormat("[REMOVAL PARAMS] -> { %i, %i, %i }\n\n", i.EmptyCellsAroundMin, i.EmptyCellsAroundMax, i.Passes);

      ss << str;

      lb.RecursiveBacktrackerMethod({ 40, 80 }, start, i);
      lb.MapRaw[start.X][start.Y] = 'X';

      ss << lb.GetMapRawString();

      ss << "\n\n================================================================================\n\n";
    }
  }

  void CellularAutomata(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss)
  {
    std::string str = "\nCellular Automata:\n\n";

    ss << str;

    lb.CellularAutomataMethod(mapSize, 40, 5, 4, 12);

    ss << lb.GetMapRawString();
  }

  void Digger(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss)
  {
    std::string str = "\nDigger:\n\n";

    ss << str;

    int iterations = (mapSize.X * mapSize.Y) / 10;

    FeatureRoomsWeights weights =
    {
      { FeatureRoomType::EMPTY, { 10, 0 }  }
    };

    lb.FeatureRoomsMethod(mapSize, { 1, 10 }, weights, 3, iterations);

    ss << lb.GetMapRawString();
  }

  void BSPRooms(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss)
  {
    std::string str = "\nBSP rooms:\n\n";

    ss << str;

    lb.RoomsMethod(mapSize, { 45, 55 }, 5);

    ss << lb.GetMapRawString();
  }

  void FeatureRooms(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss)
  {
    std::string str = "\nFeature rooms:\n\n";

    ss << str;

    FeatureRoomsWeights weights =
    {
      { FeatureRoomType::EMPTY,    { 10, 0 } },
      { FeatureRoomType::DIAMOND,  {  3, 3 } },
      { FeatureRoomType::FLOODED,  {  1, 3 } },
      { FeatureRoomType::GARDEN,   {  3, 3 } },
      { FeatureRoomType::PILLARS,  {  5, 0 } },
      { FeatureRoomType::ROUND,    {  5, 3 } },
      { FeatureRoomType::POND,     {  3, 3 } },
      { FeatureRoomType::FOUNTAIN, {  3, 2 } },
      { FeatureRoomType::SHRINE,   { 10, 1 } }
    };

    Position roomSize = { 1, 10 };

    lb.FeatureRoomsMethod(mapSize, roomSize, weights, 3, mapSize.X * mapSize.Y);

    ss << lb.GetMapRawString();
  }

  void FromTiles(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss)
  {
    std::string str = "\nFrom tiles:\n";

    ss << str;

    for (int i = 0; i < 9; i++)
    {
      std::string title(30, ' ');
      std::string delimiter(80, '=');

      title += Util::StringFormat(">>>> Tileset %i <<<<\n", i + 1);

      ss << "\n\n" << title << "\n";
      ss << delimiter << "\n\n";

      lb.FromTilesMethod({ 40, 80 }, i);

      ss << lb.GetMapRawString();

      // Postprocessed

      std::string postProcessed(30, ' ');

      postProcessed += "    (with post-processing)    \n";

      ss << "\n\n" << postProcessed << "\n";

      lb.FromTilesMethod({ 40, 80 }, i, true, true);

      ss << lb.GetMapRawString();
    }
  }

  void FromLayouts(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss)
  {
    std::string str = "\nFrom layouts:\n\n";

    ss << str;

    std::vector<RoomForLevel> rooms =
    {
      {
        50,
        {
          "###",
          "###",
          "###"
        }
      },
      {
        50,
        {
          "#.#",
          "...",
          "#.#"
        }
      },
      {
        50,
        {
          "#.#",
          "#..",
          "###"
        }
      },
      {
        50,
        {
          "#.#",
          "...",
          "###"
        }
      },
      {
        50,
        {
          "#.#",
          "#.#",
          "#.#"
        }
      },
    };


    lb.BuildLevelFromLayouts(rooms, mapSize.X / 2, mapSize.Y / 2, mapSize.X, mapSize.Y);

    ss << lb.GetMapRawString();
  }

  //
  // NOTE: for non square map size dimensions must be swapped
  // because, well, you know.
  // Another day, same shit.
  //
  void LevelBuilderTest(std::stringstream& ss)
  {
    std::string str;

    str = "\nLevel builders:\n\n";

    ss << str;

    LevelBuilder lb;

    //Position mapSize(24, 78); // Check comments above this method
    Position mapSize(100, 100);

    DisplayProgress();
    Tunneler(lb, mapSize, ss);
    //
    DisplayProgress();
    RecursiveBacktracker(lb, mapSize, ss);
    //
    DisplayProgress();
    CellularAutomata(lb, mapSize, ss);
    //
    DisplayProgress();
    Digger(lb, mapSize, ss);
    //
    DisplayProgress();
    BSPRooms(lb, mapSize, ss);
    //
    DisplayProgress();
    FeatureRooms(lb, mapSize, ss);
    //
    DisplayProgress();
    FromTiles(lb, mapSize, ss);
    //
    DisplayProgress();
    FromLayouts(lb, mapSize, ss);
  }

  void GenNamesTest(std::stringstream& ss)
  {
    ss << "\nGenerating 20 'names':\n\n";

    for (int i = 0; i < 20; i++)
    {
      auto n = Util::GenerateName(true);
      ss << i + 1 << ") " << n << "\n";
    }

    ss << "\n";
    ss << "no duplicate vowels:\n\n";

    for (int i = 0; i < 20; i++)
    {
      auto n = Util::GenerateName();
      ss << i + 1 << ") " << n << "\n";
    }

    ss << "\n";
  }

  void Run()
  {
    std::ofstream file;
    std::stringstream ss;

    file.open("tests.txt");

    DisplayProgress();

    //DebugLog("***** START TESTS *****\n\n");
    ss << "***** START TESTS *****\n\n";

    TestLoS(ss, 4, 4, 2);
    ss << "\n- o -\n";

    DisplayProgress();

    RoomTests(ss);
    ss << "\n- o -\n";

    DisplayProgress();

    RNGTests(ss);
    ss << "\n- o -\n";

    DisplayProgress();

    LevelUpTests(ss);
    ss << "\n- o -\n";

    DisplayProgress();

    WeightedRandomTest(ss);
    ss << "\n- o -\n";

    DisplayProgress();

    LootDropTest(ss);
    ss << "\n- o -\n";

    DisplayProgress();

    LevelBuilderTest(ss);
    ss << "\n- o -\n";

    DisplayProgress();

    GenNamesTest(ss);

    //DebugLog("\n\n***** o *****\n");
    ss << "\n\n***** o *****\n";

    file << ss.str();

    file.close();

    DebugLog("Test results have been written into 'tests.txt'\n\n");
  }

  void DisplayProgress()
  {
    static int progress = 0;
    DebugLog("\t\t%i\\15\n", progress);
    progress++;
  }
}
