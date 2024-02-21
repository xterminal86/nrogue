#include "select-class-state.h"
#include "printer.h"
#include "application.h"
#include "util.h"

void SelectClassState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case ALT_K2:
    case NUMPAD_2:
    //
    #ifdef USE_SDL
    case KEY_DOWN:
    #endif
    //
      _menuIndex++;
      break;

    case ALT_K8:
    case NUMPAD_8:
    //
    #ifdef USE_SDL
    case KEY_UP:
    //
    #endif
      _menuIndex--;
      break;

    case VK_ENTER:
      if (_menuIndex == 3)
      {
        Application::Instance().ChangeState(GameStates::CUSTOM_CLASS_STATE);
      }
      else
      {
        Application::Instance().ChangeState(GameStates::ENTER_NAME_STATE);
      }
      break;

    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::MENU_STATE);
      break;

    default:
      break;
  }

  _menuIndex = Util::Clamp(_menuIndex, 0, _menuItems.size() - 1);

  Application::Instance().PlayerInstance.SelectedClass = _menuIndex;
}

// =============================================================================

void SelectClassState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    Printer::Instance().PrintFB(_twHalf,
                                _thHalf - 4,
                                "Who are you?",
                                Printer::kAlignCenter,
                                Colors::WhiteColor,
                                Colors::BlackColor);

    int offset = 0;
    int index = 0;
    for (auto& i : _menuItems)
    {
      uint32_t bgColor = (index == _menuIndex)
                        ? Colors::ShadesOfGrey::Four
                        : Colors::BlackColor;

      Printer::Instance().PrintFB(_twHalf,
                                  _thHalf + offset,
                                  i,
                                  Printer::kAlignCenter,
                                  Colors::WhiteColor,
                                  bgColor);

      index++;
      offset++;
    }

    Printer::Instance().PrintFB(_twHalf,
                                _th - 1,
                                "WARNING: not fully implemented yet!",
                                Printer::kAlignCenter,
                                Colors::YellowColor,
                                Colors::BlackColor);

    Printer::Instance().Render();
  }
}
