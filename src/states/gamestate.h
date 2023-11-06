#ifndef GAMESTATE_H
#define GAMESTATE_H

#ifndef USE_SDL
#include <ncurses.h>
#else
#include <SDL2/SDL.h>
#endif

#include <algorithm>
#include <string>
#include <sstream>
#include <typeinfo>
#include <chrono>

#include "constants.h"

using Ns = std::chrono::nanoseconds;

// #############################################################################

#define RECORD_ACTION(key)                     \
  if (_keysToRecord.count(key) == 1)           \
  {                                            \
    Application::Instance().RecordAction(key); \
  }

// #############################################################################

#define DELAY_REPLAY()                            \
  if (Application::Instance().ReplayMode)          \
  {                                                \
    switch (Application::Instance().ReplaySpeed_) \
    {                                              \
      case ReplaySpeed::SLOW:                      \
        Util::Sleep(300);                          \
        break;                                     \
                                                   \
      case ReplaySpeed::NORMAL:                    \
        Util::Sleep(150);                          \
        break;                                     \
                                                    \
      case ReplaySpeed::FAST:                      \
        Util::Sleep(75);                           \
        break;                                     \
    }                                               \
  }

// #############################################################################

#define RECORD_NUMPAD()           \
  _keysToRecord[ALT_K1]   = true; \
  _keysToRecord[NUMPAD_1] = true; \
  _keysToRecord[ALT_K2]   = true; \
  _keysToRecord[NUMPAD_2] = true; \
  _keysToRecord[ALT_K3]   = true; \
  _keysToRecord[NUMPAD_3] = true; \
  _keysToRecord[ALT_K4]   = true; \
  _keysToRecord[NUMPAD_4] = true; \
  _keysToRecord[ALT_K5]   = true; \
  _keysToRecord[NUMPAD_5] = true; \
  _keysToRecord[ALT_K6]   = true; \
  _keysToRecord[NUMPAD_6] = true; \
  _keysToRecord[ALT_K7]   = true; \
  _keysToRecord[NUMPAD_7] = true; \
  _keysToRecord[ALT_K8]   = true; \
  _keysToRecord[NUMPAD_8] = true; \
  _keysToRecord[ALT_K9]   = true; \
  _keysToRecord[NUMPAD_9] = true;

// #############################################################################

class GameState
{
  public:
    GameState();
    virtual ~GameState() = default;

    //
    // Called once during registration of states.
    //
    virtual void Init() {}

    //
    // Called on current state right before
    // state change is going to be performed.
    //
    virtual void Cleanup() {}

    //
    // Called on new state right after
    // state change has been performed.
    //
    virtual void Prepare() {}

    //
    // Basically called every frame (see Application::Run()).
    //
    virtual void HandleInput() = 0;

    //
    // Redraws current state after user presses anything on a keyboard.
    // Override flag to force redraw.
    //
    virtual void Update(bool forceUpdate = false) = 0;

    //
    // Driven by corresponding backend (ncurses or SDL2).
    //
    int GetKeyDown();

  protected:
    int _keyPressed = -1;

    std::unordered_map<int, bool> _keysToRecord;

    void DrawHeader(const std::string& header);

    const int _tw;
    const int _th;
    const int _twHalf;
    const int _twQuarter;
    const int _thHalf;
    const int _thQuarter;

#ifdef USE_SDL
    bool ShouldShiftMap(int& key);

    void TakeScreenshot();

    void AdjustWindowSize(const SDL_Event& evt);

