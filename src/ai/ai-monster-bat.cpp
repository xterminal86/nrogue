#include "ai-monster-bat.h"

AIMonsterBat::AIMonsterBat()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterBat::PrepareScript()
{
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
