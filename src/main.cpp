#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "application.h"
#include "constants.h"
#include "util.h"

void TestLoS(int sx, int sy, int ex, int ey)
{
  auto res = Util::BresenhamLine(sx, sy, ex, ey);
  
  printf ("[%i;%i] -> [%i;%i]\n", sx, sy, ex, ey);
  
  for (auto& i : res)
  {
    printf("(%i %i) - ", i.X, i.Y);
  }
  
  printf("\n");
}

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
    
  Logger::Instance().Disabled = true;

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

  printf("Goodbye!\n");
  
  // Line of sight tests
  
  /*
  TestLoS(10, 10, 1, 10);
  TestLoS(10, 10, 1, 1);
  TestLoS(10, 10, 10, 1);
  TestLoS(10, 10, 20, 1);
  TestLoS(10, 10, 20, 10);
  TestLoS(10, 10, 20, 20);
  TestLoS(10, 10, 10, 20);
  TestLoS(10, 10, 1, 20);  
  */
  
  return 0;
}