    std::unordered_map<SDL_Scancode, char> _charsByScancodes =
    {
      { SDL_SCANCODE_GRAVE,        '`'  },
      { SDL_SCANCODE_1,            '1'  },
      { SDL_SCANCODE_2,            '2'  },
      { SDL_SCANCODE_3,            '3'  },
      { SDL_SCANCODE_4,            '4'  },
      { SDL_SCANCODE_5,            '5'  },
      { SDL_SCANCODE_6,            '6'  },
      { SDL_SCANCODE_7,            '7'  },
      { SDL_SCANCODE_8,            '8'  },
      { SDL_SCANCODE_9,            '9'  },
      { SDL_SCANCODE_0,            '0'  },
      { SDL_SCANCODE_MINUS,        '-'  },
      { SDL_SCANCODE_EQUALS,       '='  },
      { SDL_SCANCODE_Q,            'q'  },
      { SDL_SCANCODE_W,            'w'  },
      { SDL_SCANCODE_E,            'e'  },
      { SDL_SCANCODE_R,            'r'  },
      { SDL_SCANCODE_T,            't'  },
      { SDL_SCANCODE_Y,            'y'  },
      { SDL_SCANCODE_U,            'u'  },
      { SDL_SCANCODE_I,            'i'  },
      { SDL_SCANCODE_O,            'o'  },
      { SDL_SCANCODE_P,            'p'  },
      { SDL_SCANCODE_LEFTBRACKET,  '['  },
      { SDL_SCANCODE_RIGHTBRACKET, ']'  },
      { SDL_SCANCODE_A,            'a'  },
      { SDL_SCANCODE_S,            's'  },
      { SDL_SCANCODE_D,            'd'  },
      { SDL_SCANCODE_F,            'f'  },
      { SDL_SCANCODE_G,            'g'  },
      { SDL_SCANCODE_H,            'h'  },
      { SDL_SCANCODE_J,            'j'  },
      { SDL_SCANCODE_K,            'k'  },
      { SDL_SCANCODE_L,            'l'  },
      { SDL_SCANCODE_SEMICOLON,    ';'  },
      { SDL_SCANCODE_APOSTROPHE,   '\'' },
      { SDL_SCANCODE_BACKSLASH,    '\\' },
      { SDL_SCANCODE_Z,            'z'  },
      { SDL_SCANCODE_X,            'x'  },
      { SDL_SCANCODE_C,            'c'  },
      { SDL_SCANCODE_V,            'v'  },
      { SDL_SCANCODE_B,            'b'  },
      { SDL_SCANCODE_N,            'n'  },
      { SDL_SCANCODE_M,            'm'  },
      { SDL_SCANCODE_COMMA,        ','  },
      { SDL_SCANCODE_PERIOD,       '.'  },
      { SDL_SCANCODE_SLASH,        '/'  }
    };

    std::unordered_map<SDL_Scancode, int> _specialKeysByScancode =
    {
      { SDL_SCANCODE_SPACE,     SDLK_SPACE     },
      { SDL_SCANCODE_RETURN,    SDLK_RETURN    },
      { SDL_SCANCODE_RETURN2,   SDLK_RETURN2   },
      { SDL_SCANCODE_BACKSPACE, SDLK_BACKSPACE },
      { SDL_SCANCODE_TAB,       SDLK_TAB       },
      { SDL_SCANCODE_KP_1,      SDLK_KP_1      },
      { SDL_SCANCODE_KP_2,      SDLK_KP_2      },
      { SDL_SCANCODE_KP_3,      SDLK_KP_3      },
      { SDL_SCANCODE_KP_4,      SDLK_KP_4      },
      { SDL_SCANCODE_KP_5,      SDLK_KP_5      },
      { SDL_SCANCODE_KP_6,      SDLK_KP_6      },
      { SDL_SCANCODE_KP_7,      SDLK_KP_7      },
      { SDL_SCANCODE_KP_8,      SDLK_KP_8      },
      { SDL_SCANCODE_KP_9,      SDLK_KP_9      },
      { SDL_SCANCODE_UP,        SDLK_UP        },
      { SDL_SCANCODE_DOWN,      SDLK_DOWN      },
      { SDL_SCANCODE_LEFT,      SDLK_LEFT      },
      { SDL_SCANCODE_RIGHT,     SDLK_RIGHT     }
    };

    std::unordered_map<char, char> _shiftMapping =
    {
      { '`', '~' },
      { '1', '!' },
      { '2', '@' },
      { '3', '#' },
      { '4', '$' },
      { '5', '%' },
      { '6', '^' },
      { '7', '&' },
      { '8', '*' },
      { '9', '(' },
      { '0', ')' },
      { '-', '_' },
      { '=', '+' },
      { '/', '?' },
      { '.', '>' },
      { ',', '<' }
    };

    SDL_Rect _renderDst;
#endif
};

#endif
