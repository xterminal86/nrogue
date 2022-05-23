#include "ai-idle.h"

AIIdle::AIIdle()
{
  _hash = typeid(*this).hash_code();
}

void AIIdle::PrepareScript()
{
  _scriptAsText =
R"(
[TREE]
  [SEL]
    [TASK p1="idle"]
)";

  /*
  _scriptCompiled =
  {
    0x00, 0x01, 0x02, 0x02, 0x04, 0x06, 0x65, 0xFF
  };
  */
}
