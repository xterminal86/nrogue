#include "ai-monster-herobrine.h"

AIMonsterHerobrine::AIMonsterHerobrine()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterHerobrine::PrepareScript()
{
  // TODO: improve

  _script =
R"(
[TREE]
  [SEL]
    [COND p1="player_in_range"]
      [SEL]
        [COND p1="player_visible"]
          [SEQ]
            [TASK p1="save_player_pos"]
            [SEL]
              [COND p1="player_in_range" p2="1"]
                [TASK p1="attack_basic"]
              [TASK p1="chase_player"]
        [TASK p1="goto_last_player_pos"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
