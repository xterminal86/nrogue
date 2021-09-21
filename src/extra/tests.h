#ifndef TESTS_H
#define TESTS_H

#include <sstream>

class Player;
class LevelBuilder;
class Position;

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

  void Tunneler(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss);
  void RecursiveBacktracker(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss);
  void CellularAutomata(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss);
  void Digger(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss);
  void BSPRooms(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss);
  void FeatureRooms(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss);
  void FromTiles(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss);
  void FromLayouts(LevelBuilder& lb, const Position& mapSize, std::stringstream& ss);

  void GenNamesTest(std::stringstream& ss);

  void Run();
  void DisplayProgress();
}

#endif
