#include "ai-monster-basic.h"
#include "ai-component.h"
#include "application.h"
#include "map.h"
#include "rng.h"
#include "map-level-base.h"

#include "behaviour-tree.h"
#include "task-random-movement.h"
#include "task-attack-basic.h"
#include "task-idle.h"

AIMonsterBasic::AIMonsterBasic()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterBasic::PrepareScript()
{
  _script =
R"(
[TREE]
  [SEL]
    [COND p1="in_range"]
      [COND p1="player_visible"]
        [SEL]
          [COND p1="in_range" p2="1"]
            [TASK p1="attack_basic"]
          [TASK p1="chase_player"]
    [TASK p1="move_rnd"]
    [TASK p1="end"]
)";
}

