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
  // "player_in_range" checks if player is specified number of
  // tiles away or closer, so if you need to perform different
  // tasks at different ranges, put check of the closest range
  // first in the tree.
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

  /*
  _scriptCompiled =
  {
    0x00, 0x01, 0x02, 0x02, 0x04, 0x07, 0x7A, 0xFF,
    0x06, 0x07, 0x78, 0xFF, 0x08, 0x02, 0x0A, 0x07,
    0x7A, 0x01, 0xFF, 0x0C, 0x06, 0x69, 0xFF, 0x0A,
    0x06, 0x6D, 0xFF, 0x04, 0x06, 0x66, 0xFF, 0x04,
    0x06, 0x65, 0xFF
  };
  */
}

