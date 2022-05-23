#include "gamestate.h"

#include "application.h"
#include "printer.h"
#include "timer.h"
#include "util.h"

GameState::GameState() :
  _tw(Printer::TerminalWidth),
  _th(Printer::TerminalHeight),
  _twHalf(_tw / 2),
  _twQuarter(_tw / 4),
  _thHalf(_th / 2),
  _thQuarter(_th / 4)
{
#ifdef USE_SDL
  auto& dws = Application::Instance().GetDefaultWindowSize();
  _renderDst = { 0, 0, dws.first, dws.second };
#endif
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

        if (sc == SDL_SCANCODE_F9)
        {
          TakeScreenshot();
        }
      }
      break;

      case SDL_KEYUP:
      {
        res = -1;
      }
      break;

      case SDL_WINDOWEVENT:
      {
        switch (evt.window.event)
        {
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            AdjustWindowSize(evt);
            break;

          case SDL_WINDOWEVENT_RESTORED:
          case SDL_WINDOWEVENT_EXPOSED:
            Application::Instance().ForceDrawCurrentState();
            break;
        }
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
int GameState::GetKeyDown()
{
  return getch();
}
#endif

#ifdef USE_SDL
void GameState::AdjustWindowSize(const SDL_Event& evt)
{
  int ww = evt.window.data1;
  int wh = evt.window.data2;

  auto& tws = Printer::Instance().GetTileWHScaled();

  bool wOk = (std::abs(ww - _renderDst.w) > tws.first);
  bool hOk = (std::abs(wh - _renderDst.h) > tws.second);

  if (wOk && hOk)
  {
    ww -= (ww % tws.first);
    wh -= (wh % tws.second);

    _renderDst.w = ww;
    _renderDst.h = wh;

    Printer::Instance().SetRenderDst(_renderDst);
    Application::Instance().ForceDrawCurrentState();
  }
}

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

void GameState::TakeScreenshot()
{
  auto& app = Application::Instance();
  auto r = Application::Instance().Renderer;
  auto ws = app.GetDefaultWindowSize();
  SDL_Surface* sshot = SDL_CreateRGBSurface(0,
                                            ws.first,
                                            ws.second,
                                            32,
                                            0x00FF0000,
                                            0x0000FF00,
                                            0x000000FF,
                                            0xFF000000);
  SDL_RenderReadPixels(r,
                       nullptr,
                       SDL_PIXELFORMAT_ARGB8888,
                       sshot->pixels,
                       sshot->pitch);
  std::string time = Util::GetCurrentDateTimeString();
  std::string fname = Util::StringFormat("s_%s.bmp", time.data());
  SDL_SaveBMP(sshot, fname.data());
  SDL_FreeSurface(sshot);
  DebugLog("Wrote %s", fname.data());
}
#endif

void GameState::DrawHeader(const std::string& header)
{
  int tw = Printer::TerminalWidth;

  for (int x = 0; x < tw; x++)
  {
    #ifdef USE_SDL
    Printer::Instance().PrintFB(x,
                                0,
                                (int)NameCP437::HBAR_2,
                                Colors::WhiteColor);
    #else
    Printer::Instance().PrintFB(x,
                                0,
                                ACS_HLINE,
                                Colors::WhiteColor);
    #endif
  }

  Printer::Instance().PrintFB(tw / 2,
                              0,
                              header,
                              Printer::kAlignCenter,
                              Colors::WhiteColor,
                              Colors::MessageBoxHeaderBgColor);
}
