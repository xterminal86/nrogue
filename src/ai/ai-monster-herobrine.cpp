#include "ai-monster-herobrine.h"
#include "ai-component.h"

#include "task-attack-basic.h"

AIMonsterHerobrine::AIMonsterHerobrine()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

/*
void AIMonsterHerobrine::ConstructAI()
{
  auto& objRef = AIComponentRef->OwnerGameObject;

  Root* rootNode = new Root(objRef);
  {
    Selector* selectorMain = new Selector(objRef);
    {
      Sequence* offenceSeq = new Sequence(objRef);
      {
        Selector* selChase = new Selector(objRef);
        {
          // If player is in 1 tile range, attack him.
          Sequence* seqAtk = new Sequence(objRef);
          {
            TaskPlayerInRange* taskPlayerInRange = new TaskPlayerInRange(objRef);
            TaskAttackBasic* taskAttackBasic = new TaskAttackBasic(objRef);

            seqAtk->AddNode(taskPlayerInRange);
            seqAtk->AddNode(taskAttackBasic);
          }

          selChase->AddNode(seqAtk);

          TaskChasePlayer* taskChasePlayer = new TaskChasePlayer(objRef);
          selChase->AddNode(taskChasePlayer);
        }

        offenceSeq->AddNode(selChase);
      }

      // Since we start in closed room,
      // there should be at least idle state for enemy.
      Selector* selectorIdle = GetIdleSelector();

      selectorMain->AddNode(offenceSeq);
      selectorMain->AddNode(selectorIdle);
    }

    rootNode->SetNode(selectorMain);
  }

  _root.reset(rootNode);
}
*/
