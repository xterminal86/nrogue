#include "gamestate.h"

#include "map.h"
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
  auto& dws = Game::gPrnt.GetDefaultWindowSize();
  _renderDst = { 0, 0, dws.first, dws.second };
#endif
}

// =============================================================================

#ifdef USE_SDL
int GameState::GetKeyDown()
{
  int res = -1;

  SDL_Event evt;

  //
  // SDL_WaitEvent() cannot be used because we have animations (we do, lol).
  //
  while (SDL_PollEvent(&evt))
  //if (SDL_WaitEvent(&evt))
  {
    switch(evt.type)
    {
      //
      // To allow application to be closed by clicking 'X'
      // on a window frame
      //
      case SDL_QUIT:
        Game::gApp.ChangeState(GameStates::EXIT_GAME);
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
          GameStates s = GameStates::MESSAGE_BOX_STATE;
          if (Game::gApp.CurrentStateIs(s))
          {
            Game::gApp.CloseMessageBox();
          }

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
            Game::gApp.ForceDrawCurrentState();
            break;
        }
      }
      break;
    }
  }

  //
  // SDL_GetModState() must be called after all events
  // have been polled.
  // (I guess I read it somewhere, SDL2 documentation says nothing...)
  //
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

// =============================================================================

#ifdef USE_SDL
void GameState::AdjustWindowSize(const SDL_Event& evt)
{
  int ww = evt.window.data1;
  int wh = evt.window.data2;

  int wx = 0;
  int wy = 0;

  if (Game::gApp.GameConfig.PreserveAspectRatio)
  {
    double newHeight = (double)wh / Game::gPrnt.GraphicsWindowAspectRatio;
    double newWidth  = newHeight * Game::gPrnt.GraphicsWindowAspectRatio;

    int adjustedWidth  = (int)(newWidth * Game::gPrnt.GraphicsWindowAspectRatio);
    int adjustedHeight = (int)(newHeight * Game::gPrnt.GraphicsWindowAspectRatio);

    int dw = ww - adjustedWidth;
    int dh = wh - adjustedHeight;

    wx = dw / 2;
    wy = dh / 2;
    ww = adjustedWidth;
    wh = adjustedHeight;
  }

  Game::gPrnt.ResizedWindowSize() = { ww, wh };

  _renderDst.x = wx;
  _renderDst.y = wy;
  _renderDst.w = ww;
  _renderDst.h = wh;

  Game::gPrnt.SetRenderDst(_renderDst);
  Game::gApp.ForceDrawCurrentState();
}

// =============================================================================

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

// =============================================================================

void GameState::TakeScreenshot()
{
  auto r = Game::gPrnt.Renderer;
  SDL_Surface* sshot = SDL_CreateRGBSurface(
    0,
    Game::gPrnt.ResizedWindowSize().first,
    Game::gPrnt.ResizedWindowSize().second,
    32,
    0x00FF0000,
    0x0000FF00,
    0x000000FF,
    0xFF000000
  );

  //
  // Setting second parameter to nullptr leads to double free error on taking
  // a screenshot after screen size change.
  //
  int rc = SDL_RenderReadPixels(r,
                                &_renderDst,
                                SDL_PIXELFORMAT_ARGB8888,
                                sshot->pixels,
                                sshot->pitch);
  if (rc != 0)
  {
    ConsoleLog("[ERR] failed to take screenshot: '%s'", SDL_GetError());
    return;
  }

  std::string time = Util::GetCurrentDateTimeString(true);
  std::string fname = Util::StringFormat("s_%s.bmp", time.data());
  SDL_SaveBMP(sshot, fname.data());
  SDL_FreeSurface(sshot);

  Game::gApp.ShowMessageBox(MessageBoxType::WAIT_FOR_INPUT,
                            "Screenshot Taken",
                            { fname },
                            Colors::MessageBoxBlueBorder);
  DebugLog("Wrote %s", fname.data());
}
#endif

// =============================================================================

