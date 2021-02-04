#include "gamestate.h"
#include "printer.h"
#include "timer.h"
#include "util.h"

#ifdef USE_SDL
int GameState::_lastPressedKey = SDL_NUM_SCANCODES;
#endif

GameState::GameState()
{
}

#ifdef USE_SDL
int GameState::GetKeyDown(bool noRepeat)
{
  int res = -1;

  // We don't want all SDL events, just the keyboard state,
  // so no 'while (SDL_PollEvent()) {}' or anything.
  SDL_PumpEvents();

  // From SDL2 documentation:
  // "Note: This function gives you the current state
  // after all events have been processed,
  // so if a key or button has been pressed and released
  // before you process events,
  // then the pressed state will never show up
  // in the SDL_GetKeyboardState() calls."
  //
  // TLDR: keyboard state querying must be done after
  // any form of SDL_PumpEvents()
  const Uint8* kbState = SDL_GetKeyboardState(nullptr);
  SDL_Keymod keyMod = SDL_GetModState();

  bool shiftPressed = (keyMod & KMOD_LSHIFT || keyMod & KMOD_RSHIFT);

  if (noRepeat)
  {
    res = ProcessKeydown(kbState, shiftPressed);
  }
  else
  {
    res = ProcessKeyRepeat(kbState, shiftPressed);
  }

  return res;
}
#endif

#ifndef USE_SDL
int GameState::GetKeyDown(bool waitForEvent)
{
  return getch();
}
#endif

#ifdef USE_SDL
int GameState::ProcessKeydown(const Uint8* kbState, bool shiftPressed)
{
  int res = -1;

  // At first I wanted to make fast movement only when
  // user holds Shift and arrow keys. But it didn't work.
  // If we try to made this check inside if (shiftPressed) {}
  // block, for some reason key spamming happens only
  // AFTER you press desired key and THEN Shift,
  // not Shift first and then desired key.
  // So I decided to go with OS-like style.
  //
  // As if SDL_PumpEvents() collects character data first
  // and special keys data second somehow, because it all should
  // happen in one update, so wtf?
  //
  res = FindKeyIn(_specialKeysByScancode, kbState);
  if (res == -1)
  {
    res = FindKeyIn(_charsByScancodes, kbState);
  }

  if (shiftPressed)
  {
    if (!ShouldShiftMap(res))
    {
      res = std::toupper(res);
    }
  }

  bool isNew = (res != _lastPressedKey
             && res != std::tolower(_lastPressedKey)
             && res != std::toupper(_lastPressedKey));

  if (isNew)
  {
    //DebugLog("[NEW] %i\n", res);
    _lastPressedKey = res;
  }  
  else
  {
    res = -1;
  }

  return res;
}

int GameState::ProcessKeyRepeat(const Uint8* kbState, bool shiftPressed)
{
  int res = -1;

  res = FindKeyIn(_specialKeysByScancode, kbState);
  if (res == -1)
  {
    res = FindKeyIn(_charsByScancodes, kbState);
  }

  if (res != -1)
  {
    if (shiftPressed && !ShouldShiftMap(res))
    {
      res = std::toupper(res);
    }

    if (_inputDelayCounter == Ns{0} || _inputDelayCounter > _inputDelay)
    {
      _lastPressedKey = res;
    }
    else
    {
      res = -1;
    }

    // For some reason this works, but if we try to accumulate
    // Timer::DeltaTime(), it doesn't.
    // Probably has something to do with my accumulation
    // of delta time in nanoseconds and subsequent overflow
    // of this counter or something.
    //
    // Also, input delay value is different, it seems like 300ms
    // is equal to Ns{30000} for this implementation.
    //
    // std::chrono is a piece of shit.
    //
    _inputDelayCounter++;
  }
  else
  {
    if (_inputDelayCounter != Ns{0})
    {
      _inputDelayCounter = Ns{0};
    }
  }

  return res;
}

int GameState::GetKeysPressed(const Uint8* kbState)
{
  int count = 0;

  for (auto& kvp : _charsByScancodes)
  {
    if (kbState[kvp.first])
    {
      count++;
    }
  }

  for (auto& kvp : _specialKeysByScancode)
  {
    if (kbState[kvp.first])
    {
      count++;
    }
  }

  return count;
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
