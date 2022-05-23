#include "ai-monster-herobrine.h"

AIMonsterHerobrine::AIMonsterHerobrine()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterHerobrine::PrepareScript()
{
  _scriptAsText =
R"(
[TREE]
  [SEL]
    [COND p1="player_in_range" p2="1"]
      [TASK p1="attack"]
    [TASK p1="chase_player"]
    [TASK p1="idle"]
)";

  /*
  _scriptCompiled =
  {
    0x00, 0x01, 0x02, 0x02, 0x04, 0x07, 0x7A, 0x01,
    0xFF, 0x06, 0x06, 0x69, 0xFF, 0x04, 0x06, 0x6D,
    0xFF, 0x04, 0x06, 0x65, 0xFF
  };
  */
}
