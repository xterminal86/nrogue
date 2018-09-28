#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "application.h"
#include "constants.h"

int main()
{  
  initscr();
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

  printf("Goodbye!\n");
   
  return 0;
}
