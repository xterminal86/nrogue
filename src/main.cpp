#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "application.h"
#include "constants.h"
#include "map.h"
#include "tests.h"
#include "printer.h"
#include "util.h"
#include "rng.h"

bool _runTests = false;

int main()
{  
  Logger::Instance().Init();  
  Logger::Instance().Prepare(true);

  RNG::Instance().Init();

  initscr();
  nodelay(stdscr, true);     // non-blocking getch()
  keypad(stdscr, true);      // enable numpad
  noecho();
  curs_set(false);
    
  start_color();

  Printer::Instance().Init();  

  Application::Instance().Init();

  Map::Instance().Init();      
  Map::Instance().CreateMap(MapType::TOWN);

  Application::Instance().Run();
  
  endwin();

  if (_runTests)
  {
    Tests::Run();
  }
    
  printf("Goodbye!\n");

  return 0;
}
