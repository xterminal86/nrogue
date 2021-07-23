#include "ai-monster-vampire-bat.h"

AIMonsterVampireBat::AIMonsterVampireBat()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterVampireBat::PrepareScript()
{
  _script =
R"(
[TREE]
  [SEL]r
    [COND p1="player_in_range"]
      [COND p1="player_visible"]
        [SEL]
          [COND p1="player_in_range" p2="1"]
            [SEL]
              [COND p1="player_next_turn" p2="1"]
                [TASK p1="move_away"]
              [TASK p1="attack_special" p2="hp_steal"]
          [COND p1="player_in_range" p2="2"]
            [SEL]
              [COND p1="player_next_turn" p2="1"]
                [TASK p1="idle"]
              [TASK p1="chase_player"]
          [TASK p1="chase_player"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";

}
