#include "ai-idle.h"

AIIdle::AIIdle()
{
  _hash = typeid(*this).hash_code();
}

void AIIdle::PrepareScript()
{
  _script =
R"(
[TREE]
  [SEL]
    [TASK p1="idle"]
)";
}
