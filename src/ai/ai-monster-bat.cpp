#include "ai-monster-bat.h"

#include "ai-component.h"

#include "task-player-in-range.h"
#include "task-player-visible.h"
#include "task-attack-basic.h"
#include "task-chase-player-smart.h"
#include "task-random-movement.h"
#include "task-idle.h"
#include "task-run-from-player.h"

AIMonsterBat::AIMonsterBat()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterBat::ConstructAI()
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
          TaskChasePlayerSmart* taskChasePlayer = new TaskChasePlayerSmart(objRef, AgroRadius);

          Sequence* seqAtk = new Sequence(objRef);
          {
            TaskPlayerInRange* taskPlayerInRange = new TaskPlayerInRange(objRef);
            TaskAttackBasic* taskAttackBasic = new TaskAttackBasic(objRef);
            TaskRunFromPlayer* taskRun = new TaskRunFromPlayer(objRef);

            seqAtk->AddNode(taskPlayerInRange);
            seqAtk->AddNode(taskAttackBasic);
            seqAtk->AddNode(taskRun);
          }

          selChase->AddNode(seqAtk);
          selChase->AddNode(taskChasePlayer);
        }

        offenceSeq->AddNode(selChase);
      }

      Selector* selectorIdle = GetIdleSelector();

      selectorMain->AddNode(offenceSeq);
      selectorMain->AddNode(selectorIdle);
    }

    rootNode->SetNode(selectorMain);
  }

  _root.reset(rootNode);
}
