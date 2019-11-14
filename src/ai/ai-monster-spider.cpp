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
    [COND p1="in_range"]
      [SEL]
        [COND p1="player_visible"]
          [SEL]
            [COND p1="has_effect" p2="player" p3="Psd"]
              [TASK p1="move_away"]
            [COND p1="in_range" p2="1"]
              [TASK p1="attack_effect" p2="Psd"]
            [TASK p1="chase_player"]
        [TASK p1="goto_last_player_pos"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
