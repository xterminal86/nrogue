#include "ai-monster-herobrine.h"

AIMonsterHerobrine::AIMonsterHerobrine()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterHerobrine::PrepareScript()
{
  _script =
R"(
[TREE]
  [SEL]
    [COND p1="player_in_range" p2="1"]
      [TASK p1="attack"]
    [TASK p1="chase_player"]
    [TASK p1="idle"]
)";
}