void GameState::DrawHeader(const std::string& header)
{
  int tw = Printer::TerminalWidth;

  for (int x = 0; x < tw; x++)
  {
    #ifdef USE_SDL
    Game::gPrnt.PrintChar(
      x,
      0,
      (int)NameCP437::HBAR_2,
      Colors::White,
      Colors::Black
    );
    #else
    Game::gPrnt.PrintFB(x,
                        0,
                        ACS_HLINE,
                        Colors::WhiteColor,
                        Colors::BlackColor);
    #endif
  }

  Game::gPrnt.PrintText(
    tw / 2,
    0,
    header,
    Printer::kAlignCenter,
    Colors::White,
    Colors::MessageBoxHeaderBg
  );
}

// =============================================================================

void GameState::MoveCursor(Position& cursorPosition, int dx, int dy)
{
  auto& playerRef = Game::gApp.PlayerInstance;

  int nx = cursorPosition.X + dx;
  int ny = cursorPosition.Y + dy;

#ifdef USE_SDL
  static int hw = Printer::GraphicsWindowWidth  / 2;
  static int hh = Printer::GraphicsWindowHeight / 2;
#else
  static int hw = _twHalf;
  static int hh = _thHalf;
#endif

  //
  // To compensate for cursor image.
  //
  nx = Util::Clamp(nx, playerRef.PosX - hw, playerRef.PosX + hw - 1);
  ny = Util::Clamp(ny, playerRef.PosY - hh, playerRef.PosY + hh - 1);

  cursorPosition.X = nx;
  cursorPosition.Y = ny;
}

// =============================================================================

void GameState::DrawCursor(const Position& cursorPosition)
{
#ifdef USE_SDL
  if (Game::gApp.AppData.UseGraphics)
  {
    Game::gPrnt.DrawGraphicsTile(
      cursorPosition.X + Game::gMap.CurrentLevel->MapOffsetX,
      cursorPosition.Y + Game::gMap.CurrentLevel->MapOffsetY,
      GraphicTiles::GUI_LOOK_CURSOR
    );
  }
  else
  {
    Game::gPrnt.DrawSubstituteGraphicsTile(
      cursorPosition.X + Game::gMap.CurrentLevel->MapOffsetX - 1,
      cursorPosition.Y + Game::gMap.CurrentLevel->MapOffsetY - 1,
      (int)NameCP437::ULCORNER_1
    );

    Game::gPrnt.DrawSubstituteGraphicsTile(
      cursorPosition.X + Game::gMap.CurrentLevel->MapOffsetX + 1,
      cursorPosition.Y + Game::gMap.CurrentLevel->MapOffsetY - 1,
      (int)NameCP437::URCORNER_1
    );

    Game::gPrnt.DrawSubstituteGraphicsTile(
      cursorPosition.X + Game::gMap.CurrentLevel->MapOffsetX - 1,
      cursorPosition.Y + Game::gMap.CurrentLevel->MapOffsetY + 1,
      (int)NameCP437::DLCORNER_1
    );

    Game::gPrnt.DrawSubstituteGraphicsTile(
      cursorPosition.X + Game::gMap.CurrentLevel->MapOffsetX + 1,
      cursorPosition.Y + Game::gMap.CurrentLevel->MapOffsetY + 1,
      (int)NameCP437::DRCORNER_1
    );
  }
#else
  Game::gPrnt.PrintChar(
    cursorPosition.X + Game::gMap.CurrentLevel->MapOffsetX + 1,
    cursorPosition.Y + Game::gMap.CurrentLevel->MapOffsetY,
    '<',
    Colors::WhiteColor,
    Colors::BlackColor
  );

  Game::gPrnt.PrintChar(
    cursorPosition.X + Game::gMap.CurrentLevel->MapOffsetX - 1,
    cursorPosition.Y + Game::gMap.CurrentLevel->MapOffsetY,
    '>',
    Colors::WhiteColor,
    Colors::BlackColor
  );
#endif
}

// =============================================================================

void GameState::DisplayGameLog()
{
  int x = Printer::TerminalWidth - 1;
  int y = Printer::TerminalHeight;

  int count = 0;
  auto msgs = Game::gPrnt.GetLastMessages();
  for (GameLogMessageData* m : msgs)
  {
    if (m == nullptr)
    {
      break;
    }

    Game::gPrnt.PrintText(
      x,
      y - Game::gPrnt.GetLastMessagesCount() + count,
      m->Message,
      Printer::kAlignRight,
      m->FgColor,
      m->BgColor
    );

    count++;
  }
}
