#include "gamestate.h"

#include "application.h"
#include "printer.h"
#include "timer.h"
#include "util.h"

GameState::GameState()
{
}

#ifdef USE_SDL
int GameState::GetKeyDown()
{
  int res = -1;

  SDL_Event evt;

  // SDL_WaitEvent() cannot be used because we have animations
  while (SDL_PollEvent(&evt))
  {
    switch(evt.type)
    {
      // To allow application closing by clicking 'X'
      // on window frame
      case SDL_QUIT:
        Application::Instance().ChangeState(GameStates::EXIT_GAME);
        break;

      case SDL_KEYDOWN:
      {
        auto& sc = evt.key.keysym.scancode;
        if (_charsByScancodes.count(sc) == 1)
        {
          res = _charsByScancodes[sc];
        }
        else if (_specialKeysByScancode.count(sc) == 1)
        {
          res = _specialKeysByScancode[sc];
        }
      }
      break;

      case SDL_KEYUP:
      {
        res = -1;
      }
      break;      
    }    
  }

  // SDL_GetModState() must be called after all events
  // have been polled
  SDL_Keymod keyMod = SDL_GetModState();
  bool shiftPressed = (keyMod & KMOD_LSHIFT || keyMod & KMOD_RSHIFT);

  if (shiftPressed)
  {
    if (!ShouldShiftMap(res))
    {
      res = std::toupper(res);
    }
  }

  return res;
}
#else
int GameState::GetKeyDown(bool waitForEvent)
{
  return getch();
}
#endif

#ifdef USE_SDL
bool GameState::ShouldShiftMap(int& key)
{
  bool res = false;

  for (auto& kvp : _shiftMapping)
  {
    if (key == kvp.first)
    {
      key = kvp.second;
      res = true;
      break;
    }
  }

  return res;
}
#endif

void GameState::DrawHeader(const std::string& header)
{
  int tw = Printer::Instance().TerminalWidth;

  for (int x = 0; x < tw; x++)
  {
    #ifdef USE_SDL
    Printer::Instance().PrintFB(x, 0, (int)NameCP437::HBAR_2, "#FFFFFF");
    #else
    Printer::Instance().PrintFB(x, 0, '-', "#FFFFFF");
    #endif
  }

  Printer::Instance().PrintFB(tw / 2, 0, header, Printer::kAlignCenter, "#FFFFFF", GlobalConstants::MessageBoxHeaderBgColor);
}
