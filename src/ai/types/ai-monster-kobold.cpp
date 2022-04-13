#include "ai-monster-kobold.h"

AIMonsterKobold::AIMonsterKobold()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterKobold::PrepareScript()
{
  _script =
R"(
[TREE]
  [SEL]
    [COND p1="player_in_range"]
      [COND p1="player_visible"]
        [SEL]
          [COND p1="player_in_range" p2="1"]
            [SEL]
              [COND p1="hp_low"]
                [TASK p1="drink_potion" p2="HP"]
              [TASK p1="attack_basic"]
          [COND p1="hp_low"]
            [SEL]
              [TASK p1="drink_potion" p2="HP"]
              [TASK p1="move_away"]
          [COND p1="has_equipped" p2="WPN"]
            [TASK p1="chase_player"]
          [TASK p1="move_away"]
    [COND p1="hp_low"]
      [TASK p1="drink_potion" p2="HP"]
    [TASK p1="break_stuff"]
    [TASK p1="move_smart"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
