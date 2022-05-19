#include "ai-monster-troll.h"

AIMonsterTroll::AIMonsterTroll()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterTroll::PrepareScript()
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
              [SEL]
                [COND p1="hp_low"]
                  [TASK p1="move_away"]
                [TASK p1="attack"]
            [COND p1="hp_low"]
              [SEL]
                [TASK p1="move_away"]
            [TASK p1="chase_player"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
