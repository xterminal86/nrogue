#ifndef NPCINTERACTSTATE_H
#define NPCINTERACTSTATE_H

#include <vector>
#include <string>

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
    bool _textPrinting = false;
    int _gossipBlockIndex = 0;

    std::vector<std::string> _animatedTextFinished;

    AINPC* _npcRef;

    void DisplayStillText();
    void AnimateText();
    void PrintHeader();
    void PrintFooter();

    enum class WhatKey
    {
      NAME = 0,
      JOB,
      GOSSIP,
      NONE
    };

    WhatKey _whatKey = WhatKey::NONE;
};

#endif // NPCINTERACTSTATE_H
