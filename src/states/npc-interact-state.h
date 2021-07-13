#ifndef NPCINTERACTSTATE_H
#define NPCINTERACTSTATE_H

#include <vector>
#include <string>

#include "gamestate.h"

class AINPC;

class NPCInteractState : public GameState
{  
  public:
    void Prepare() override;
    void Cleanup() override;
    void HandleInput() override;
    void Update(bool forceUpdate = false) override;
    void SetNPCRef(AINPC* npc);

  private:
    bool _textPrinting = false;
    int _gossipBlockIndex = 0;

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
      SERVICES,
      NONE
    };

    WhatKey _whatKey = WhatKey::NONE;

    std::vector<std::string> _blockToPrint;

    size_t _charPos = 0;
    size_t _currentLine = 0;
};

#endif // NPCINTERACTSTATE_H
