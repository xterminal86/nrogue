#include "mainstate.h"
#include "infostate.h"
#include "application.h"

void MainState::Init()
{
  _player = std::unique_ptr<GameObject>(new GameObject(Map::Instance().PlayerStartX,
                                                       Map::Instance().PlayerStartY, 
                                                      '@', "#00FFFF"));
                                                      
  auto npc = std::make_unique<GameObject>(18, 12, '@', "#FFFF00");
  _mapObjects.push_back(std::move(npc));  
}

void MainState::HandleInput()
{
  char ch = getch();  
  
  switch (ch)
  {
    case 'w':
      if (_player.get()->Move(0, -1))
      {
        Map::Instance().MapOffsetY++;
      }
      break;

    case 's':
      if (_player.get()->Move(0, 1))
      {
        Map::Instance().MapOffsetY--;
      }
      break;

    case 'a':
      if (_player.get()->Move(-1, 0))
      {
        Map::Instance().MapOffsetX++;
      }
      break;

    case 'd':
      if (_player.get()->Move(1, 0))
      {
        Map::Instance().MapOffsetX--;
      }
      break;
    
    case '@':
      Application::Instance().ChangeState(Application::Instance().kInfoStateIndex);
      break;
      
    case 'q':
      Application::Instance().ChangeState(Application::Instance().kExitGameIndex);
      break;
      
    default:
      break;
  }  
}

void MainState::Update()
{
  clear();
  
  Map::Instance().Draw();
  
  for (auto& item : _mapObjects)
  {
    item.get()->Draw();
  }

  _player.get()->Draw();
  
  // player coords
  auto coords = Util::StringFormat("[%i;%i]", _player.get()->PosX(), _player.get()->PosY());
  Printer::Instance().Print(0, Printer::Instance().TerminalHeight - 1, coords, Printer::kAlignLeft, "#FFFFFF");
  
  refresh();  
}
