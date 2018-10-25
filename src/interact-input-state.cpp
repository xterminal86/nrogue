#include "interact-input-state.h"

#include "application.h"
#include "map.h"
#include "printer.h"

void InteractInputState::Init()
{
  _playerRef = &Application::Instance().PlayerInstance;
}

void InteractInputState::Prepare()
{
  _cursorPosition.X = _playerRef->PosX;
  _cursorPosition.Y = _playerRef->PosY;

  Printer::Instance().AddMessage("Interact in which direction?");
}

void InteractInputState::HandleInput()
{
  _keyPressed = getch();

  bool dirSet = false;

  switch (_keyPressed)
  {
    case NUMPAD_7:
      dirSet = true;
      _cursorPosition.X -= 1;
      _cursorPosition.Y -= 1;
      break;

    case NUMPAD_8:
      dirSet = true;
      _cursorPosition.Y -= 1;
      break;

    case NUMPAD_9:
      dirSet = true;
      _cursorPosition.X += 1;
      _cursorPosition.Y -= 1;
      break;

    case NUMPAD_4:
      dirSet = true;
      _cursorPosition.X -= 1;
      break;

    case NUMPAD_6:
      dirSet = true;
      _cursorPosition.X += 1;
      break;

    case NUMPAD_1:
      dirSet = true;
      _cursorPosition.X -= 1;
      _cursorPosition.Y += 1;
      break;

    case NUMPAD_2:
      dirSet = true;
      _cursorPosition.Y += 1;
      break;

    case NUMPAD_3:
      dirSet = true;
      _cursorPosition.X += 1;
      _cursorPosition.Y += 1;
      break;

    case 'q':
      Printer::Instance().AddMessage("Cancelled");
      Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
      break;

    default:
      break;
  }

  if (dirSet)
  {
    auto res = Map::Instance().GetGameObjectsAtPosition(_cursorPosition.X, _cursorPosition.Y);
    if (res.size() != 0)
    {
      TryToInteractWithObject(res.back());
    }
    else
    {
      auto* cell = &Map::Instance().MapArray[_cursorPosition.X][_cursorPosition.Y];
      TryToInteractWithObject(cell);
    }
  }
}


void InteractInputState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    _playerRef->CheckVisibility();

    Map::Instance().Draw(_playerRef->PosX, _playerRef->PosY);

    _playerRef->Draw();

    Printer::Instance().PrintFB(Printer::Instance().TerminalWidth - 1,
                                Printer::Instance().TerminalHeight - 1,
                                Printer::Instance().GetLastMessage(),
                                Printer::kAlignRight,
                                "#FFFFFF");

    Printer::Instance().Render();
  }
}

void InteractInputState::TryToInteractWithObject(GameObject* go)
{
  if (go->Interact())
  {
    _playerRef->FinishTurn();
    Map::Instance().UpdateGameObjects();
    Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);    
  }
  else
  {
    Printer::Instance().AddMessage("Can't interact with: " + go->ObjectName);
    Application::Instance().ChangeState(Application::GameStates::MAIN_STATE);
  }
}
