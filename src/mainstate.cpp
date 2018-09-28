#include "mainstate.h"
#include "infostate.h"
#include "application.h"

void MainState::Init()
{
  _player = std::unique_ptr<GameObject>(new GameObject(Map::Instance().PlayerStartX,
                                                       Map::Instance().PlayerStartY, 
                                                      '@', "#00FFFF"));

  _player.get()->VisibilityRadius = 12;
}

void MainState::HandleInput()
{
  _keyPressed = getch();  
  
  switch (_keyPressed)
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
      Application::Instance().ChangeState(Application::GameStates::INFO_STATE);
      break;
      
    case 'q':
      Application::Instance().ChangeState(Application::GameStates::MENU_STATE);
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

  _player.get()->CheckVisibility();
  _player.get()->Draw();
  
  // Some debug info
  auto coords = Util::StringFormat("[%i;%i] %u", _player.get()->PosX(), 
                                                 _player.get()->PosY(),
                                                 _keyPressed);
  Printer::Instance().Print(0, Printer::Instance().TerminalHeight - 1, coords, Printer::kAlignLeft, "#FFFFFF");

  refresh();  
}
