#ifndef NPCINTERACTSTATE_H
#define NPCINTERACTSTATE_H

#include "gamestate.h"

class AINPC;

class NPCInteractState : public GameState
{
  public:
    void Cleanup() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void SetNPCRef(AINPC* npc);

  private:
    AINPC* _npcRef;
};

#endif // NPCINTERACTSTATE_H
