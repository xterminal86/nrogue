#include "ai-monster-smart.h"
#include "ai-component.h"

#include "behaviour-tree.h"

#include "task-attack-basic.h"
#include "task-random-movement.h"
#include "task-idle.h"

AIMonsterSmart::AIMonsterSmart()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

/*
void AIMonsterSmart::ConstructAI()
{
  auto& objRef = AIComponentRef->OwnerGameObject;

  Root* rootNode = new Root(objRef);
  {
    Selector* selectorMain = new Selector(objRef);
    {
      Selector* attackOrChase = new Selector(objRef);
      {
        Sequence* attackSeq = new Sequence(objRef);
        {
          TaskPlayerInRange* taskPlayerInRange = new TaskPlayerInRange(objRef);
          TaskAttackBasic* taskAttack = new TaskAttackBasic(objRef);

          attackSeq->AddNode(taskPlayerInRange);
          attackSeq->AddNode(taskAttack);
        }

        attackOrChase->AddNode(attackSeq);

        TaskChasePlayerSmart* taskChaseSmart = new TaskChasePlayerSmart(objRef, AgroRadius);

        attackOrChase->AddNode(taskChaseSmart);
      }

      Selector* selectorIdle = GetIdleSelector();

      selectorMain->AddNode(attackOrChase);
      selectorMain->AddNode(selectorIdle);
    }

    rootNode->SetNode(selectorMain);
  }

  _root.reset(rootNode);
}
*/
