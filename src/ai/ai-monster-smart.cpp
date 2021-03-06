#include "ai-monster-smart.h"

AIMonsterSmart::AIMonsterSmart()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterSmart::PrepareScript()
{
  _script =
R"(
[TREE]
  [SEL]
    [COND p1="player_in_range"]
      [SEL]
        [COND p1="player_visible"]
          [SEL]
            [COND p1="player_in_range" p2="1"]
              [TASK p1="attack_basic"]
            [TASK p1="chase_player"]
        [TASK p1="goto_last_player_pos"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
