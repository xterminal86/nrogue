#include "select-class-state.h"
#include "printer.h"
#include "application.h"
#include "map.h"
#include "map-level-town.h"
#include "util.h"

void SelectClassState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case KEY_DOWN:
      _menuIndex++;
      break;

    case KEY_UP:
      _menuIndex--;
      break;

    case VK_ENTER:
    {
      Application::Instance().PlayerInstance.Init();      

      // NOTE: little bit of an out of place hack,
      // since some NPC can contain bonus chat lines depending on player class
      MapLevelTown* mlt = static_cast<MapLevelTown*>(Map::Instance().CurrentLevel);
      mlt->CreateNPCs();

      // TODO: enter name, distribute talents if custom class is chosen
      Application::Instance().ChangeState(GameStates::ENTER_NAME_STATE);
    }
    break;

    case 'q':
      Application::Instance().ChangeState(GameStates::MENU_STATE);
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
