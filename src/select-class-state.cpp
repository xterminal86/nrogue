#include "select-class-state.h"
#include "printer.h"
#include "application.h"

void SelectClassState::HandleInput()
{
  _keyPressed = getch();

  switch (_keyPressed)
  {
    case KEY_DOWN:
      _menuIndex++;
      break;

    case KEY_UP:
      _menuIndex--;
      break;

    case 10:
      Application::Instance().PlayerInstance.Init();

      // TODO: enter name, distribute talents if custom class is chosen
      Application::Instance().ChangeState(Application::GameStates::INTRO_STATE);
      break;

    case 'q':
      Application::Instance().ChangeState(Application::GameStates::MENU_STATE);
      break;
  }

  _menuIndex = Util::Clamp(_menuIndex, 0, _menuItems.size() - 1);

  Application::Instance().PlayerInstance.SelectedClass = _menuIndex;
}

void SelectClassState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    Printer::Instance().PrintFB(tw / 2, th / 2 - 4, "Who are you?", Printer::kAlignCenter, "#FFFFFF");

    int offset = 0;
    int index = 0;
    for (auto& i : _menuItems)
    {
      std::string fgColor = (index == _menuIndex) ? "#000000" : "#FFFFFF";
      std::string bgColor = (index == _menuIndex) ? "#FFFFFF" : "#000000";

      Printer::Instance().PrintFB(tw / 2, th / 2 + offset, i, Printer::kAlignCenter, fgColor, bgColor);

      index++;
      offset++;
    }

    Printer::Instance().Render();
  }
}
