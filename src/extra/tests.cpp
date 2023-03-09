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

        ss << Util::StringFormat("[%i;%i] -> [%i;%i]\n", x, y, fx, fy);

        for (auto& i : res)
        {
          ss << Util::StringFormat("(%i %i) - ", i.X, i.Y);
        }

        bool cond = (res[0].X == x && res[0].Y == y);
        if (!cond)
        {
          ss << "*** FAILED! ***\n";
        }

        ss << "\n";
      }
    }
  }

  void RoomTests(std::stringstream& ss)
  {
    ss << "\nRoom layouts rotations:\n\n";

    for (auto& room : GlobalConstants::DungeonRooms)
    {
      ss << "Layout:\n";

      for (auto& row : room)
      {
        ss << Util::StringFormat("%s\n", row.data());
      }

      ss << "CCW_90:\n";

      auto res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_90);
      for (auto& s : res)
      {
        ss << Util::StringFormat("\t%s\n", s.data());
      }

      ss << "CCW_180:\n";

      res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_180);
      for (auto& s : res)
      {
        ss << Util::StringFormat("\t%s\n", s.data());
      }

      ss << "CCW_270:\n";

      res = Util::RotateRoomLayout(room, RoomLayoutRotation::CCW_270);
      for (auto& s : res)
      {
        ss << Util::StringFormat("\t%s\n", s.data());
      }
    }
  }

  void RNGTests(std::stringstream& ss)
  {
    ss << "\nRNG tests:\n\n";

    ss << "Random.Range(-10, 10):\n";

    int numberOfRolls = 100;

    for (int i = 0; i < numberOfRolls; i++)
    {
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(-10, 10));
    }

    ss << "\n\nRandom.Range(0, 100):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(0, 100));
    }

    ss << "\n\nRandom.Range(-20, -10):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(-20, -10));
    }

    ss << "\n\nRandom.Range(100, 10):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(100, 10));
    }

    ss << "\n\nRandom.Range(-10, -100):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(-10, -100));
    }

    ss << "\n\nRandom.Range(10, -10):\n";

    for (int i = 0; i < numberOfRolls; i++)
    {
      ss << Util::StringFormat("%i ", RNG::Instance().RandomRange(10, -10));
    }

    const int d100 = 100;
    const int iterations = 1000;

    ss << Util::StringFormat("\n\n1RN and 2RN tests on %i iterations\n\n", iterations);
    ss << "For i < 100\n";
    ss << "  roll 1d100 'iterations' of times and check if result is less than i\n";
    ss << "\n";

    std::map<int, int> results1RN;
    std::map<int, int> results2RN;

    for (int i = 0; i < d100; i++)
    {
      results1RN[i] = 0;
      results2RN[i] = 0;
    }

    for (int i = 0; i < d100; i++)
    {
      for (int j = 0; j < iterations; j++)
      {
        bool res1RN = Util::Rolld100(i);
        bool res2RN = Util::Rolld100(i, true);

        results1RN[i] += res1RN ? 1 : 0;
        results2RN[i] += res2RN ? 1 : 0;
      }
    }

    for (int i = 0; i < d100; i++)
    {
      auto it1 = results1RN.begin();
      auto it2 = results2RN.begin();

      std::advance(it1, i);
      std::advance(it2, i);

      ss << Util::StringFormat("1RN[%d] = %d\n", it1->first, it1->second);
      ss << Util::StringFormat("2RN[%d] = %d\n", it2->first, it2->second);
      ss << "\n";
    }
  }

  void TestWeightsMap(const std::vector<std::tuple<GameObjectType, int, std::string>>& testData,
                      int rolls,
                      std::stringstream& ss)
  {
    std::map<GameObjectType, int> toTest;
    std::map<GameObjectType, std::string> stringNames;

    size_t maxLength = 0;
    for (auto& i : testData)
    {
      toTest.emplace(std::get<0>(i), std::get<1>(i));
      stringNames.emplace(std::get<0>(i), std::get<2>(i));

      if (std::get<2>(i).length() > maxLength)
      {
        maxLength = std::get<2>(i).length();
      }
    }

    ss << "\nTesting on " << rolls << " rolls\n\n";

    for (auto& kvp : toTest)
    {
      std::string spaces(maxLength - stringNames[kvp.first].length(), ' ');
      ss << "{ " << stringNames[kvp.first] << ", " << spaces << " w = " << kvp.second << " }\n";
    }

    ss << "\n\n";

    for (int i = 0; i < rolls; i++)
    {
      auto res = Util::WeightedRandom(toTest);
      std::string spaces(maxLength - stringNames[res.first].length(), ' ');
      ss << i << ") got " << stringNames[res.first] << ", " << spaces << " w = " << res.second << "\n";
    }
  }

  void AutoLevel(std::stringstream& ss, Player& p, int level)
  {
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
      p.LevelUpSilent();
    }

    for (auto& a : attrsMap)
    {
      auto kvp = a.second;

      ss << Util::StringFormat("%s: %i (* %i)\n", kvp.first.data(), kvp.second.OriginalValue(), kvp.second.Talents);
    }

    ss << "\n";
    ss << Util::StringFormat("HP: %i (* %i)\n", p.Attrs.HP.Max().Get(), p.Attrs.HP.Talents);
    ss << Util::StringFormat("MP: %i (* %i)\n", p.Attrs.MP.Max().Get(), p.Attrs.MP.Talents);
  }

  void LevelUpTests(std::stringstream& ss)
  {
    ss << "\nAuto levelling tests:\n\n";

    Player p1;

    p1.Attrs.Str.Talents = 3;
    p1.Attrs.Def.Talents = 2;
    p1.Attrs.Skl.Talents = 1;

    AutoLevel(ss, p1, 20);
  }

  void WeightedRandomTest(std::stringstream& ss)
  {
    ss << "\nWeighted random test:\n\n";

    std::map<GemType, int> gemsMap =
    {
      { GemType::WORTHLESS_GLASS, 250 },
      { GemType::BLACK_OBSIDIAN,  150 },
      { GemType::GREEN_JADE,      100 },
      { GemType::PURPLE_FLUORITE,  75 },
      { GemType::PURPLE_AMETHYST,  50 },
      { GemType::RED_GARNET,       43 },
      { GemType::WHITE_OPAL,       37 },
      { GemType::BLACK_JETSTONE,   35 },
      { GemType::ORANGE_AMBER,     30 },
      { GemType::BLUE_AQUAMARINE,  20 },
      { GemType::YELLOW_CITRINE,   20 },
      { GemType::GREEN_EMERALD,    12 },
      { GemType::BLUE_SAPPHIRE,    10 },
      { GemType::ORANGE_JACINTH,    9 },
      { GemType::RED_RUBY,          8 },
      { GemType::WHITE_DIAMOND,     7 },
    };

    int runs = 1000;

    ss << "Total scores on " << runs << " runs:\n\n";

    auto totalScores = Util::RollWeightsMap(gemsMap, runs);
    for (auto& kvp : totalScores)
    {
      ss << "[" << static_cast<int>(kvp.first) << "] = " << kvp.second << "\n";
    }

    std::vector<std::tuple<GameObjectType, int, std::string>> testData =
    {
      { GameObjectType::BAT,    20, "Bat"    },
      { GameObjectType::SPIDER, 10, "Spider" },
      { GameObjectType::TROLL,   1, "Troll"  }
    };

    std::map<GameObjectType, int> t =
    {
      { GameObjectType::BAT,    20 },
      { GameObjectType::SPIDER, 10 },
      { GameObjectType::TROLL,   1 }
    };

    auto r = Util::WeightsToProbability(t);

    ss << "\n";

    ss << "Bat, Spider, Troll probabilities:\n\n";

    for (auto& kvp : r)
    {
      ss << static_cast<int>(kvp.first) << " = " << kvp.second * 100.0 << "%\n";
    }

    TestWeightsMap(testData, 10, ss);
    TestWeightsMap(testData, 20, ss);
    TestWeightsMap(testData, 30, ss);

    testData =
    {
      { GameObjectType::BAT,    10, "Bat"    },
      { GameObjectType::SPIDER, 10, "Spider" },
      { GameObjectType::TROLL,   1, "Troll"  }
    };

    TestWeightsMap(testData, 10, ss);
    TestWeightsMap(testData, 20, ss);
    TestWeightsMap(testData, 30, ss);
  }

  void LootDropTest(std::stringstream& ss)
  {
    ss << "\nLoot drop test:\n\n";

    std::map<ItemType, int> scores;

    std::map<ItemType, int> lootTable =
    {
      { ItemType::FOOD,     4 },
      { ItemType::NOTHING, 20 }
    };

    for (int i = 0; i < 100; i++)
    {
      auto weights = lootTable;
      auto kvp = Util::WeightedRandom(weights);

      scores[kvp.first]++;
    }

    for (auto& kvp : scores)
    {
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
    int number = 100;

    ss << "\nGenerating " << number << " 'names':\n\n";

    for (int i = 0; i < number; i++)
    {
      auto n = Util::GenerateName(true);
      ss << i + 1 << ") " << n << "\n";
    }

    ss << "\n";
    ss << "no duplicate vowels:\n\n";

    for (int i = 0; i < number; i++)
    {
      auto n = Util::GenerateName();
      ss << i + 1 << ") " << n << "\n";
    }

    ss << "\n";
    ss << "no duplicate vowels (with endings):\n\n";

    for (int i = 0; i < number; i++)
    {
      auto n = Util::GenerateName(false, true, GlobalConstants::NameEndings);
      ss << i + 1 << ") " << n << "\n";
    }

    ss << "\n";
    ss << "'town' names:\n\n";

    for (int i = 0; i < number; i++)
    {
      auto n = Util::GenerateName(false, true, GlobalConstants::TownNameEndings);
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

    std::string padding(40, '*');

    ss << padding << " START TESTS " << padding << "\n\n";

    TestLoS(ss, 4, 4, 2);
    ss << "\n\n" << padding << " o " << padding << "\n";

    DisplayProgress();

    RoomTests(ss);
    ss << "\n\n" << padding << " o " << padding << "\n";

    DisplayProgress();

    RNGTests(ss);
    ss << "\n\n" << padding << " o " << padding << "\n";

    DisplayProgress();

    LevelUpTests(ss);
    ss << "\n\n" << padding << " o " << padding << "\n";

    DisplayProgress();

    WeightedRandomTest(ss);
    ss << "\n\n" << padding << " o " << padding << "\n";

    DisplayProgress();

    LootDropTest(ss);
    ss << "\n\n" << padding << " o " << padding << "\n";

    DisplayProgress();

    LevelBuilderTest(ss);
    ss << "\n\n" << padding << " o " << padding << "\n";

    DisplayProgress();

    GenNamesTest(ss);
    ss << "\n\n" << padding << " o " << padding << "\n";

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
