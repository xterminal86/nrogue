#include "menu-state.h"
#include "application.h"
#include "printer.h"
#include "util.h"

void MenuState::Init()
{
  _titleX = _twHalf;
  _titleY = _thHalf / 2 - _title.size() / 2;

  _debugInfo = Util::StringFormat("terminal size: %ix%i", _tw, _th);
}

void MenuState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_ENTER:
      Application::Instance().ChangeState(GameStates::SELECT_CLASS_STATE);
      break;

    case VK_CANCEL:
      Application::Instance().ChangeState(GameStates::EXIT_GAME);
      break;

    default:
      break;
  }
}

void MenuState::Update(bool forceUpdate)
{
  if (_keyPressed != -1 || forceUpdate)
  {
    Printer::Instance().Clear();

    #ifdef USE_SDL
    Printer::Instance().DrawWindow({ 0, 0 },
                                   { _tw - 1, _th - 1 },
                                   "",
                                   GlobalConstants::BlackColor,
                                   GlobalConstants::BlackColor,
                                   GlobalConstants::WhiteColor);

    #else
    auto border = Util::GetPerimeter(0, 0, _tw - 1, _th - 1);
    for (auto& i : border)
    {
      Printer::Instance().PrintFB(i.X, i.Y, ' ', GlobalConstants::WhiteColor, GlobalConstants::WhiteColor);
    }
    #endif

    int yOffset = 0;
    for (auto& s : _title)
    {
      int xAlign = s.length() / 2;
      int xOffset = 0;
      for (auto& c : s)
      {
        if (c == '#')
        {
          Printer::Instance().PrintFB(_titleX - xAlign + xOffset,
                                       _titleY + yOffset,
                                       ' ',
                                       GlobalConstants::BlackColor,
                                       GlobalConstants::WhiteColor);
        }
        else
        {
          Printer::Instance().PrintFB(_titleX - xAlign + xOffset,
                                       _titleY + yOffset,
                                       c,
                                       GlobalConstants::WhiteColor);
        }

        xOffset++;
      }

      yOffset++;
    }

    Printer::Instance().PrintFB(_twHalf,
                                 _thHalf + 2,
                                 _welcome,
                                 Printer::kAlignCenter,
                                 GlobalConstants::WhiteColor);

    for (size_t i = 0; i < _signature.size(); i++)
    {
      Printer::Instance().PrintFB(_tw - 2,
                                   _th - 1 - (_signature.size() - i),
                                   _signature[i],
                                   Printer::kAlignRight,
                                   GlobalConstants::WhiteColor);
    }

    Printer::Instance().PrintFB(_twHalf,
                                 _th - 2,
                                 _builtWith,
                                 Printer::kAlignCenter,
                                 GlobalConstants::WhiteColor);

    Printer::Instance().PrintFB(2,
                                 _th - 2,
                                 _debugInfo,
                                 Printer::kAlignLeft,
                                 GlobalConstants::WhiteColor);

    Printer::Instance().Render();
  }
}
