#include "ai-monster-spider.h"

AIMonsterSpider::AIMonsterSpider()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterSpider::PrepareScript()
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
              [COND p1="has_effect" p2="player" p3="Psd"]
                [SEL]
                  [TASK p1="move_away"]
                  [COND p1="player_in_range" p2="1"]
                    [TASK p1="attack_effect" p2="Psd"]
              [COND p1="player_in_range" p2="1"]
                [TASK p1="attack_effect" p2="Psd"]
              [TASK p1="chase_player"]
        [COND p1="has_effect" p2="player" p3="Psd"]
          [TASK p1="move_away"]
        [TASK p1="goto_last_player_pos"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}