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

  auto scr = initscr();
  nodelay(scr, true);     // non-blocking getch()
  keypad(scr, true);      // enable numpad
  noecho();
  curs_set(false);
  
  int mx = 0;
  int my = 0;

  getmaxyx(scr, my, mx);

  start_color();

  Printer::Instance().Init();  
  Printer::Instance().TerminalWidth = mx;
  Printer::Instance().TerminalHeight = my;

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
