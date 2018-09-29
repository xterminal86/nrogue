#include "mainstate.h"
#include "infostate.h"
#include "application.h"
#include "fighter-component.h"

void MainState::Init()
{
  _player = std::unique_ptr<GameObject>(new GameObject(Map::Instance().PlayerStartX,
                                                       Map::Instance().PlayerStartY, 
                                                      '@', "#00FFFF"));

  _player.get()->VisibilityRadius = 12;
  
  _player.get()->AddComponent(new FighterComponent());
  auto res = _player.get()->GetComponent(typeid(Component).hash_code());
}

void MainState::HandleInput()
{
  _keyPressed = getch();  
  
  switch (_keyPressed)
  {
    case NUMPAD_7:
      if (_player.get()->Move(-1, -1))
      {
        Map::Instance().MapOffsetY++;
        Map::Instance().MapOffsetX++;
      }
      break;
      
    case NUMPAD_8:
      if (_player.get()->Move(0, -1))
      {
        Map::Instance().MapOffsetY++;
      }
      break;

    case NUMPAD_9:
      if (_player.get()->Move(1, -1))
      {
        Map::Instance().MapOffsetY++;
        Map::Instance().MapOffsetX--;
      }
      break;

    case NUMPAD_4:
      if (_player.get()->Move(-1, 0))
      {
        Map::Instance().MapOffsetX++;
      }
      break;
    
    case NUMPAD_2:
      if (_player.get()->Move(0, 1))
      {
        Map::Instance().MapOffsetY--;
      }
      break;
    
    case NUMPAD_6:
      if (_player.get()->Move(1, 0))
      {
        Map::Instance().MapOffsetX--;
      }
      break;
    
    case NUMPAD_1:
      if (_player.get()->Move(-1, 1))
      {
        Map::Instance().MapOffsetY--;
        Map::Instance().MapOffsetX++;
      }
      break;

    case NUMPAD_3:
      if (_player.get()->Move(1, 1))
      {
        Map::Instance().MapOffsetY--;
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
  if (_keyPressed != -1)
  {
    clear();
    
    Map::Instance().Draw();
  
    _player.get()->CheckVisibility();
    _player.get()->Draw();
      
    for (auto& item : _mapObjects)
    {
      item.get()->Draw();
    }
    
    // Some debug info  
    _debugInfo = Util::StringFormat("[%i;%i] %i", _player.get()->PosX(), 
                                                  _player.get()->PosY(),
                                                  _keyPressed);    
    
    Printer::Instance().Print(0, Printer::Instance().TerminalHeight - 1, _debugInfo, Printer::kAlignLeft, "#FFFFFF");
    
    refresh();  
  }
}
