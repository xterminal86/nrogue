#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <memory>

#include "constants.h"
#include "printer.h"
#include "game-object.h"
#include "map.h"
#include "colorpair.h"
#include "util.h"

std::vector<std::unique_ptr<GameObject>> _mapObjects;

bool _gameRunning = true;

std::unique_ptr<GameObject> _player;

void DrawAll()
{
  Map::Instance().Draw();

  for (auto& item : _mapObjects)
  {
    item.get()->Draw();
  }

  _player.get()->Draw();
}

void QueryInput(char ch)
{
  switch(ch)
  {
    case 'w':
      _player.get()->Move(0, -1);
      Map::Instance().MapOffsetY++;
      break;

    case 's':
      _player.get()->Move(0, 1);
      Map::Instance().MapOffsetY--;
      break;

    case 'a':
      _player.get()->Move(-1, 0);
      Map::Instance().MapOffsetX++;
      break;

    case 'd':
      _player.get()->Move(1, 0);
      Map::Instance().MapOffsetX--;
      break;

    case 'q':
      _gameRunning = false;
      break;
  }
}

void PrintTestScreen()
{
  int step = 4;

  for (int x = 0; x < 80; x += step)
  {
    for (int y = 0; y < 25; y += step)
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

  Map::Instance().Init();
  Printer::Instance().Init();

  //auto npc = std::make_unique<GameObject>(18, 12, '@', 2, COLOR_YELLOW);

  //_mapObjects.push_back(std::move(npc));

  Map::Instance().CreateMap();

  Map::Instance().MapOffsetX = 40;
  Map::Instance().MapOffsetY = 12;

  _player = std::make_unique<GameObject>(Map::Instance().PlayerStartX,
                                         Map::Instance().PlayerStartY, '@', "#00FFFF");

  PrintTestScreen();

  Printer::Instance().Print(40, 14, "Press any key to start", Printer::kAlignCenter, "#FFFFFF");

  while (_gameRunning)
  {
    char ch = getch();

    // Causes flickering due to lack of doublebuffering

    clear();

    //_player.get()->Clear();

    QueryInput(ch);

    DrawAll();

    // player coords
    auto coords = Util::StringFormat("[%i;%i]", _player.get()->PosX(), _player.get()->PosY());
    Printer::Instance().Print(0, 30, coords, Printer::kAlignLeft, "#FFFFFF");

    refresh();
  }

  endwin();

  return 0;
}
