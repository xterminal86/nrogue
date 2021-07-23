#include "ai-monster-bat.h"

AIMonsterBat::AIMonsterBat()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterBat::PrepareScript()
{
  // NOTE: maybe a little OP for slow / melee classes.
  // You can win only if you can corner the monster,
  // if you're fast enough or if you have ranged weapon.

  _script =
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
              [TASK p1="attack_basic"]
          [COND p1="player_in_range" p2="2"]
            [SEL]
              [COND p1="player_next_turn" p2="1"]
                [TASK p1="idle"]
              [TASK p1="chase_player"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
