#include "ai-monster-bat.h"

AIMonsterBat::AIMonsterBat()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterBat::PrepareScript()
{
  //
  // NOTE: maybe a little OP for slow / melee classes.
  // You can win only if you can corner the monster,
  // if you're fast enough or if you have ranged weapon.
  //
  // BUG: looks like if SPD is equal to player's,
  // monster's behvaiour is not as expected - it doesn't
  // accumulate turns or take into account number of player turns.
  //
  _scriptAsText =
R"(
[TREE]
  [SEL]
    [COND p1="player_in_range"]
      [COND p1="player_visible"]
        [SEL]
          [COND p1="player_in_range" p2="1"]
            [SEL]
              [COND p1="player_next_turn" p2="1"]
                [TASK p1="move_away"]
              [TASK p1="attack"]
          [COND p1="player_in_range" p2="2"]
            [SEL]
              [COND p1="player_next_turn" p2="1"]
                [TASK p1="idle"]
              [TASK p1="chase_player"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";

  /*
  _scriptCompiled =
  {
    0x00, 0x01, 0x02, 0x02, 0x04, 0x07, 0x7A, 0xFF,
    0x06, 0x07, 0x78, 0xFF, 0x08, 0x02, 0x0A, 0x07,
    0x7A, 0x01, 0xFF, 0x0C, 0x02, 0x0E, 0x07, 0x7C,
    0x01, 0xFF, 0x10, 0x06, 0x68, 0xFF, 0x0E, 0x06,
    0x69, 0xFF, 0x0A, 0x07, 0x7A, 0x02, 0xFF, 0x0C,
    0x02, 0x0E, 0x07, 0x7C, 0x01, 0xFF, 0x10, 0x06,
    0x65, 0xFF, 0x0E, 0x06, 0x6D, 0xFF, 0x04, 0x06,
    0x66, 0xFF, 0x04, 0x06, 0x65, 0xFF
  };
  */
}
