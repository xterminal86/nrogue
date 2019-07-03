#include "ai-monster-herobrine.h"

AIMonsterHerobrine::AIMonsterHerobrine()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterHerobrine::ConstructAI()
{
  AIModelBase::ConstructAI();
}
