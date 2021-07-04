#include "ai-monster-bat.h"

AIMonsterBat::AIMonsterBat()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterBat::PrepareScript()
{
  // TODO: implement hit-and-run tactics by improving the script
  //
  // 1) If player is visible, approach player
  // 2) If there are enough moves to get to the player, hit him,
  // and pull back, do it
  // 3) Otherwise maintain distance until there are enough moves to do 2)
  //

  _script =
R"(
[TREE]
  [SEL]
    [COND p1="in_range"]
      [COND p1="player_visible"]
        [SEL]
          [COND p1="in_range" p2="1"]
            [SEQ]
              [TASK p1="attack_basic"]
              [SUCC]
                [COND p1="turns_left"]
                  [TASK p1="move_away"]
          [TASK p1="chase_player"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
