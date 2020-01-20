#ifndef HELPSTATE_H
#define HELPSTATE_H

#include "gamestate.h"

class HelpState : public GameState
{
  public:
    void Prepare() override;

    void HandleInput() override;
    void Update(bool forceUpdate = false) override;

  private:
    void DrawScrollBars();

    int _scrollPosition = 0;

    StringsArray2D _helpText =
    {
      R"('Q' - Cancel / Close menu     )",
      R"('A' - Attack                  )",
      R"('f' - Fire weapon / wand      )",
      R"('i' - Interact                )",
      R"('l' - Enter "look mode"       )",
      R"('m' - Display message log     )",
      R"('@' - Display character sheet )",
      R"('E' - Display inventory       )",
      R"('$' - Count your money        )",
      R"('g' - Get item from the ground)",
      R"('>' - Go down the stairs      )",
      R"('<' - Go up the stairs        )",
      R"('Q' - Exit game               )",
      R"()",
      R"('q' 'w' 'e' )",
      R"(   \ | /    )",
      R"('a'-'x'-'d' or keypad for movement)",
      R"(   / | \    )",
      R"('z' 's' 'c' )",
    };
};

#endif // HELPSTATE_H
