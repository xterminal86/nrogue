#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "application.h"
#include "constants.h"
#include "util.h"
#include "tests.h"

bool _runTests = true;

int main()
{  
  auto scr = initscr();
  nodelay(scr, true);     // non-blocking getch()
  keypad(scr, true);      // enable numpad
  noecho();
  curs_set(false);

  int mx = GlobalConstants::MapX;
  int my = GlobalConstants::MapY;

  getmaxyx(stdscr, my, mx);

  start_color();

  Logger::Instance().Init();
  
  //Logger::Instance().Disabled = true;

  Printer::Instance().Init();  
  Printer::Instance().TerminalWidth = mx;
  Printer::Instance().TerminalHeight = my;
  
  Map::Instance().Init();      
  Map::Instance().CreateMap();
  Map::Instance().MapOffsetX = mx / 2;
  Map::Instance().MapOffsetY = my / 2;
 
  Application::Instance().Init();  
  Application::Instance().Run();
  
  endwin();

  if (_runTests)
  {
    Tests::Run();
  }

  printf("Goodbye!\n");

  return 0;
}
