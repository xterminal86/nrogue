#include "ai-monster-basic.h"

AIMonsterBasic::AIMonsterBasic()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterBasic::PrepareScript()
{
  //
  // NOTE: fail status of the child node
  // will propagate to the topmost control tag.
  //
  // E.g. in the script below selector has 3 child nodes
  // and will return true (and thus perform action) if
  // any one of the children nodes return true after iterating
  // over them in order of their enumeration.
  // If first child returned false, selector will go to its
  // next child until someone returns true.
  // So to avoid infinite loop, make sure that your script
  // always has "fallback" task that returns true
  // and make controlled object finish its turn.
  // Usually it's the last task in selector.
  //
  _script =
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
}

