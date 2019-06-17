#include "ai-monster-basic.h"
#include "ai-component.h"
#include "application.h"
#include "map.h"
#include "rng.h"
#include "map-level-base.h"

#include "behaviour-tree.h"
#include "task-player-visible.h"
#include "task-player-in-range.h"
#include "task-random-movement.h"
#include "task-chase-player.h"
#include "task-attack-basic.h"
#include "task-idle.h"

AIMonsterBasic::AIMonsterBasic()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterBasic::ConstructAI()
{
  auto& objRef = AIComponentRef->OwnerGameObject;

  Root* rootNode = new Root(objRef);
  {
    Selector* selectorMain = new Selector(objRef);
    {
      Sequence* offenceSeq = new Sequence(objRef);
      {
        // Player is in agro range and is visible.
        TaskPlayerInRange* taskPlayerInRange = new TaskPlayerInRange(objRef, AgroRadius);
        TaskPlayerVisible* taskPlayerVisible = new TaskPlayerVisible(objRef);

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

          // Otherwise chase him down.
          TaskChasePlayer* taskChasePlayer = new TaskChasePlayer(objRef);
          selChase->AddNode(taskChasePlayer);
        }

        offenceSeq->AddNode(taskPlayerInRange);
        offenceSeq->AddNode(taskPlayerVisible);
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

