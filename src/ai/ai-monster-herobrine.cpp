#include "ai-monster-herobrine.h"
#include "ai-component.h"

#include "task-attack-basic.h"

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
    [COND p1="in_range"]
      [COND p1="player_visible"]
        [SEL]
          [COND p1="in_range" p2="1"]
            [TASK p1="attack_basic"]
          [TASK p1="chase_player"]
    [TASK p1="move_rnd"]
    [TASK p1="idle"]
)";
}
