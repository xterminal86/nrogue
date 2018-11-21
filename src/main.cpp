#define RUN_TESTS 0

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "application.h"
#include "game-objects-factory.h"
#include "constants.h"
#include "map.h"
#include "printer.h"
#include "util.h"
#include "rng.h"
#include "level-builder.h"

#if RUN_TESTS == 1

#include "tests.h"

#endif

int main()
{  
  Logger::Instance().Init();  
  Logger::Instance().Prepare(true);

  RNG::Instance().Init();
  RNG::Instance().SetSeed(1);

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
