#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "application.h"
#include "constants.h"
#include "printer.h"
#include "util.h"

void PrintTestScreen()
{
  int step = 4;

  for (int x = 0; x < Printer::Instance().TerminalWidth; x += step)
  {
    for (int y = 0; y < Printer::Instance().TerminalHeight; y += step)
    {
      Printer::Instance().Print(x, y, "x", Printer::kAlignLeft, "#FFFFFF");
    }
  }
}

int main()
{
  initscr();
  noecho();
  curs_set(false);

  int mx = GlobalConstants::MapX;
  int my = GlobalConstants::MapY;

  getmaxyx(stdscr, my, mx);

  start_color();

  Printer::Instance().Init();  
  Printer::Instance().TerminalWidth = mx;
  Printer::Instance().TerminalHeight = my;
  
  Map::Instance().Init();      
  Map::Instance().CreateMap();
  Map::Instance().MapOffsetX = mx / 2;
  Map::Instance().MapOffsetY = my / 2;
  
  PrintTestScreen();

  Printer::Instance().Print(mx / 2, my / 2, "Press any key to start", Printer::kAlignCenter, "#FFFFFF");
  
  auto resolution = Util::StringFormat("terminal size: %ix%i", mx, my);
  Printer::Instance().Print(mx, my - 1, resolution, Printer::kAlignRight, "#FFFFFF");  
  
  Application::Instance().Init();  
  Application::Instance().Run();
  
  endwin();

  printf("Goodbye!\n");
  
  return 0;
}
