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
    #endif
    //
      _menuIndex--;
      break;

    case VK_ENTER:
      if (_menuIndex == 3)
      {
        Game::gApp.ChangeState(GameStates::CUSTOM_CLASS_STATE);
      }
      else
      {
        Game::gApp.ChangeState(GameStates::ENTER_NAME_STATE);
      }
      break;

    case VK_CANCEL:
      Game::gApp.ChangeState(GameStates::MENU_STATE);
      break;

    default:
      break;
  }

  _menuIndex = Util::Clamp(_menuIndex, 0, _menuItems.size() - 1);

  Game::gApp.PlayerInstance.SelectedClass = _menuIndex;
}

// =============================================================================

void SelectClassState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Game::gPrnt.Clear();

    Game::gPrnt.PrintText(
      _twHalf,
      _thHalf - 4,
      "Who are you?",
      Printer::kAlignCenter,
      Colors::White,
      Colors::Black
    );

    int offset = 0;
    int index = 0;
    for (auto& i : _menuItems)
    {
      uint32_t bgColor = (index == _menuIndex)
                        ? Colors::ShadesOfGrey::Four
                        : Colors::Black;

      Game::gPrnt.PrintText(
        _twHalf,
        _thHalf + offset,
        i,
        Printer::kAlignCenter,
        Colors::White,
        bgColor
      );

      index++;
      offset++;
    }

    Game::gPrnt.PrintText(
      _twHalf,
      _th - 1,
      "WARNING: not fully implemented yet!",
      Printer::kAlignCenter,
      Colors::Yellow,
      Colors::Black
    );

    Game::gPrnt.Render();
  }
}
