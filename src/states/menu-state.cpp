#include "menu-state.h"
#include "application.h"
#include "printer.h"
#include "util.h"

void MenuState::HandleInput()
{
  _keyPressed = GetKeyDown();

  switch (_keyPressed)
  {
    case VK_ENTER:
      Application::Instance().ChangeState(GameStates::SELECT_CLASS_STATE);
      break;

    case 'q':    
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

    int tw = Printer::Instance().TerminalWidth;
    int th = Printer::Instance().TerminalHeight;

    int halfW = tw / 2;
    int halfH = th / 2;

    int titleX = halfW;
    int titleY = halfH / 2 - _title.size() / 2;

    #ifdef USE_SDL
    Printer::Instance().DrawWindow({ 0, 0 },
                                   { tw - 1, th - 1 },
                                   "",
                                   "#000000",
                                   "#000000",
                                   "#FFFFFF");

    #else
    auto border = Util::GetPerimeter(0, 0, tw - 1, th - 1);
    for (auto& i : border)
    {      
      Printer::Instance().PrintFB(i.X, i.Y, ' ', "#FFFFFF", "#FFFFFF");
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
          Printer::Instance().PrintFB(titleX - xAlign + xOffset, titleY + yOffset, ' ', "#000000", "#FFFFFF");
        }
        else
        {
          Printer::Instance().PrintFB(titleX - xAlign + xOffset, titleY + yOffset, c, "#FFFFFF");
        }

        xOffset++;
      }

      yOffset++;
    }

    Printer::Instance().PrintFB(halfW, halfH + 2, "(press 'Enter' to start, 'q' to exit)", Printer::kAlignCenter, "#FFFFFF");
    Printer::Instance().PrintFB(tw - 2, th - 2, "(c) 2018 by xterminal86", Printer::kAlignRight, "#FFFFFF");

    std::string builtWith;

    #ifdef USE_SDL
    builtWith = "powered by SDL2";
    #else
      #if defined(__unix__) || defined(__linux__)
      builtWith = "powered by ncurses";
      #else
      builtWith = "powered by pdcurses";
      #endif
    #endif

    Printer::Instance().PrintFB(halfW, th - 2, builtWith, Printer::kAlignCenter, "#FFFFFF");

    auto debugInfo = Util::StringFormat("terminal size: %ix%i", tw, th);
    Printer::Instance().PrintFB(1, th - 2, debugInfo, Printer::kAlignLeft, "#FFFFFF");

    Printer::Instance().Render();
  }
}
