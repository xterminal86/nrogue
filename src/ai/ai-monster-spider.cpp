#include "ai-monster-spider.h"

#include "ai-component.h"

#include "task-player-in-range.h"
#include "task-player-has-effect.h"
#include "task-player-visible.h"
#include "task-random-movement.h"
#include "task-run-from-player.h"
#include "task-idle.h"
#include "task-attack-effect.h"
#include "task-chase-player-smart.h"

AIMonsterSpider::AIMonsterSpider()
{
  _hash = typeid(*this).hash_code();

  IsAgressive = true;
}

void AIMonsterSpider::ConstructAI()
{
  auto& objRef = AIComponentRef->OwnerGameObject;

  Root* rootNode = new Root(objRef);
  {
    Selector* selectorMain = new Selector(objRef);
    {
      Sequence* seqAttack = new Sequence(objRef);
      {
        TaskPlayerInRange* taskRange = new TaskPlayerInRange(objRef, AgroRadius);
        TaskPlayerVisible* taskVis = new TaskPlayerVisible(objRef);

        seqAttack->AddNode(taskRange);
        seqAttack->AddNode(taskVis);

        Selector* selOffence = new Selector(objRef);
        {
          Sequence* seqCheckEffect = new Sequence(objRef);
          {
            TaskPlayerHasEffect* taskEffect = new TaskPlayerHasEffect(objRef, EffectType::POISONED);
            TaskRunFromPlayer* taskRun = new TaskRunFromPlayer(objRef);

            seqCheckEffect->AddNode(taskEffect);
            seqCheckEffect->AddNode(taskRun);
          }

          Sequence* shouldAttack = new Sequence(objRef);
          {
            // FIXME: poison chance / strength calculation.

            Effect e =
            {
              EffectType::POISONED,
              objRef->Attrs.Lvl.CurrentValue,
              objRef->Attrs.Lvl.CurrentValue * 2,
              true,
              false
            };

            std::map<Effect, int> em;
            em.emplace(e, 50);

            TaskPlayerInRange* taskRange = new TaskPlayerInRange(objRef);
            TaskAttackEffect* attackEffect = new TaskAttackEffect(objRef, em);

            shouldAttack->AddNode(taskRange);
            shouldAttack->AddNode(attackEffect);
          }

          TaskChasePlayerSmart* taskChasePlayer = new TaskChasePlayerSmart(objRef, AgroRadius);

          selOffence->AddNode(seqCheckEffect);
          selOffence->AddNode(shouldAttack);
          selOffence->AddNode(taskChasePlayer);
        }

        seqAttack->AddNode(selOffence);
      }

      Selector* selIdle = GetIdleSelector();

      selectorMain->AddNode(seqAttack);
      selectorMain->AddNode(selIdle);
    }

    rootNode->SetNode(selectorMain);
  }

  _root.reset(rootNode);
}
