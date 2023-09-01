#ifndef HELPSTATE_H
#define HELPSTATE_H

#include "gamestate.h"

class HelpState : public GameState
{
  public:
    HelpState();

    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    StringV _helpText =
    {
      R"('q' - Cancel / Close menu     )",
      R"('a' - Attack                  )",
      R"('f' - Fire weapon / wand      )",
      R"('i' - Interact                )",
      R"('I' - Scenario information    )",
      R"('l' - Look around             )",
      R"('m' - Message log             )",
      R"('@' - Character sheet         )",
      R"('e' - Inventory               )",
      R"('$' - Count your money        )",
      R"('g' - Get item from the ground)",
      R"('S' - Save and exit           )",
      R"('>' - Go down the stairs      )",
      R"('<' - Go up the stairs        )",
      R"('Q' - Exit game               )",
      R"()"
    };

    StringV _keymap =
    {
      R"('q' 'w' 'e' )",
      R"(   \ | /    )",
      R"('a'-'x'-'d' or numpad for movement)",
      R"(   / | \    )",
      R"('z' 's' 'c' )",
    };

    #ifdef USE_SDL
    const std::string _specialText = "'F9' - Take screenshot";
    #endif
};

#endif // HELPSTATE_H
