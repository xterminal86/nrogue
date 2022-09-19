#include "ai-monster-bat.h"

AIMonsterBat::AIMonsterBat()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterBat::PrepareScript()
{
  //
  // Bat AI is replaced with basic attack pattern.
  //

  _scriptAsText =
R"(
[TREE]
  [SEL]
    [COND p1="player_in_range"]
      [COND p1="player_visible"]
        [SEL]
          [COND p1="player_in_range" p2="1"]
            [TASK p1="attack"]
          [TASK p1="chase_player"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";

  // NOTE: "archer" AI pseudocode

/*
  _scriptAsText =
R"(
[TREE]
  [SEL]
    [COND p1="player_in_range"]
      [COND p1="player_visible"]
        [SEL]
          [COND p1="player_in_range" p2="1"]
            [SEL]
              [COND p1="turns_check" p2="2" p3="lt"]
                [TASK p1="move_away"]
              [TASK p1="attack_melee"]
          [COND p1="player_in_range" p2="2"]
            [SEL]
              [COND p1="turns_check" p2="3" p3="ge"]
                [TASK p1="chase_player"]
              [COND p1="has_ammo"]
                [TASK p1="attack_ranged"]
          [TASK p1="move_away"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
*/

}
