#define RUN_TESTS 0

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "application.h"
#include "game-objects-factory.h"
#include "map.h"
#include "printer.h"
#include "util.h"
#include "rng.h"

#if RUN_TESTS == 1
#include "tests.h"
#endif

// TODO:
//
// Status effects
// Ranged weapons / throwing
// Wands
// Scrolls
// Level design
// Skills
// Magic / Rare / Unique items
// Traps, secrets
// Locked doors / chests

int main()
{
  RNG::Instance().Init();
  RNG::Instance().SetSeed(1547020537474375167);
  //RNG::Instance().SetSeed(1);
  //RNG::Instance().SetSeed(1544714037606745311);
  //RNG::Instance().SetSeed(1545127588351497486);

  Logger::Instance().Init();
  Logger::Instance().Prepare(false);

  auto str = Util::StringFormat("World seed is %lu", RNG::Instance().Seed);
  Logger::Instance().Print(str);

  initscr();
  nodelay(stdscr, true);     // non-blocking getch()
  keypad(stdscr, true);      // enable numpad
  noecho();
  curs_set(false);
    
  start_color();

  Printer::Instance().Init();  

  GameObjectsFactory::Instance().Init();

  Application::Instance().Init();

  Map::Instance().Init();

  Application::Instance().Run();
  
  endwin();

  #if RUN_TESTS == 1
  Tests::Run();
  #endif

  printf("Goodbye!\n");

  return 0;
}
