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
    // If player is in agro range and is visible, try to chase him down
    // and attack him.
    // Otherwise move randomly or wait.
    Selector* selectorMain = new Selector(objRef);
    {
      Sequence* offenceSeq = new Sequence(objRef);
      {
        TaskPlayerInRange* taskPlayerInRange = new TaskPlayerInRange(objRef, AgroRadius);
        TaskPlayerVisible* taskPlayerVisible = new TaskPlayerVisible(objRef);

        Sequence* seqChase = new Sequence(objRef);
        {
          TaskChasePlayer* taskChasePlayer = new TaskChasePlayer(objRef);
          TaskAttackBasic* taskAttackBasic = new TaskAttackBasic(objRef);

          seqChase->AddNode(taskChasePlayer);
          seqChase->AddNode(taskAttackBasic);
        }

        offenceSeq->AddNode(taskPlayerInRange);
        offenceSeq->AddNode(taskPlayerVisible);
        offenceSeq->AddNode(seqChase);
      }

      Selector* selectorIdle = new Selector(objRef);
      {
        TaskRandomMovement* taskMove = new TaskRandomMovement(objRef);
        TaskIdle* taskIdle = new TaskIdle(objRef);

        selectorIdle->AddNode(taskMove);
        selectorIdle->AddNode(taskIdle);
      }

      selectorMain->AddNode(offenceSeq);
      selectorMain->AddNode(selectorIdle);
    }

    rootNode->SetNode(selectorMain);
  }

  _root.reset(rootNode);
}

