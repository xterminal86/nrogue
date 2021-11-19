#include "ai-monster-shelob.h"

AIMonsterShelob::AIMonsterShelob()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterShelob::PrepareScript()
{
  //
  // Ranged attack task returns success if player was hit.
  // Otherwise it returns BTResult::Failure.
  //
  // By design ranged attack with applied effect shall deal 0 damage.
  // If you also need actual damage, use two attacks inside [SEQ],
  // one ranged with effect and one basic with "always hit" override.
  //
  // Please note, that ranged attack doesn't consume the turn,
  // so you can rely on its return result status to code-in
  // some additional logic as shown in this script below.
  //
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
              [COND p1="has_effect" p2="player" p3="Par"]
                [TASK p1="chase_player"]
              [COND p1="player_in_range" p2="4"]
                [SEL]
                  [TASK p1="attack_ranged" p2="STR" p3="Par" p4="3"]
                  [TASK p1="idle"]
        [TASK p1="goto_last_player_pos"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
