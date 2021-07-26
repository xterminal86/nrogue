#ifndef TESTS_H
#define TESTS_H

#include <sstream>

class Player;

namespace Tests
{
  /// Test Bresenham line return vector in all directions
  void TestLoS(std::stringstream& ss, int x, int y, int range);

  void RoomTests(std::stringstream& ss);
  void RNGTests(std::stringstream& ss);

  void AutoLevel(std::stringstream& ss, Player& p, int level);
  void LevelUpTests(std::stringstream& ss);

  void WeightedRandomTest(std::stringstream& ss);
  void LootDropTest(std::stringstream& ss);

  void LevelBuilderTest(std::stringstream& ss);

  void GenNamesTest(std::stringstream& ss);

  void Run();
}

#endif
