#include "ai-monster-wraith.h"

AIMonsterWraith::AIMonsterWraith()
{
  _hash = typeid(*this).hash_code();
  IsAgressive = true;
}

void AIMonsterWraith::PrepareScript()
{
  _script =
R"(
[TREE]
  [SEL]
    [COND p1="player_in_range"]
      [SEL]
        [COND p1="player_in_range" p2="1"]
          [TASK p1="attack" p2="1"]
        [TASK p1="chase_player"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
