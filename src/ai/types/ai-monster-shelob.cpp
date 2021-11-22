#include "ai-monster-shelob.h"

AIMonsterShelob::AIMonsterShelob()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterShelob::PrepareScript()
{
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
                [TASK p1="attack_effect" p2="Psd"]
              [COND p1="has_effect" p2="player" p3="Par"]
                [TASK p1="chase_player"]
              [COND p1="player_in_range" p2="4"]
                [SEL]
                  [SEQ]
                    [TASK p1="attack_ranged" p2="NA"]
                    [TASK p1="apply_effect" p2="Par" p3="0" p4="6"]
                    [TASK p1="idle"]
                  [TASK p1="idle"]
        [TASK p1="goto_last_player_pos"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
