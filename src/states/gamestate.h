#ifndef GAMESTATE_H
#define GAMESTATE_H

#ifndef USE_SDL
#include <ncurses.h>
#else
#include <SDL2/SDL.h>
#endif

#include <algorithm>
#include <string>
#include <typeinfo>
#include <chrono>

#include "constants.h"

using Ns = std::chrono::nanoseconds;

class GameState
{
  public:
    GameState();

    virtual ~GameState() = default;

    virtual void Init() {}
    virtual void Cleanup() {}
    virtual void Prepare() {}
    virtual void HandleInput() = 0;
    virtual void Update(bool forceUpdate = false) = 0;

    int GetKeyDown(bool noRepeat = true);

  protected:
    int _keyPressed = -1;

    void DrawHeader(const std::string& header);

#ifdef USE_SDL    
    int GetKeysPressed(const Uint8* kbState);

    int ProcessKeydown(const Uint8* kbState, bool shiftPressed);
    int ProcessKeyRepeat(const Uint8* kbState, bool shiftPressed);

    bool ShouldShiftMap(int& key);

    template <typename T>
    int FindKeyIn(const T& dic, const Uint8* kbState)
    {
      for (auto& kvp : dic)
      {
        if (kbState[kvp.first])
        {
          return kvp.second;
        }
      }

      return -1;
    }

    // We need to share this variable across all
    // GameState instances
    static int _lastPressedKey;

    const Ns _inputDelay = Ns{50000};

    Ns _inputDelayCounter;

    std::map<SDL_Scancode, char> _charsByScancodes =
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

    std::map<SDL_Scancode, int> _specialKeysByScancode =
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

    std::map<char, char> _shiftMapping =
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
#endif
};

#endif
